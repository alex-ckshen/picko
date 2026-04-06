/*
*  HUSKYLENS resolution ratio X320*Y240.
*/
//-- define for serial.print
//#define PRT_LINKIT
//#define PRT_HUSKY
#define PRT_RPM
#include <HUSKYLENS.h>
#include <LRemote.h>
#include <Servo.h>
#define PIN_EINT_LEFT   2  //-- left speed detetor to EINT0
#define PIN_EINT_RIGHT  3  //-- right speed detetor to EINT1
#define PIN_SERVO       5
#define PIN_USER_BTN    6
//#define LED_BUILTIN   7
#define PIN_BUZZER      14
#define PIN_LIFT_FW     10 //-- driver A, INA attached forward   of left motor 
#define PIN_LIFT_BW     12 //-- driver A, INB attached backward  of left motor 
#define PIN_RIGHT_FW    13 //-- driver B, INB attached forward   of right motor
#define PIN_RIGHT_BW    17 //-- driver B, INA attached backkward of right motor

const int c = 261;
const int d = 294;
const int e = 329;
const int f = 349;
const int g = 391;
const int gS = 415;
const int a = 440;
const int aS = 455;
const int b = 466;
const int cH = 523;
const int cSH = 554;
const int dH = 587;
const int dSH = 622;
const int eH = 659;
const int fH = 698;
const int fSH = 740;
const int gH = 784;
const int gSH = 830;
const int aH = 880;

//LRemoteLabel    LKickLabel;
//LRemoteLabel    LSlowLabel;
//LRemoteLabel    LFastLabel;

LRemoteSwitch   switchLKick;
LRemoteSwitch   switchLSlow;
LRemoteSwitch   switchLFast;

LRemoteSlider   sliderRKick;
LRemoteSlider   sliderRSlow;
LRemoteSlider   sliderRFast;

LRemoteSlider   sliderWidth;
LRemoteSlider   sliderXoffset;

LRemoteLabel    labelDoffset;
LRemoteLabel    labelDwidth;

LRemoteLabel    labelUp;
LRemoteLabel    labelDright;

LRemoteSwitch   switchStart;
LRemoteSwitch   switchTrack;
LRemoteSwitch   switchSearch;
LRemoteButton   buttonPick;

LRemoteLabel    labelMessage;
LRemoteLabel    labelLspeed;
LRemoteLabel    labelRspeed;
LRemoteJoyStick stickManual;

HUSKYLENS          huskylens;
Servo              servo_pickp;  
void printResult   (HUSKYLENSResult result);
void dir_motor_pwm (unsigned int pwm_left_fw, unsigned int pwm_left_bw, unsigned int pwm_right_fw, unsigned int pwm_right_bw);

boolean motor_forward;
boolean motor_backward;
boolean obj_available;
boolean obj_got;
boolean act_search;
boolean act_stop;
boolean button_status;
int button_cntr;
int search_cntr;
int preX;
int preY;
int curX;
int curY;
long previousMillis = 0;
int  led_state = LOW;
long led_interval = 500;  

//-- pwm to rpm
const int     grid_num = 20;
const int     rpm_interval = 1000; //-- ms
unsigned int  isr_count_left;
unsigned int  isr_count_right;
unsigned long start_time;
unsigned long stop_time;
unsigned int  cur_dir_left; 
unsigned int  cur_dir_right;
unsigned int  cur_pwm_left;
unsigned int  cur_pwm_right;
unsigned int  pre_rpm_left;
unsigned int  pre_rpm_right;
unsigned int  cur_rpm_left;
unsigned int  cur_rpm_right;
int           err_rpm_left  = (pre_rpm_left  - cur_rpm_left);
int           err_rpm_right = (pre_rpm_right - cur_rpm_right);
boolean       sam_rpm_left  = (err_rpm_left  == 0);
boolean       sam_rpm_right = (err_rpm_right == 0);
boolean       sty_rpm_left  = (err_rpm_left  < 3 && err_rpm_left  > -3);
boolean       sty_rpm_right = (err_rpm_right < 3 && err_rpm_right > -3);
boolean       cur_stop_left  = (cur_pwm_left == 0);
boolean       cur_stop_right = (cur_pwm_right == 0);
unsigned int  rpm_phase;

