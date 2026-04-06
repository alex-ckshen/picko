/*
*  HUSKYLENS resolution ratio X320*Y240.
*/
//-- define for serial.print
//#define PRT_LINKIT
//#define LREMOTE
//#define PRT_HUSKY
//#define PRT_BTN
#define PRT_RPM
//#define PRT_TRACK
//#define DEMO

//-- 
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
Servo              servo_pick;
const int          pick_down = 175;
const int          pick_up   = 150;
void printResult   (HUSKYLENSResult result);
void dir_motor_pwm (unsigned int pwm_left_fw, unsigned int pwm_left_bw, unsigned int pwm_right_fw, unsigned int pwm_right_bw);

boolean motor_forward;
boolean motor_backward;
boolean obj_available;
boolean obj_got;
boolean act_search;
boolean act_stop;
int func_cntr;
int enable_cntr;
int search_cntr;
int preX;
int preY;
int curX;
int curY;
long pre_led_millis;
int  led_state;


//-- pwm to rpm
const int     grid_num = 20;
const int     rpm_interval = 1000; //-- ms
const int     led_interval = 500;  
volatile unsigned int  isr_count_left;
volatile unsigned int  isr_count_right;
unsigned long start_time;
unsigned long stop_time;
boolean       cur_dir_left; 
boolean       cur_dir_right;
unsigned int  cur_pwm_left;
unsigned int  cur_pwm_right;
unsigned int  cur_rpm_left;
unsigned int  cur_rpm_right;
unsigned int  rpm_phase;
boolean   pre_button_status;
int       cur_button_status;
boolean   long_press_acted;
boolean   func_cntr_init;
const int long_press_interval = 1500;
const int reset_interval = 2000;

volatile unsigned long last_switch;
volatile unsigned long pressed_mills;
volatile unsigned long released_mills;
volatile bool button_triggered;

//volatile bool button_acted;
volatile bool long_press;

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
  servo_pick.attach(PIN_SERVO);
  servo_pick.write(pick_up);
  
//-- attach Buzzer and mute it
  pinMode(PIN_BUZZER, OUTPUT);
  noTone(PIN_BUZZER);
  
//-- attach USRR led and turn it off
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

//-- attach USER button
  pinMode(PIN_USER_BTN, INPUT);

//-- Setup the Remote Control's UI canvas
  LRemote.setName("PICKO_v2");
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
  pre_button_status = false;
  cur_button_status = 0;
  func_cntr = 0;
  enable_cntr = 0;
  search_cntr = 0;
  long_press_acted = true;
  func_cntr_init = false;

//-- init variables for ISR
  cur_dir_left  = true; 
  cur_dir_right = true;
  cur_pwm_left  = 0;
  cur_pwm_right = 0;
  cur_rpm_left  = 0;
  cur_rpm_right = 0;
  isr_count_left = 0;
  isr_count_right = 0;
  start_time = 0;
  stop_time = 0;

  attachInterrupt( PIN_USER_BTN,   ISR_button_changed, CHANGE);
  attachInterrupt( PIN_EINT_LEFT,  ISR_counter_left,  FALLING);
  attachInterrupt( PIN_EINT_RIGHT, ISR_counter_right, FALLING);
  rpm_phase = 0;
#ifdef DEMO
  firstSection_a();
#endif 
}

void ISR_button_changed() {
  if ( digitalRead(PIN_USER_BTN) == HIGH ) {
    button_pressed(); 
  } else {
    button_released();
  }
}

void button_pressed() {
  pressed_mills = millis();
}

void button_released() {
  released_mills = millis();
  button_triggered = true;
  if ( (released_mills - pressed_mills) >  long_press_interval ) long_press = true;
  else                                                           long_press = false;
}

void pickup() {
  servo_pick.write(pick_down);
  delay(500);
  servo_pick.write(pick_up);  
}

//-- Buzzer
void buzzer() {  
  tone(PIN_BUZZER, 523, 500);
  delay(100);
  noTone(PIN_BUZZER);
}