void setup() {
//-- Initialize serial and wait for port to open:
  Serial.begin(9600);
  Serial.println("Start configuring PICKO!");  
//-- 先停止輸出動作或伺服歸位
//-- attach motor and stop them
  pinMode(PIN_LIFT_FW,  OUTPUT);
  pinMode(PIN_LIFT_BW,  OUTPUT);
  pinMode(PIN_RIGHT_FW, OUTPUT);
  pinMode(PIN_RIGHT_BW, OUTPUT);
  dir_motor_pwm( 0, 0, 0, 0);

//-- attch pickup servo and set to high position 
  servo_pickp.attach(PIN_SERVO);
  servo_pickp.write(80);
  
//-- attach Buzzer and mute it
  pinMode(PIN_BUZZER, OUTPUT);
  noTone(PIN_BUZZER);
  
//-- attach USRR led and turn it off
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

//-- attach USER button
  pinMode(PIN_USER_BTN, INPUT);

//-- Setup the Remote Control's UI canvas
  LRemote.setName("PICKO_v1");
  LRemote.setOrientation(RC_LANDSCAPE);
  LRemote.setGrid(6, 4);

//LKickLabel.setPos(0, 0);
//LKickLabel.setSize(1, 1);
//LKickLabel.setText("LK 130");
//LKickLabel.setColor(RC_BLUE);
//LRemote.addControl(LKickLabel);
//
//LSlowLabel.setPos(1, 0);
//LSlowLabel.setSize(1, 1);
//LSlowLabel.setText("LS 20");
//LSlowLabel.setColor(RC_BLUE);
//LRemote.addControl(LSlowLabel);
//
//LFastLabel.setPos(2, 0);
//LFastLabel.setSize(1, 1);
//LFastLabel.setText("LF 20");
//LFastLabel.setColor(RC_BLUE);
//LRemote.addControl(LFastLabel);

  switchLKick.setPos(0, 0);
  switchLKick.setSize(1, 1);
  switchLKick.setColor(RC_ORANGE);
  switchLKick.setText("130");
  LRemote.addControl(switchLKick);
  
  switchLSlow.setPos(1, 0);
  switchLSlow.setSize(1, 1);
  switchLSlow.setColor(RC_ORANGE);
  switchLSlow.setText("20");
  LRemote.addControl(switchLSlow);

  switchLFast.setPos(2, 0);
  switchLFast.setSize(1, 1);
  switchLFast.setColor(RC_ORANGE);
  switchLFast.setText("20");
  LRemote.addControl(switchLFast);

  sliderRKick.setPos(0, 1);
  sliderRKick.setSize(1, 1);
  sliderRKick.setText("RK");
  sliderRKick.setValueRange(-10, 10, 0);
  sliderRKick.setColor(RC_BLUE);
  LRemote.addControl(sliderRKick);
  
  sliderRSlow.setPos(1, 1);
  sliderRSlow.setSize(1, 1);
  sliderRSlow.setText("RS");
  sliderRSlow.setValueRange(-10, 10, 0);
  sliderRSlow.setColor(RC_BLUE);
  LRemote.addControl(sliderRSlow);

  sliderRFast.setPos(2, 1);
  sliderRFast.setSize(1, 1);
  sliderRFast.setText("RF");
  sliderRFast.setValueRange(-10, 10, 0);
  sliderRFast.setColor(RC_BLUE);
  LRemote.addControl(sliderRFast);
  
  sliderWidth.setPos(0, 2);
  sliderWidth.setSize(1, 1);
  sliderWidth.setText("Width");
  sliderWidth.setValueRange(0, 320, 0);
  sliderWidth.setColor(RC_BLUE);
  LRemote.addControl(sliderWidth);

  labelDwidth.setPos(1, 2);
  labelDwidth.setSize(1, 1);
  labelDwidth.setColor(RC_GREY);
  labelDwidth.setText("0");
  LRemote.addControl(labelDwidth);

  labelUp.setPos(2, 2);
  labelUp.setSize(1, 1);
  labelUp.setColor(RC_GREY);
  labelUp.setText("0");
  LRemote.addControl(labelUp);
  
  sliderXoffset.setPos(0, 3);
  sliderXoffset.setSize(1, 1);
  sliderXoffset.setText("CTR");
  sliderXoffset.setValueRange(140, 180, 140);
  sliderXoffset.setColor(RC_BLUE);
  LRemote.addControl(sliderXoffset);

  labelDoffset.setPos(1, 3);
  labelDoffset.setSize(1, 1);
  labelDoffset.setColor(RC_GREY);
  labelDoffset.setText("0");
  LRemote.addControl(labelDoffset);

  labelDright.setPos(2, 3);
  labelDright.setSize(1, 1);
  labelDright.setColor(RC_GREY);
  labelDright.setText("0");
  LRemote.addControl(labelDright);
  
  labelMessage.setPos(4, 0);
  labelMessage.setSize(2, 1);
  labelMessage.setColor(RC_BLUE);
  labelMessage.setText("Press to Start");
  LRemote.addControl(labelMessage);
  
  labelLspeed.setPos(4, 1);
  labelLspeed.setSize(1, 1);
  labelLspeed.setColor(RC_GREY);
  labelLspeed.setText("0");
  LRemote.addControl(labelLspeed);
 
  labelRspeed.setPos(5, 1);
  labelRspeed.setSize(1, 1);
  labelRspeed.setColor(RC_GREY);
  labelRspeed.setText("0");
  LRemote.addControl(labelRspeed);
  
  stickManual.setPos(4, 2);
  stickManual.setSize(2, 2);
  stickManual.setColor(RC_BLUE);
  LRemote.addControl(stickManual);

  switchStart.setPos(3, 0);
  switchStart.setSize(1, 1);
  switchStart.setColor(RC_ORANGE);
  switchStart.setText("Start");
  LRemote.addControl(switchStart);
  
  switchTrack.setPos(3, 1);
  switchTrack.setSize(1, 1);
  switchTrack.setColor(RC_ORANGE);
  switchTrack.setText("Track");
  LRemote.addControl(switchTrack);

  switchSearch.setPos(3, 2);
  switchSearch.setSize(1, 1);
  switchSearch.setColor(RC_ORANGE);
  switchSearch.setText("Search");
  LRemote.addControl(switchSearch);

  buttonPick.setPos(3, 3);
  buttonPick.setSize(1, 1);
  buttonPick.setColor(RC_ORANGE);
  buttonPick.setText("Pick");
  LRemote.addControl(buttonPick);

//-- Start broadcasting our remote contoller
//-- This method implicitly initialized underlying BLE subsystem
//-- to create a BLE peripheral, and then
//-- start advertisement on it.
  LRemote.begin();
  Serial.println("Linkit LRemote started!");
  
  Wire.begin();
  Serial.println("Huskylens started!");
  while (!huskylens.begin(Wire))  {
    Serial.println(F("Huskylens begin failed!"));
    Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>I2C)"));
    Serial.println(F("2.Please recheck the connection."));
    delay(100);
  }

  huskylens.writeAlgorithm(ALGORITHM_OBJECT_TRACKING); //Switch the algorithm to object tracking.
//huskylens.writeAlgorithm(ALGORITHM_COLOR_RECOGNITION);

//-- initialize the variables we're linked to
  obj_available = false;
  obj_got = false;
  act_search = false;
  act_stop = false;
  preX = 1000;
  preY = 1000;
  curX = 1000;
  curY = 1000;
  button_status = false;
  button_cntr = 0;
  search_cntr = 0;

//-- init variables for ISR
  cur_dir_left  = 1; 
  cur_dir_right = 1;
  cur_pwm_left  = 0;
  cur_pwm_right = 0;
  pre_rpm_left  = 0;
  pre_rpm_right = 0;
  cur_rpm_left  = 0;
  cur_rpm_right = 0;
  sty_rpm_left  = false;
  sty_rpm_right = false;
  isr_count_left = 0;
  isr_count_right = 0;
  start_time = 0;
  stop_time = 0;
  attachInterrupt( PIN_EINT_LEFT,  ISR_counter_left,  FALLING);
  attachInterrupt( PIN_EINT_RIGHT, ISR_counter_right, FALLING);
  rpm_phase = 0;

//firstSection_a();
}

void pickup() {
  servo_pickp.write(175);
  delay(500);
  servo_pickp.write(80);  
}

// Buzzer
void buzzer () {  
  tone(PIN_BUZZER, 523, 500);
  delay(100);
  noTone(PIN_BUZZER);
}

void beep (int note, int duration)
{
  //Play tone on PIN_BUZZER
  tone(PIN_BUZZER, note, duration);
  delay(duration);
  //Stop tone on PIN_BUZZER
  noTone(PIN_BUZZER);
  delay(50);
}

void firstSection_a() {
  beep(a, 500);
  beep(a, 500);
  beep(a, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 650);
}

void firstSection_b() {
  beep(eH, 500);
  beep(eH, 500);
  beep(eH, 500);
  beep(fH, 350);
  beep(cH, 150);
  beep(gS, 500);
  beep(f, 350);
  beep(cH, 150);
  beep(a, 650);
}

void secondSection() {
  beep(aH, 500);
  beep(a, 300);
  beep(a, 150);
  beep(aH, 500);
  beep(gSH, 325);
  beep(gH, 175);
  beep(fSH, 125);
  beep(fH, 125);
  beep(fSH, 250);

  delay(325);

  beep(aS, 250);
  beep(dSH, 500);
  beep(dH, 325);
  beep(cSH, 175);
  beep(cH, 125);
  beep(b, 125);
  beep(cH, 250);

  delay(350);
}
//-
void printResult(HUSKYLENSResult result){
  if (result.command == COMMAND_RETURN_BLOCK){
    Serial.println(String()+F("Block:xCenter=")+result.xCenter+F(",yCenter=")+result.yCenter+F(",width=")
                   +result.width+F(",height=")+result.height+F(",ID=")+result.ID);
  } else if (result.command == COMMAND_RETURN_ARROW){
    Serial.println(String()+F("Arrow:xOrigin=")+result.xOrigin+F(",yOrigin=")+result.yOrigin+F(",xTarget=")
                   +result.xTarget+F(",yTarget=")+result.yTarget+F(",ID=")+result.ID);
  } else{
    Serial.println("Object unknown!");
  }
}

void dir_motor_pwm (unsigned int pwm_left_fw, unsigned int pwm_left_bw, unsigned int pwm_right_fw, unsigned int pwm_right_bw) {
  analogWrite(PIN_LIFT_FW,  pwm_left_fw);
  analogWrite(PIN_LIFT_BW,  pwm_left_bw);
  analogWrite(PIN_RIGHT_FW, pwm_right_fw);
  analogWrite(PIN_RIGHT_BW, pwm_right_bw);
}

void prot_motor_pwm (unsigned int pwm_left_fw, unsigned int pwm_left_bw, unsigned int pwm_right_fw, unsigned int pwm_right_bw) {
  int leftSpeed;
  int rightSpeed;

  if ( act_search && obj_available ) {
    dir_motor_pwm ( 0, 0, 0, 0);
  } else {
    dir_motor_pwm ( pwm_left_fw, pwm_left_bw, pwm_right_fw, pwm_right_bw);
  }
  
  if ( pwm_left_fw == 0 && pwm_right_fw == 0 ) motor_forward = false;
  else                                         motor_forward = true; 

  if ( pwm_left_bw == 0 && pwm_right_bw == 0 ) motor_backward = false;
  else                                         motor_backward = true; 

  if ( pwm_left_fw == 0 )  leftSpeed = pwm_left_bw;
  else                     leftSpeed = pwm_left_fw;
  
  if ( pwm_right_fw == 0 ) rightSpeed = pwm_right_bw;
  else                     rightSpeed = pwm_right_fw;

  labelLspeed.updateText(String(leftSpeed)); 
  labelRspeed.updateText(String(rightSpeed));
}

void forward_break() {
  prot_motor_pwm( 80, 0, 80, 0);
  delay(200);
  prot_motor_pwm( 50, 0, 50, 0);
  delay(100);
  prot_motor_pwm( 0, 0, 0, 0);
}

void backward_break() {
  prot_motor_pwm( 0, 80, 0, 80);
  delay(200);
  prot_motor_pwm( 0, 50, 0, 50);
  delay(100);
  prot_motor_pwm( 0, 0, 0, 0);
}

void motor_break() {
  if ( motor_forward ) { 
    forward_break();
  } else if ( motor_backward ) {
    backward_break();
  }
  prot_motor_pwm( 0, 0, 0, 0);
}

void joystick_motor_pwm() {
  if( stickManual.isValueChanged() ) {
    LRemoteDirection d = stickManual.getValue();
    int left_fw  = 100+d.y+d.x;
    int left_bw  = 100-d.y+d.x;
    int right_fw = 100+d.y-d.x;
    int right_bw = 100-d.y-d.x;
    
    if ( d.y > 0 ) {
      prot_motor_pwm( left_fw, 0, right_fw, 0);
    } else if ( d.y < 0 ) {
      prot_motor_pwm( 0, left_bw, 0, right_bw);
    } else {
      prot_motor_pwm( 0, 0, 0, 0);
    }
  }
}

void ISR_counter_left() {
  isr_count_left++;
}

void ISR_counter_right() {
  isr_count_right++;
}

void ISR_rpm() {
  pre_rpm_left  = cur_rpm_left;
  pre_rpm_right = cur_rpm_right;
  
//-- 計算 rpm 時，停止計數
  detachInterrupt( PIN_EINT_LEFT );
  detachInterrupt( PIN_EINT_RIGHT );
  stop_time = millis();

//-- (偵測的格數count / 一圈網格數20) / (時間差 / 1000)  
  cur_rpm_left  = isr_count_left  * (60 * 1000 / grid_num ) / (stop_time - start_time);
  cur_rpm_right = isr_count_right * (60 * 1000 / grid_num ) / (stop_time - start_time);   
  
//-- Restart the interrupt processing
  isr_count_left = 0;
  isr_count_right = 0;
  attachInterrupt( PIN_EINT_LEFT,  ISR_counter_left,  FALLING);
  attachInterrupt( PIN_EINT_RIGHT, ISR_counter_right, FALLING);
  start_time = millis();

//-- 輸出至Console
#ifdef PRT_RPM
  Serial.print("\n-cur_dir_left  = ");
  Serial.print(cur_dir_left, DEC);
  Serial.print("; cur_rpm_left   = ");
  Serial.print(cur_rpm_left, DEC);
  Serial.print("; cur_pwm_left   = ");
  Serial.print(cur_pwm_left, DEC);

  Serial.print("\n-cur_dir_right = ");
  Serial.print(cur_dir_right, DEC);
  Serial.print("; cur_rpm_right  = ");
  Serial.print(cur_rpm_right, DEC);
  Serial.print("; cur_pwm_right  = ");
  Serial.print(cur_pwm_right, DEC);

  Serial.print("\n");
#endif
}