void beep(int note, int duration)
{
//-- Play tone on PIN_BUZZER
  tone(PIN_BUZZER, note, duration);
  delay(duration);
//-- Stop tone on PIN_BUZZER
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
void printResult(HUSKYLENSResult result) {
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

void joystick_motor_pwm() {
  if( stickManual.isValueChanged() ) {
    LRemoteDirection d = stickManual.getValue();
    int left_fw  = 100+d.y+d.x;
    int left_bw  = 100-d.y+d.x;
    int right_fw = 100+d.y-d.x;
    int right_bw = 100-d.y-d.x;
    
    if ( d.y > 0 ) {
      dir_motor_pwm( left_fw, 0, right_fw, 0);
    } else if ( d.y < 0 ) {
      dir_motor_pwm( 0, left_bw, 0, right_bw);
    } else {
      dir_motor_pwm( 0, 0, 0, 0);
    }
  }
}

void ISR_counter_left() {
  isr_count_left++;
}

void ISR_counter_right() {
  isr_count_right++;
}

void auto_motor_rpm( unsigned int rpm_left_fw, unsigned int rpm_left_bw, unsigned int rpm_right_fw, unsigned int rpm_right_bw ) {
//-- max pwm is limited under 200
//-- rpm around 130 when pwm 200
  boolean      pre_dir_left;
  boolean      pre_dir_right;
  unsigned int pre_rpm_left;
  unsigned int pre_rpm_right;
  unsigned int pre_pwm_left;
  unsigned int pre_pwm_right;
  int          err_rpm_left;
  int          err_rpm_right;
  int          dif_rpm_left;
  int          dif_rpm_right;
  
  pre_dir_left  = cur_dir_left;
  pre_dir_right = cur_dir_right;
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
  
  err_rpm_left   = (pre_rpm_left  - cur_rpm_left);
  err_rpm_right  = (pre_rpm_right - cur_rpm_right);
  
  if ( (rpm_left_fw == 0) && (rpm_left_bw == 0) ) {            //-- left stop
    cur_dir_left = true;
    dif_rpm_left = 0;
  } else if ( cur_dir_left && (rpm_left_bw == 0) ) { //-- left forwarding
    cur_dir_left = true;
    dif_rpm_left = rpm_left_fw - cur_rpm_left;  
  } else if ( cur_dir_left && (rpm_left_bw > 0) ) {  //-- left forward to backward
    cur_dir_left = false;
    dif_rpm_left = rpm_left_bw;
  } else if ( !cur_dir_left && (rpm_left_fw > 0) ) {   //-- left backward to forward
    cur_dir_left = true;
    dif_rpm_left = rpm_left_fw;
  } else if ( !cur_dir_left && (rpm_left_fw == 0) ) {    //-- left backwarding
    cur_dir_left = false;
    dif_rpm_left = rpm_left_bw - cur_rpm_left;
  }
 
  if ( (rpm_left_fw == 0)  && (rpm_left_bw == 0) ) {        //-- left stop
    cur_pwm_left = 0;
  } else if ( cur_pwm_left == 0 && (rpm_left_fw != 0 || rpm_left_bw != 0) ) { //-- kick off from stop
    cur_pwm_left = 100; //pwm=81@rpm=60
  } else if ( dif_rpm_left > 0 ) { //(err_rpm_left == 0) && 
    if ( cur_pwm_left < 199 ) cur_pwm_left = cur_pwm_left + 1;
    else                      cur_pwm_left = 200;
  } else if ( dif_rpm_left < 0 ) { //(err_rpm_left == 0) && 
    if ( cur_pwm_left > 1 )   cur_pwm_left = cur_pwm_left - 1;
    else                      cur_pwm_left = 0;
  }
  
  if ( (rpm_right_fw == 0) && (rpm_right_bw == 0) ) {          //-- right stop
    cur_dir_right = true;
    dif_rpm_right = 0;
  } else if ( cur_dir_right && rpm_right_bw == 0 ) {   //-- right forwarding
    cur_dir_right = true;
    dif_rpm_right = rpm_right_fw - cur_rpm_right;
  } else if ( cur_dir_right && rpm_right_bw > 0 ) {    //-- right forward to backward
    cur_dir_right = false;
    dif_rpm_right = rpm_right_bw;
  } else if ( !cur_dir_right && rpm_right_fw > 0 ) {   //-- right backward to forward
    cur_dir_right = true;
    dif_rpm_right = rpm_right_fw;
  } else if ( !cur_dir_right && rpm_right_fw == 0 ) {  //-- right backwarding
    cur_dir_right = false;
    dif_rpm_right = rpm_right_bw - cur_rpm_right;
  }
  
  if ( (rpm_right_fw == 0) && (rpm_right_bw == 0) ) {        //-- right stop
    cur_pwm_right = 0;
  } else if ( cur_pwm_right == 0 && (rpm_right_fw != 0 || rpm_right_bw != 0)) { //-- kick off from stop
    cur_pwm_right = 105; //pwm=95@rpm=60
  } else if ( dif_rpm_right > 0 ) { //(err_rpm_right == 0) && 
    if ( cur_pwm_right < 199 ) cur_pwm_right = cur_pwm_right + 1;
    else                       cur_pwm_right = 200;
  } else if ( dif_rpm_right < 0 ) { //(err_rpm_right == 0) && 
    if ( cur_pwm_right > 1 )   cur_pwm_right = cur_pwm_right - 1;
    else                       cur_pwm_right = 0;
  }

  if ( cur_dir_left && cur_dir_right ) {
    dir_motor_pwm( cur_pwm_left, 0, cur_pwm_right, 0);
  } else if ( cur_dir_left && !cur_dir_right ) {
    dir_motor_pwm( cur_pwm_left, 0, 0, cur_pwm_right);
  } else if ( !cur_dir_left && cur_dir_right ) {
    dir_motor_pwm( 0, cur_pwm_left, cur_pwm_right, 0);
  } else if ( !cur_dir_left && !cur_dir_right ) {
    dir_motor_pwm( 0, cur_pwm_left, 0, cur_pwm_right);
  }
  
//-- 輸出至Console
#ifdef PRT_RPM
  Serial.println(String()+F("pre_dir_left  =")+pre_dir_left+F(", pre_rpm_left  =")+pre_rpm_left+F(", pre_pwm_left  =")+pre_pwm_left);
  Serial.println(String()+F("pre_dir_right =")+pre_dir_right+F(", pre_rpm_right =")+pre_rpm_right+F(", pre_pwm_right =")+pre_pwm_right);
  Serial.println(String()+F("cur_dir_left  =")+cur_dir_left+F(", cur_rpm_left  =")+cur_rpm_left+F(", cur_pwm_left  =")+cur_pwm_left);
  Serial.println(String()+F("cur_dir_right =")+cur_dir_right+F(", cur_rpm_right =")+cur_rpm_right+F(", cur_pwm_right =")+cur_pwm_right);
#endif

  //labelLspeed.updateText(String(pwm_left_val)); 
  //labelRspeed.updateText(String(pwm_right_val));
}

void joystick_motor_rpm() {
  if( stickManual.isValueChanged() ) {
    LRemoteDirection d = stickManual.getValue();
    int left_fw  = 100+d.y+d.x;
    int left_bw  = 100-d.y+d.x;
    int right_fw = 100+d.y-d.x;
    int right_bw = 100-d.y-d.x;
    
    if ( d.y > 0 ) {
      auto_motor_rpm( left_fw, 0, right_fw, 0);
    } else if ( d.y < 0 ) {
      auto_motor_rpm( 0, left_bw, 0, right_bw);
    } else {
      auto_motor_rpm( 0, 0, 0, 0);
    }
  }
}

void forward_break() {
  auto_motor_rpm( 80, 0, 80, 0);
  delay(200);
  auto_motor_rpm( 50, 0, 50, 0);
  delay(100);
  auto_motor_rpm( 0, 0, 0, 0);
}

void backward_break() {
  auto_motor_rpm( 0, 80, 0, 80);
  delay(200);
  auto_motor_rpm( 0, 50, 0, 50);
  delay(100);
  auto_motor_rpm( 0, 0, 0, 0);
}

void motor_break() {
  if ( motor_forward ) { 
    forward_break();
  } else if ( motor_backward ) {
    backward_break();
  }
  auto_motor_rpm( 0, 0, 0, 0);
}

//--
void object_searching() {
  auto_motor_rpm( 100, 0, 100, 0);
  delay(500);
  auto_motor_rpm( 100, 0, 50, 0);
  delay(500);
  auto_motor_rpm( 0, 0, 0, 0);
  delay(1000);
}

void object_tracking( int diffXcenter, int diffWidth ) {

    if ( diffXcenter >= 10 ) {                             //-- large turn right
      auto_motor_rpm( 90, 0, 60, 0);
    } else if ( diffXcenter < 10 && diffXcenter >= 0 ) {   //-- small turn right
      auto_motor_rpm( 60+diffXcenter, 0, 60, 0);
    } else if ( diffXcenter < 0 && diffXcenter > -10 ) {   //-- small turn left
      auto_motor_rpm( 60, 0, 60-diffXcenter, 0);
    } else if ( diffXcenter <= -10) {                      //-- large turn left
      auto_motor_rpm( 60, 0, 90, 0);
    } else if ( diffWidth > 50 ) {                         //-- forwarding
      auto_motor_rpm( 60, 0, 60, 0);
    } else {
      auto_motor_rpm( 0, 0, 0, 0);
    }
}

void tracking_message ( int diffWidth, int diffXcenter ) {
/*
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
*/  
}

//-
void loop() {
  HUSKYLENSResult result;
  int diffWidth;
  int diffXcenter;
//-- Searching
  int diffRight;
  int diffUp;
  unsigned long pressed_millsMillis = millis();
  int expect_width   = 180;
  int expect_Xcenter = 160;
  unsigned int pre_func_cntr;

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
#ifdef LREMOTE
    labelMessage.updateText("No Huskeylens!");
#endif    
    obj_available = false;
    preX = 1000;
    preY = 1000;
    curX = 1000;
    curY = 1000;

  } else if( !huskylens.isLearned() ) {
#ifdef PRT_HUSKY    
    Serial.println(F("Object not learned!"));
#endif
#ifdef LREMOTE
    labelMessage.updateText("Not Tracking!");
#endif    
    obj_available = false;
    preX = 1000;
    preY = 1000;
    curX = 1000;
    curY = 1000;
  } else if( huskylens.available() ) {
    result = huskylens.read();
#ifdef PRT_HUSKY
    Serial.println(F("Object Found!"));
    printResult(result);
#endif
//-- tracking
    obj_available = true;
    //Block:xCenter=160,yCenter=200,width=60,height=60,ID=1
    diffXcenter = (result.xCenter - 160);  //> 0 turn right
    diffWidth   = (60 - result.width);     //> 0 forwarding

#ifdef LREMOTE
    labelDoffset.updateText( String(diffXcenter) );
    labelDwidth.updateText( String(diffWidth) );
#endif

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

#ifdef LREMOTE
//labelUp.updateText(String(diffUp));
//labelDright.updateText(String(diffRight));
  labelUp.updateText(String(curX));
  labelDright.updateText(String(curY));
#endif  
 
#ifdef PRT_HUSKY
  Serial.println(String()+F(" ,curX=")+curX
                         +F(" ,curY=")+curY
                         +F(" ,preX=")+preX
                         +F(" ,preY=")+preY
                         +F(" ,diffRight=")+diffRight
                         +F(" ,diffUp=")+diffUp);
#endif
     
//-- searching 
#ifdef LREMOTE
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
#endif

//-- function setting
  if ( button_triggered && (long_press == false) ) {
    beep(c, 500);
    enable_cntr = 0;
    long_press_acted == true;
    if ( func_cntr_init == true ) {
      func_cntr_init = false;
      pre_func_cntr = func_cntr;
      func_cntr = 1;
#ifdef PRT_BTN
      Serial.println(String()+F("init_cntr=")+func_cntr+F("func_cntr_init = ")+func_cntr_init);
#endif
    } else if ( func_cntr > 5 ) {
      func_cntr = 1;
#ifdef PRT_BTN      
      Serial.println(String()+F("reset_cntr=")+func_cntr);
#endif
    } else  {
      func_cntr++;
#ifdef PRT_BTN
      Serial.println(String()+F("func_cntr=")+func_cntr);
#endif
    }
    
    button_triggered = false;
  }
  
  if ( button_triggered && long_press ) {
    beep(a, 500);
    long_press_acted = false;
    func_cntr_init = true;
    if ( enable_cntr > 0 ) { // 0 -> 1 -> 0 -> 1..
      enable_cntr = 0;
    } else {
      enable_cntr++;
    }
    button_triggered = false;
    
#ifdef PRT_BTN
    Serial.println(String()+F("enable_cntr=")+enable_cntr);
#endif
  }

//-- manual pickup test
  if ( ((func_cntr == 1) && (long_press_acted == false)) ||
       (buttonPick.isValueChanged() && (buttonPick.getValue() == 1)) ) {
    pickup();
    long_press_acted = true;
#ifdef PRT_BTN
    Serial.println(String()+F("func_servo func_cntr=")+func_cntr+F("enable_cntr=")+enable_cntr);
#endif
  }

//--
//Block:xCenter=159,yCenter=203,width=66,height=66,ID=1
  if ( (func_cntr == 2) && (enable_cntr == 1) ) {
    if ( obj_available && (curY < 240) && (curY > 50) && (curX > 140) && (curX < 180) ) {
      auto_motor_rpm( 60, 0, 60, 0);
      delay (200);
      pickup();
      beep(a, 200);
//    delay (1000);
//    curX = 1000;
//    curY = 1000;
    }  
    
    if ( long_press_acted == false ) {
      long_press_acted = true;
#ifdef PRT_BTN
      Serial.println(String()+F("func pick on"));
#endif
    }
  } else if ( ( (func_cntr == 2) && (enable_cntr == 0) && (long_press_acted == false) ) ||
              ( (pre_func_cntr == 2) && (enable_cntr == 0) ) ) {
    long_press_acted = true;
    pre_func_cntr = 0;
#ifdef PRT_BTN
    Serial.println(String()+F("func pick off"));
#endif
  }

//-- rpm test
  if ( (func_cntr == 3) && (enable_cntr == 1) ) {
    //-- 每秒(rpm_interval)更新rpm
    if ( millis() - start_time > rpm_interval ) {
      auto_motor_rpm( 60, 0, 60, 0);
    }
    if ( long_press_acted == false ) {
      long_press_acted = true;
#ifdef PRT_BTN
      Serial.println(String()+F("func rpm on"));
#endif
    }
  } else if ( ( (func_cntr == 3) && (enable_cntr == 0) && (long_press_acted == false) ) ||
              ( (pre_func_cntr == 3) && (enable_cntr == 0) ) ) {
    long_press_acted = true;
    pre_func_cntr = 0;
    auto_motor_rpm( 0, 0, 0, 0);
#ifdef PRT_BTN
    Serial.println(String()+F("func rpm off"));
#endif
  }
  
//-- cruising
  if ( (func_cntr == 4) && (enable_cntr == 1) ) {
    auto_motor_rpm( 60, 0, 140, 0);
    if ( long_press_acted == false ) {
      long_press_acted = true;
#ifdef PRT_BTN
      Serial.println(String()+F("func_cruise on"));
#endif
    }
  } else if ( ( (func_cntr == 4) && (enable_cntr == 0) && (long_press_acted == false) ) ||
              ( (pre_func_cntr == 4) && (enable_cntr == 0) ) ) {
    long_press_acted = true;
    pre_func_cntr = 0;
    auto_motor_rpm( 0, 0, 0, 0);
#ifdef PRT_BTN
    Serial.println(String()+F("func_cruise off"));
#endif
  }
  
//-- tracking
  if ( (func_cntr == 5) && (enable_cntr == 1) ) {
#ifdef PRT_TRACK
      Serial.println(String()+F("diffXcenter = ")+diffXcenter+F("diffWidth = ")+diffWidth);
#endif
    if ( obj_available ) {
      object_tracking( diffXcenter, diffWidth );
    } else {
      auto_motor_rpm( 0, 0, 0, 0);
    }
    if ( long_press_acted == false ) {
      long_press_acted = true;
#ifdef PRT_BTN
      Serial.println(String()+F("func_track on"));
#endif
    }
  } else if ( ( (func_cntr == 5) && (enable_cntr == 0) && (long_press_acted == false) ) ||
              ( (pre_func_cntr == 5) && (enable_cntr == 0) ) ) {
    long_press_acted = true;
    pre_func_cntr = 0;
    auto_motor_rpm( 0, 0, 0, 0);
#ifdef PRT_BTN
    Serial.println(String()+F("func_track off"));
#endif
  }
  
//-- searching
  if ( (func_cntr == 6) && (enable_cntr == 1) ) {
    if ( long_press_acted == false ) {
      long_press_acted = true;
#ifdef PRT_BTN
      Serial.println(String()+F("func_search on"));
#endif
    }
  } else if ( ( (func_cntr == 6) && (enable_cntr == 0) && (long_press_acted == false) ) ||
              ( (pre_func_cntr == 6) && (enable_cntr == 0) ) ) {
    long_press_acted = true;
    pre_func_cntr = 0;
    auto_motor_rpm( 0, 0, 0, 0);
#ifdef PRT_BTN
    Serial.println(String()+F("func_search off"));
#endif
  }

//-- function in action
/*
  if ( func_search || switchStart.getValue() ) {
    if ( !switchSearch.getValue() && !switchTrack.getValue() && func_pickup ) {
      joystick_motor_pwm();
      //joystick_motor_rpm();
      
    } else if ( obj_available && (func_search||switchTrack.getValue()) ) {
      object_tracking( diffXcenter, diffWidth );
      search_cntr = 0;
      act_stop = true;
      
    } else if ( (switchSearch.getValue() || func_search) && !obj_available && (search_cntr < 5) && !act_stop ) {
      act_search = true;
      object_searching();
      act_search = false;
      search_cntr++;
      
    } else if ( (switchTrack.getValue() || switchSearch.getValue() || func_search) && !obj_available && act_stop ) {
      auto_motor_rpm( 0, 0, 0, 0);
      delay(1000);
      act_stop = false;
      
    } else {
      //auto_motor_rpm( 0, 0, 0, 0);
    } 
  } else {
    //auto_motor_rpm( 0, 0, 0, 0);
  }
*/

//-- Action LED
  if ( func_cntr == 3 ) {
    digitalWrite( LED_BUILTIN, HIGH );
    led_state = HIGH;
  } else if ( func_cntr == 6 ) {
    if ( (millis() - pre_led_millis) > led_interval ) { 
      pre_led_millis = millis(); 
      if (led_state == LOW) {
        digitalWrite( LED_BUILTIN, HIGH );
        led_state = HIGH;
      } else {
        digitalWrite( LED_BUILTIN, LOW );
        led_state = LOW;
      }
    }
  } else  {
    digitalWrite( LED_BUILTIN, LOW );
    led_state = LOW;
  }
}