void motor_rpm( unsigned int rpm_left_fw, unsigned int rpm_left_bw, unsigned int rpm_right_fw, unsigned int rpm_right_bw ) {
//-- max pwm is limited under 200
//-- rpm around 130 when pwm 200
  int dif_rpm_left;
  int dif_rpm_right;
  
  if ( rpm_left_fw == 0 && rpm_left_bw == 0 ) {         //-- left stop
    cur_dir_left = 1;
    dif_rpm_left = 0;
  } else if ( cur_dir_left == 1 && rpm_left_bw > 0 ) {  //-- left forward to backward
    cur_dir_left = 0;
    dif_rpm_left = rpm_left_bw;
  } else if ( cur_dir_left == 0 && rpm_left_fw > 0 ) {  //-- left backward to forward
    cur_dir_left = 1;
    dif_rpm_left = rpm_left_fw;
  } else if ( cur_dir_left == 1 && rpm_left_bw == 0 ) { //-- left forwarding
    cur_dir_left = 1;
    dif_rpm_left = rpm_left_fw - cur_rpm_left;
  } else if ( cur_dir_left == 0 && rpm_left_fw == 0 ) { //-- left backwarding
    cur_dir_left = 0;
    dif_rpm_left = rpm_left_bw - cur_rpm_left;
  }

  if ( rpm_right_fw == 0 && rpm_right_bw == 0 ) {         //-- right stop
    cur_dir_right = 1;
    dif_rpm_right = 0;
  } else if ( cur_dir_right == 1 && rpm_right_bw > 0 ) {  //-- right forward to backward
    cur_dir_right = 0;
    dif_rpm_right = rpm_right_bw;
  } else if ( cur_dir_right == 0 && rpm_right_fw > 0 ) {  //-- right backward to forward
    cur_dir_right = 1;
    dif_rpm_right = rpm_right_fw;
  } else if ( cur_dir_right == 1 && rpm_right_bw == 0 ) { //-- right forwarding
    cur_dir_right = 1;
    dif_rpm_right = rpm_right_fw - cur_rpm_right;
  } else if ( cur_dir_right == 0 && rpm_right_fw == 0 ) { //-- right backwarding
    cur_dir_right = 0;
    dif_rpm_right = rpm_right_bw - cur_rpm_right;
  }
  
  if ( rpm_left_fw == 0 && rpm_left_bw == 0 ) {        //-- left stop
    cur_pwm_left = 0;
  } else if ( cur_pwm_left == 0 && (rpm_left_fw != 0 || rpm_left_bw != 0) ) { //-- kick off from stop
    cur_pwm_left = 100;
  } else if ( sam_rpm_left && dif_rpm_left > 0 ) {
    if ( cur_pwm_left < 199 ) cur_pwm_left = cur_pwm_left + 1;
    else                      cur_pwm_left = 200;
  } else if ( sam_rpm_left && dif_rpm_left < 0 ) {
    if ( cur_pwm_left > 1 )   cur_pwm_left = cur_pwm_left - 1;
    else                      cur_pwm_left = 0;
  }

  if ( rpm_right_fw == 0 && rpm_right_bw == 0 ) {        //-- right stop
    cur_pwm_right = 0;
  } else if ( cur_pwm_right == 0 && (rpm_right_fw != 0 || rpm_right_bw != 0)) { //-- kick off from stop
    cur_pwm_right = 100;
  } else if ( sam_rpm_right && dif_rpm_right > 0 ) {
    if ( cur_pwm_right < 199 ) cur_pwm_right = cur_pwm_right + 1;
    else                       cur_pwm_right = 200;
  } else if ( sam_rpm_right && dif_rpm_right < 0 ) {
    if ( cur_pwm_right > 1 )   cur_pwm_right = cur_pwm_right - 1;
    else                       cur_pwm_right = 0;
  }

/*
  if ( rpm_left_fw == 0 && rpm_left_bw == 0 ) {        //-- left stop
    cur_pwm_left = 0;
  } else if ( cur_pwm_left == 0 ) {                    //-- kick off from stop
    cur_pwm_left = 150;
  } else if ( dif_rpm_left > 10 ) {
    if ( cur_pwm_left < 180 ) cur_pwm_left = cur_pwm_left + 20;
    else                      cur_pwm_left = 200;
  } else if ( (dif_rpm_left > 2) && (dif_rpm_left <= 10) ) {
    if ( cur_pwm_left < 197 ) cur_pwm_left = cur_pwm_left + 3;
    else                      cur_pwm_left = 200;
  } else if ( dif_rpm_left >= -10 && dif_rpm_left < -2 ) {
    if ( cur_pwm_left > 3 )   cur_pwm_left = cur_pwm_left - 3;
    else                      cur_pwm_left = 0;
  } else if ( dif_rpm_left < -10 ) {
    if ( cur_pwm_left > 20 )  cur_pwm_left = cur_pwm_left - 20;
    else                      cur_pwm_left = 0;
  }

  if ( rpm_right_fw == 0 && rpm_right_bw == 0 ) {         //-- right stop
    cur_pwm_right = 0;
  } else if ( cur_pwm_right == 0 ) {                      //-- kick off from stop
    cur_pwm_right = 150;
  } else if ( dif_rpm_right > 10 ) {
    if ( cur_pwm_right < 180 ) cur_pwm_right = cur_pwm_right + 20;
    else                       cur_pwm_right = 200;
  } else if ( (dif_rpm_right > 2) && (dif_rpm_right <= 10) ) {
    if ( cur_pwm_right < 197 ) cur_pwm_right = cur_pwm_right + 3;
    else                       cur_pwm_right = 200;
  } else if ( dif_rpm_right >= -10 && dif_rpm_right < -2 ) {
    if ( cur_pwm_right > 3 )   cur_pwm_right = cur_pwm_right - 3;
    else                       cur_pwm_right = 0;
  } else if ( dif_rpm_right < -10 ) {
    if ( cur_pwm_right > 20 )  cur_pwm_right = cur_pwm_right - 20;
    else                       cur_pwm_right = 0;
  }
*/  
#ifdef PRT_RPM
  Serial.print("\n cur_dir_left  = ");
  Serial.print(cur_dir_left, DEC);
  Serial.print("; cur_rpm_left   = ");
  Serial.print(cur_rpm_left, DEC);
  Serial.print("; cur_pwm_left   = ");
  Serial.print(cur_pwm_left, DEC);

  Serial.print("\n cur_dir_right = ");
  Serial.print(cur_dir_right, DEC);
  Serial.print("; cur_rpm_right  = ");
  Serial.print(cur_rpm_right, DEC);
  Serial.print("; cur_pwm_right  = ");
  Serial.print(cur_pwm_right, DEC);

  Serial.print("\n");
#endif

  if ( cur_dir_left == 1 && cur_dir_right == 1 ) {
    dir_motor_pwm( cur_pwm_left, 0, cur_pwm_right, 0);
  } else if ( cur_dir_left == 1 && cur_dir_right == 0 ) {
    dir_motor_pwm( cur_pwm_left, 0, 0, cur_pwm_right);
  } else if ( cur_dir_left == 0 && cur_dir_right == 1 ) {
    dir_motor_pwm( 0, cur_pwm_left, cur_pwm_right, 0);
  } else if ( cur_dir_left == 0 && cur_dir_right == 0 ) {
    dir_motor_pwm( 0, cur_pwm_left, 0, cur_pwm_right);
  }
}

void auto_motor_rpm( unsigned int rpm_left_fw, unsigned int rpm_left_bw, unsigned int rpm_right_fw, unsigned int rpm_right_bw ) {
//-- max pwm is limited under 200
//-- rpm around 130 when pwm 200
  int dif_rpm_left;
  int dif_rpm_right;
  unsigned int pre_pwm_left;
  unsigned int pre_pwm_right;

  pre_rpm_left  = cur_rpm_left;
  pre_rpm_right = cur_rpm_right;
  pre_pwm_left  = cur_pwm_left;
  pre_pwm_right = cur_pwm_right;
  
//-- 計算 rpm 時，停止計數
  detachInterrupt( PIN_EINT_LEFT );
  detachInterrupt( PIN_EINT_RIGHT );
  stop_time = millis();

//-- (偵測的格數count / 一圈網格數20) / (時間差 / 1000)  
  cur_rpm_left  = isr_count_left  * (60 * 1000 / grid_num ) / (stop_time - start_time);
  cur_rpm_right = isr_count_right * (60 * 1000 / grid_num ) / (stop_time - start_time);   
  
//-- Restart the interrupt processing
  isr_count_left = 0;
  isr_count_right = 0;
  attachInterrupt( PIN_EINT_LEFT,  ISR_counter_left,  FALLING);
  attachInterrupt( PIN_EINT_RIGHT, ISR_counter_right, FALLING);
  start_time = millis();

//-- 輸出至Console
#ifdef PRT_RPM
  Serial.print("\n-pre_dir_left  = ");
  Serial.print(cur_dir_left, DEC);
  Serial.print("; pre_rpm_left   = ");
  Serial.print(pre_rpm_left, DEC);
  Serial.print("; pre_pwm_left   = ");
  Serial.print(pre_pwm_left, DEC);

  Serial.print("\n-pre_dir_right = ");
  Serial.print(cur_dir_right, DEC);  
  Serial.print("; pre_rpm_right  = ");
  Serial.print(pre_rpm_right, DEC);
  Serial.print("; pre_pwm_right  = ");
  Serial.print(pre_pwm_right, DEC);
  Serial.print("\n");
#endif
  
  if ( rpm_left_fw == 0 && rpm_left_bw == 0 ) {         //-- left stop
    cur_dir_left = 1;
    dif_rpm_left = 0;
  } else if ( cur_dir_left == 1 && rpm_left_bw > 0 ) {  //-- left forward to backward
    cur_dir_left = 0;
    dif_rpm_left = rpm_left_bw;
  } else if ( cur_dir_left == 0 && rpm_left_fw > 0 ) {  //-- left backward to forward
    cur_dir_left = 1;
    dif_rpm_left = rpm_left_fw;
  } else if ( cur_dir_left == 1 && rpm_left_bw == 0 ) { //-- left forwarding
    cur_dir_left = 1;
    dif_rpm_left = rpm_left_fw - cur_rpm_left;
  } else if ( cur_dir_left == 0 && rpm_left_fw == 0 ) { //-- left backwarding
    cur_dir_left = 0;
    dif_rpm_left = rpm_left_bw - cur_rpm_left;
  }

  if ( rpm_right_fw == 0 && rpm_right_bw == 0 ) {         //-- right stop
    cur_dir_right = 1;
    dif_rpm_right = 0;
  } else if ( cur_dir_right == 1 && rpm_right_bw > 0 ) {  //-- right forward to backward
    cur_dir_right = 0;
    dif_rpm_right = rpm_right_bw;
  } else if ( cur_dir_right == 0 && rpm_right_fw > 0 ) {  //-- right backward to forward
    cur_dir_right = 1;
    dif_rpm_right = rpm_right_fw;
  } else if ( cur_dir_right == 1 && rpm_right_bw == 0 ) { //-- right forwarding
    cur_dir_right = 1;
    dif_rpm_right = rpm_right_fw - cur_rpm_right;
  } else if ( cur_dir_right == 0 && rpm_right_fw == 0 ) { //-- right backwarding
    cur_dir_right = 0;
    dif_rpm_right = rpm_right_bw - cur_rpm_right;
  }
  
  if ( rpm_left_fw == 0 && rpm_left_bw == 0 ) {        //-- left stop
    cur_pwm_left = 0;
  } else if ( cur_pwm_left == 0 && (rpm_left_fw != 0 || rpm_left_bw != 0) ) { //-- kick off from stop
    cur_pwm_left = 100;
  } else if ( sam_rpm_left && dif_rpm_left > 0 ) {
    if ( cur_pwm_left < 199 ) cur_pwm_left = cur_pwm_left + 1;
    else                      cur_pwm_left = 200;
  } else if ( sam_rpm_left && dif_rpm_left < 0 ) {
    if ( cur_pwm_left > 1 )   cur_pwm_left = cur_pwm_left - 1;
    else                      cur_pwm_left = 0;
  }

  if ( rpm_right_fw == 0 && rpm_right_bw == 0 ) {        //-- right stop
    cur_pwm_right = 0;
  } else if ( cur_pwm_right == 0 && (rpm_right_fw != 0 || rpm_right_bw != 0)) { //-- kick off from stop
    cur_pwm_right = 100;
  } else if ( sam_rpm_right && dif_rpm_right > 0 ) {
    if ( cur_pwm_right < 199 ) cur_pwm_right = cur_pwm_right + 1;
    else                       cur_pwm_right = 200;
  } else if ( sam_rpm_right && dif_rpm_right < 0 ) {
    if ( cur_pwm_right > 1 )   cur_pwm_right = cur_pwm_right - 1;
    else                       cur_pwm_right = 0;
  }

/*
  if ( rpm_left_fw == 0 && rpm_left_bw == 0 ) {        //-- left stop
    cur_pwm_left = 0;
  } else if ( cur_pwm_left == 0 ) {                    //-- kick off from stop
    cur_pwm_left = 150;
  } else if ( dif_rpm_left > 10 ) {
    if ( cur_pwm_left < 180 ) cur_pwm_left = cur_pwm_left + 20;
    else                      cur_pwm_left = 200;
  } else if ( (dif_rpm_left > 2) && (dif_rpm_left <= 10) ) {
    if ( cur_pwm_left < 197 ) cur_pwm_left = cur_pwm_left + 3;
    else                      cur_pwm_left = 200;
  } else if ( dif_rpm_left >= -10 && dif_rpm_left < -2 ) {
    if ( cur_pwm_left > 3 )   cur_pwm_left = cur_pwm_left - 3;
    else                      cur_pwm_left = 0;
  } else if ( dif_rpm_left < -10 ) {
    if ( cur_pwm_left > 20 )  cur_pwm_left = cur_pwm_left - 20;
    else                      cur_pwm_left = 0;
  }

  if ( rpm_right_fw == 0 && rpm_right_bw == 0 ) {         //-- right stop
    cur_pwm_right = 0;
  } else if ( cur_pwm_right == 0 ) {                      //-- kick off from stop
    cur_pwm_right = 150;
  } else if ( dif_rpm_right > 10 ) {
    if ( cur_pwm_right < 180 ) cur_pwm_right = cur_pwm_right + 20;
    else                       cur_pwm_right = 200;
  } else if ( (dif_rpm_right > 2) && (dif_rpm_right <= 10) ) {
    if ( cur_pwm_right < 197 ) cur_pwm_right = cur_pwm_right + 3;
    else                       cur_pwm_right = 200;
  } else if ( dif_rpm_right >= -10 && dif_rpm_right < -2 ) {
    if ( cur_pwm_right > 3 )   cur_pwm_right = cur_pwm_right - 3;
    else                       cur_pwm_right = 0;
  } else if ( dif_rpm_right < -10 ) {
    if ( cur_pwm_right > 20 )  cur_pwm_right = cur_pwm_right - 20;
    else                       cur_pwm_right = 0;
  }
*/  
#ifdef PRT_RPM
  Serial.print("\n cur_dir_left  = ");
  Serial.print(cur_dir_left, DEC);
  Serial.print("; cur_rpm_left   = ");
  Serial.print(cur_rpm_left, DEC);
  Serial.print("; cur_pwm_left   = ");
  Serial.print(cur_pwm_left, DEC);

  Serial.print("\n cur_dir_right = ");
  Serial.print(cur_dir_right, DEC);
  Serial.print("; cur_rpm_right  = ");
  Serial.print(cur_rpm_right, DEC);
  Serial.print("; cur_pwm_right  = ");
  Serial.print(cur_pwm_right, DEC);

  Serial.print("\n");
#endif

  if ( cur_dir_left == 1 && cur_dir_right == 1 ) {
    dir_motor_pwm( cur_pwm_left, 0, cur_pwm_right, 0);
  } else if ( cur_dir_left == 1 && cur_dir_right == 0 ) {
    dir_motor_pwm( cur_pwm_left, 0, 0, cur_pwm_right);
  } else if ( cur_dir_left == 0 && cur_dir_right == 1 ) {
    dir_motor_pwm( 0, cur_pwm_left, cur_pwm_right, 0);
  } else if ( cur_dir_left == 0 && cur_dir_right == 0 ) {
    dir_motor_pwm( 0, cur_pwm_left, 0, cur_pwm_right);
  }
}

void joystick_motor_rpm() {
  if( stickManual.isValueChanged() ) {
    LRemoteDirection d = stickManual.getValue();
    int left_fw  = 100+d.y+d.x;
    int left_bw  = 100-d.y+d.x;
    int right_fw = 100+d.y-d.x;
    int right_bw = 100-d.y-d.x;
    
    if ( d.y > 0 ) {
      motor_rpm( left_fw, 0, right_fw, 0);
    } else if ( d.y < 0 ) {
      motor_rpm( 0, left_bw, 0, right_bw);
    } else {
      motor_rpm( 0, 0, 0, 0);
    }
  }
}

//--
void object_searching_pwm() {
  prot_motor_pwm( 200, 0, 200, 0);
  delay(500);
  prot_motor_pwm( 200, 0, 150, 0);
  delay(500);
  prot_motor_pwm( 200, 0, 100, 0);
  delay(500);
  prot_motor_pwm( 0, 0, 0, 0);
  delay(1000);
}

//--
void object_searching_rpm() {
  motor_rpm( 100, 0, 100, 0);
  delay(500);
  motor_rpm( 100, 0, 50, 0);
  delay(500);
  motor_rpm( 0, 0, 0, 0);
  delay(1000);
}

#define MAX_WIDTH  20
#define MIN_WIDTH  3
#define MAX_OFFSET 15
#define MIN_OFFSET 3
#define MKICK      115

void object_tracking( int diffWidth, int diffXcenter ) {

int rKick; 
int rSlow;
int rFast;

  if ( switchLKick.getValue() ) {
    rKick = 20+sliderRKick.getValue();
  } else {
    rKick = 10;
  }

  if ( switchLSlow.getValue() ) {
    rSlow = 20+sliderRSlow.getValue();
  } else {
    rSlow = 20;
  }

  if ( switchLFast.getValue() ) {
    rFast = sliderRFast.getValue();
  } else {
    rFast = 20;
  }

  if ( diffWidth <= MAX_WIDTH*(-1) ) {
    if ( motor_forward ) forward_break();
    //prot_motor_pwm( 0, MKICK+rKick+rSlow, 0, MKICK+20+20); //145, 155
      prot_motor_pwm( 0, 150+8, 0, 150 );
  
  } else if ( (diffWidth > MAX_WIDTH*(-1)) && (diffWidth <= MIN_WIDTH*(-1)) ) {
    if ( motor_forward ) forward_break();
    //prot_motor_pwm( 0, MKICK+rKick, 0, MKICK+20); //125, 135
      prot_motor_pwm( 0, 130+10, 0, 130 );

  } else if ( (diffWidth > MIN_WIDTH*(-1)) && (diffWidth < MIN_WIDTH) ) {
    motor_break();

  } else if ( diffWidth >= MIN_WIDTH && diffWidth < MAX_WIDTH ) {
    if ( diffXcenter >= MAX_OFFSET ) {
      //prot_motor_pwm( MKICK+rKick, 0, MKICK+20+20+20, 0); //125, 175
      prot_motor_pwm( 130+10, 0, 170, 0);

    } else if ( (diffXcenter < MAX_OFFSET) && (diffXcenter >= MIN_OFFSET) ) {
      //prot_motor_pwm( MKICK+rKick, 0, MKICK+20+20, 0); //125, 155
      prot_motor_pwm( 130+10, 0, 150, 0);

    } else if ( (diffXcenter <= MIN_OFFSET*(-1)) && (diffXcenter > MAX_OFFSET*(-1)) ) {
      //prot_motor_pwm( MKICK+rKick+rSlow, 0, MKICK+20, 0); //145, 135
      prot_motor_pwm( 150+8, 0, 130, 0);
      
    } else if ( diffXcenter <= MAX_OFFSET*(-1)) {
      //prot_motor_pwm( MKICK+rKick+rSlow+rFast, 0, MKICK+20, 0); //165, 135
      prot_motor_pwm( 170+5, 0, 130, 0);
      
    } else {
      //prot_motor_pwm( MKICK+rKick, 0, MKICK+20, 0); //125, 135
      prot_motor_pwm( 130+10, 0, 130, 0);
    
    }
    
  } else if ( diffWidth >=  MAX_WIDTH ) {
    if ( (diffXcenter >= MIN_OFFSET) ) {
      //prot_motor_pwm( MKICK+rKick, 0, MKICK+20+20, 0);
      prot_motor_pwm( 130+10, 0, 150, 0);

    } else if ( (diffXcenter <= MIN_OFFSET*(-1))) {
      //prot_motor_pwm( MKICK+rKick+rSlow, 0, MKICK+20, 0);
      prot_motor_pwm( 150+8, 0, 130, 0);
      
    } else {
      //prot_motor_pwm( MKICK+rKick, 0, MKICK+20, 0); //125, 135
      prot_motor_pwm( 130+10, 0, 130, 0);
      
    }
  } else {
    motor_break();
    delay(500);
    
  }
}

void tracking_message ( int diffWidth, int diffXcenter ) {
    
  if ( diffWidth <= MAX_WIDTH*(-1) ) {
    labelMessage.updateText("Fast Backward!");
  
  } else if ( (diffWidth > MAX_WIDTH*(-1)) && (diffWidth <= MIN_WIDTH*(-1)) ) {
    labelMessage.updateText("Slow Backward!");
 
  } else if ( (diffWidth > MIN_WIDTH*(-1)) && (diffWidth < MIN_WIDTH) ) {
    labelMessage.updateText("Motor Break!");

  } else if ( diffWidth >= MIN_WIDTH ) {
    if ( diffXcenter >= MAX_OFFSET ) {
      labelMessage.updateText("Fast Right!");
    } else if ( (diffXcenter < MAX_OFFSET) && (diffXcenter >= MIN_OFFSET) ) {
      labelMessage.updateText("Slow Right!");
    } else if ( (diffXcenter <= MIN_OFFSET*(-1)) && (diffXcenter > MAX_OFFSET*(-1)) ) {
      labelMessage.updateText("Slow Left!");
    } else if ( diffXcenter <= MAX_OFFSET*(-1) ) {
      labelMessage.updateText("Fast Left!");
    } else {
      labelMessage.updateText("Slow Forward!");
    }
    
  } else {
    labelMessage.updateText("Stop!");
    
  }
}

//-
void loop() {
  HUSKYLENSResult result;
  int diffWidth;
  int diffXcenter;
//-- Searching
  int diffRight;
  int diffUp;
  unsigned long currentMillis = millis();
  int expect_width;
  int expect_Xcenter;
  boolean btn_nouse  = (button_cntr == 0);
  boolean btn_both   = (button_cntr > 0);
  boolean btn_search = (button_cntr == 1);
  boolean btn_track  = (button_cntr == 2);

//-- 每秒(rpm_interval)更新rpm
//if ( millis() - start_time > rpm_interval ) auto_motor_rpm( 60, 0, 60, 0);
  if ( !button_status && digitalRead(PIN_USER_BTN) == HIGH )  {
    beep(a, 500);
    button_status = true;
    if ( button_cntr > 1 ) {
      button_cntr = 0;
    } else {
      button_cntr++;
    }
  } else if ( button_status && digitalRead(PIN_USER_BTN) == HIGH ) {
    beep(c, 500);
    button_status = false;
    if ( button_cntr > 1 ) {
      button_cntr = 0;
    } else {
      button_cntr++;
    }
  }

//switchStart.getValue() &&
//(switchSearch.getValue() && switchTrack.getValue()
  
  if ( btn_track ) {    
    if ( currentMillis - previousMillis > led_interval ) { 
      previousMillis = currentMillis;   
      if (led_state == LOW) {
        digitalWrite( LED_BUILTIN, HIGH );
        led_state = HIGH;
      } else {
        digitalWrite( LED_BUILTIN, LOW );
        led_state = LOW;
      }
    }
  } else if ( btn_search ) {
    digitalWrite( LED_BUILTIN, HIGH );
    led_state = HIGH;
  } else {
    digitalWrite( LED_BUILTIN, LOW );
    led_state = LOW;
  }
/*  
  if ( btn_nouse ) {
    expect_width   = sliderWidth.getValue();
    expect_Xcenter = sliderXoffset.getValue();
  } else {
*/  
    expect_width   = 240;
    expect_Xcenter = 160;
//}
//-- check if connected by BLE central devices,
//-- e.g. an mobile app
  if( !LRemote.connected() ) {
#ifdef PRT_LINKIT    
    Serial.println("waiting for connection");
    delay(1000);
#endif
  } else {
//-- The interval between button down/up
//-- can be very short - e.g. a quick tap
//-- on the screen.
//-- We could lose some event if we
//-- delay something like 100ms.
    delay(15);
  }
  
//-- Process the incoming BLE write request
//-- and translate them to control events
  LRemote.process();

//-- Huskylens status
  if ( !huskylens.request() ) {
#ifdef PRT_HUSKY    
    Serial.println(F("Fail to request objects from HUSKYLENS!"));
#endif
    labelMessage.updateText("No Huskeylens!");
    obj_available = false;
    preX = 1000;
    preY = 1000;
    curX = 1000;
    curY = 1000;

  } else if( !huskylens.isLearned() ) {
#ifdef PRT_HUSKY    
    Serial.println(F("Object not learned!"));
#endif
    labelMessage.updateText("Not Tracking!");
    obj_available = false;
    preX = 1000;
    preY = 1000;
    curX = 1000;
    curY = 1000;
  } else if( huskylens.available() ) {
#ifdef PRT_HUSKY
    Serial.println(F("Object Found!"));
#endif
    result = huskylens.read();
#ifdef PRT_HUSKY
    printResult(result);
#endif
//-- tracking
    obj_available = true;
    diffWidth   = (expect_width - result.width);     //> 0 forwarding
    diffXcenter = (result.xCenter - expect_Xcenter);  //> 0 turn right
    labelDoffset.updateText( String(diffXcenter) );
    labelDwidth.updateText( String(diffWidth) );

//-- searching
    preX = curX;
    preY = curY;
    curX = result.xCenter;
    curY = result.yCenter;
  } else {
//  Serial.println(F("Object disappeared!"));
    obj_available = false;
  }

  if ( preX != 1000 && preY != 1000 && preX != 1000 && preY != 1000 ) {
    diffUp    = (preY - curY);
    diffRight = (curX - preX);
  } else {
    diffRight = 0;
    diffUp  = 0;
  }

//labelUp.updateText(String(diffUp));
//labelDright.updateText(String(diffRight));
  labelUp.updateText(String(curX));
  labelDright.updateText(String(curY));
 
#ifdef PRT_HUSKY
  Serial.println(String()+F(" ,curX=")+curX
                         +F(" ,curY=")+curY
                         +F(" ,preX=")+preX
                         +F(" ,preY=")+preY
                         +F(" ,diffRight=")+diffRight
                         +F(" ,diffUp=")+diffUp);
#endif
//-- munual pickup
  if( buttonPick.isValueChanged() && (buttonPick.getValue() == 1) ) {
    pickup();
  }
//  if ( !obj_got && !obj_available && (curY > 50) && (curX > 50) && (curX < 270) ) { //&& (diffUp < 0)
  if ( (curY > 150) && (curX > 140) && (curX < 180) ) {
    //pickup();
    obj_got = true;
    beep(a, 200);    
//    beep(c, 200);
  } else if ( obj_available ) {
    obj_got = false; 
  }
     
//-- searching 
    if ( obj_available ) {
      tracking_message ( diffWidth, diffXcenter );
    } else {
      if ( obj_got ) {
        labelMessage.updateText("Obj got");
      } else if ( diffRight > 0 && diffUp > 0 ) {
        labelMessage.updateText("Right Up");
      } else if ( diffRight > 0 && diffUp < 0 ) {
        labelMessage.updateText("Right Down");
      } else if ( diffRight < 0 && diffUp > 0 ) {
        labelMessage.updateText("Left Up"); 
      } else if ( diffRight < 0 && diffUp < 0 ) {
        labelMessage.updateText("Left Down"); 
      } else {
        labelMessage.updateText("Obj lost"); 
      }
    }
    
//-- Action
  if ( switchStart.getValue() || btn_both ) {
    if ( !switchSearch.getValue() && !switchTrack.getValue() && btn_nouse ) {
      joystick_motor_pwm();
      //joystick_motor_rpm();
      
    } else if ( (switchTrack.getValue() || btn_both) && obj_available ) {
      object_tracking( diffWidth, diffXcenter );
      search_cntr = 0;
      act_stop = true;
      
    } else if ( (switchSearch.getValue() || btn_search) && !obj_available && (search_cntr < 5) && !act_stop ) {
      act_search = true;
      //object_searching_pwm();
      object_searching_pwm();
      act_search = false;
      search_cntr++;
      
    } else if ( (switchTrack.getValue() || switchSearch.getValue() || btn_both) && !obj_available && act_stop ) {
      prot_motor_pwm( 0, 0, 0, 0);
      delay(1000);
      act_stop = false;
      
    } else {
      //motor_break();
      //motor_rpm( 0, 0, 0, 0);
    } 
  } else {
    //motor_break();
    //motor_rpm( 0, 0, 0, 0);
  }

}
