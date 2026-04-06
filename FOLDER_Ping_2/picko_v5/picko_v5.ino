/*
*  HUSKYLENS resolution ratio X320*Y240.
*/
//-- 
#include "Arduino.h"
#include "LTimer.h"
#include <HUSKYLENS.h>
//#include <LRemote.h>
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

//-- define for function
#define FUNC_RPM    5  //1
#define FUNC_CIRCLE 2  //2
#define FUNC_TRACK  3  //3
#define FUNC_PICKUP 4  //4
#define FUNC_SEARCH 1  //5
#define FUNC_SERVO  6  //6

#define LED_ON      1  //other test
#define LED_BLINK   5  //search

//-- define for serial.print
#define PRT_HUSKY
//#define PRT_BTN
#define PRT_RPM
#define PRT_TRACK
//#define DEMO

//--
const int long_press_interval = 1000;
volatile unsigned long pressed_mills;
volatile unsigned long released_mills;
volatile bool button_triggered;
volatile bool long_press;
int      func_cntr;
boolean  func_enable;
boolean  func_acted;
boolean  func_reset;
boolean  func_search_en;

#define HUSKY_OFF     0
#define HUSKY_ON      1
#define HUSKY_CHECK   2
#define HUSKY_NOREQ   3
#define HUSKY_UNKNOWN 4
#define HUSKY_AVAIL   5
#define HUSKY_GONE    6

HUSKYLENS huskylens;
void      printResult( HUSKYLENSResult result );
int       husky_state;
int       pre_husky_state;
const int expect_width   = 80; //max:60
const int expect_Xcenter = 160;
boolean   obj_captured;
int       preX;
int       preY;
int       curX;
int       curY;

//-- servo
Servo     servo_pick;
const int pick_down = 175;
const int pick_up   = 80;

//-- LED
const int led_interval = 500;  
long      pre_led_millis;
int       led_state;

//-- pwm to rpm
const int     grid_num = 20;
const int     rpm_interval = 1000; //-- ms
volatile unsigned int isr_count_left;
volatile unsigned int isr_count_right;
unsigned long rpm_start_time;
unsigned long rpm_stop_time;
unsigned int  rpm_left_fw;
unsigned int  rpm_left_bw; 
unsigned int  rpm_right_fw; 
unsigned int  rpm_right_bw;
boolean       stop_motor;
boolean       sty_rpm_left;
boolean       sty_rpm_right;
boolean       cur_dir_left; 
boolean       cur_dir_right;
unsigned int  cur_pwm_left;
unsigned int  cur_pwm_right;
unsigned int  cur_rpm_left;
unsigned int  cur_rpm_right;
boolean       rpm_ctrl_en;
void dir_motor_pwm( unsigned int pwm_left_fw, unsigned int pwm_left_bw, unsigned int pwm_right_fw, unsigned int pwm_right_bw );

//-- 
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

// callback function for timer0

//-- instantiation
//LTimer timer0(LTIMER_0);
int val0 = 0;
void _callback0(void *usr_data)
{
  val0 = !val0;
  digitalWrite(7, val0);
}

void setup() {
//-- Initialize serial and wait for port to open:
//Serial.begin(9600);
  Serial.begin(115200);
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

//-- I2C to Huskylens
  pre_husky_state = HUSKY_OFF;
  husky_state     = HUSKY_OFF;
  Wire.begin();
  Serial.println("Huskylens started!");
  
  while ( !huskylens.begin(Wire) )  {
    if ( husky_state != HUSKY_CHECK ) {
      Serial.println(F("Huskylens begin failed!"));
      Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>I2C)"));
      Serial.println(F("2.Please recheck the connection."));
    }
    pre_husky_state = HUSKY_CHECK;
    husky_state = HUSKY_CHECK;
    delay(100);
  }
  pre_husky_state = HUSKY_ON;
  husky_state     = HUSKY_ON;
  huskylens.writeAlgorithm(ALGORITHM_OBJECT_TRACKING); //Switch the algorithm to object tracking.
//huskylens.writeAlgorithm(ALGORITHM_COLOR_RECOGNITION);

//-- initialize the variables we're linked to
  obj_captured = false;
  preX = -1;
  preY = -1;
  curX = -1;
  curY = -1;
  func_cntr = 0;
  func_reset = false;
  func_enable = false;
  func_acted = true;
  func_search_en = false;

//-- init variables for ISR
  rpm_ctrl_en  = true;
  rpm_left_fw  = 0;
  rpm_left_bw  = 0; 
  rpm_right_fw = 0; 
  rpm_right_bw = 0;
  stop_motor = false;
  sty_rpm_left = false;
  sty_rpm_right = false;
  cur_dir_left  = true; 
  cur_dir_right = true;
  cur_pwm_left  = 0;
  cur_pwm_right = 0;
  cur_rpm_left  = 0;
  cur_rpm_right = 0;
  isr_count_left = 0;
  isr_count_right = 0;
  rpm_start_time = 0;
  rpm_stop_time = 0;
  led_state = LOW;
  attachInterrupt( PIN_USER_BTN,   ISR_button_changed, CHANGE);
  attachInterrupt( PIN_EINT_LEFT,  ISR_counter_left,  FALLING);
  attachInterrupt( PIN_EINT_RIGHT, ISR_counter_right, FALLING);
//timer0.begin();
//timer0.start(500, LTIMER_REPEAT_MODE, _callback0, NULL);
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

void dir_motor_pwm (unsigned int pwm_left_fw, unsigned int pwm_left_bw, unsigned int pwm_right_fw, unsigned int pwm_right_bw) {
  analogWrite(PIN_LIFT_FW,  pwm_left_fw);
  analogWrite(PIN_LIFT_BW,  pwm_left_bw);
  analogWrite(PIN_RIGHT_FW, pwm_right_fw);
  analogWrite(PIN_RIGHT_BW, pwm_right_bw);
}

void ISR_counter_left() {
  isr_count_left++;
}

void ISR_counter_right() {
  isr_count_right++;
}

//--
void printResult( HUSKYLENSResult result ) {
  if (result.command == COMMAND_RETURN_BLOCK) { 
    if ( (preX != curX) || (preY != curY) ) {
      Serial.println(String()+F("     Block:xCenter = ")+result.xCenter
                             +F("; yCenter = ")+result.yCenter
                             +F("; width = ")+result.width
                             +F("; height = ")+result.height
                             +F("; ID = ")+result.ID);
    }
  } else if (result.command == COMMAND_RETURN_ARROW){
    Serial.println(String()+F("     Arrow:xOrigin=")+result.xOrigin
                           +F("; yOrigin=")+result.yOrigin
                           +F("; xTarget=")+result.xTarget
                           +F("; yTarget=")+result.yTarget
                           +F("; ID=")+result.ID);
  } else{
    Serial.println("Object unknown!");
  }
}

void object_tracking( int curX, int curY, int diffXcenter, int diffWidth ) {
#ifdef PRT_TRACK
   int pre_diffXcenter;
   int pre_diffWidth;

   if ( (pre_diffXcenter != diffXcenter) || (pre_diffWidth != diffWidth) ) {
     Serial.println(String()+F("             object_tracking: diffXcenter = ")+diffXcenter+F("; diffWidth = ")+diffWidth);
   }
   pre_diffXcenter = diffXcenter;
   pre_diffWidth =diffWidth;
#endif
    if ( (curX == -1) && (curY == -1) ) {
      rpm_left_fw  = 0;
      rpm_left_bw  = 0; 
      rpm_right_fw = 0; 
      rpm_right_bw = 0;
    } else if ( (diffWidth > 0) && (diffXcenter >= 10) ) {                        //-- large turn right
      rpm_left_fw  = 60;
      rpm_left_bw  = 0; 
      rpm_right_fw = 30; 
      rpm_right_bw = 0;
    } else if ( (diffWidth > 0) && (diffXcenter >= 0) && (diffXcenter < 20) ) {   //-- small turn right
      rpm_left_fw  = 30 + diffXcenter;
      rpm_left_bw  = 0; 
      rpm_right_fw = 30; 
      rpm_right_bw = 0;
    } else if ( (diffWidth > 0) && (diffXcenter > -20) && (diffXcenter < 0) ) {   //-- small turn left
      rpm_left_fw  = 30;
      rpm_left_bw  = 0; 
      rpm_right_fw = 30 - diffXcenter; 
      rpm_right_bw = 0;
    } else if ( (diffWidth > 0) && (diffXcenter <= -10) ) {                      //-- large turn left
      rpm_left_fw  = 30;
      rpm_left_bw  = 0; 
      rpm_right_fw = 60; 
      rpm_right_bw = 0;
    } else if ( diffWidth > 0 ) {    //-- forwarding
      rpm_left_fw  = 30;
      rpm_left_bw  = 0; 
      rpm_right_fw = 30; 
      rpm_right_bw = 0;
    } else {                         //-- stop
      rpm_left_fw  = 0;
      rpm_left_bw  = 0; 
      rpm_right_fw = 0; 
      rpm_right_bw = 0;
    }
}

//-
void loop() {
  HUSKYLENSResult result; 
  int curWidth;
  int diffWidth;
  int diffXcenter;
  int diffRight;
  int diffUp;
  unsigned int pre_func_cntr;
//-- rpm
  boolean      pre_dir_left;
  boolean      pre_dir_right;
  unsigned int pre_rpm_left;
  unsigned int pre_rpm_right;
  unsigned int pre_pwm_left;
  unsigned int pre_pwm_right;
  int          dif_rpm_left;
  int          dif_rpm_right;
 
  
  if ( !huskylens.request() ) {
#ifdef PRT_HUSKY
    if ( husky_state != HUSKY_NOREQ ) {
      Serial.println(F("Fail to request objects from HUSKYLENS!"));
    }
#endif
    pre_husky_state = husky_state;
    husky_state = HUSKY_NOREQ;
    obj_captured = false;
    preX = -1;
    preY = -1;
    curX = -1;
    curY = -1;
    curWidth = -1;
    diffXcenter = 0;
    diffWidth   = 0;
    diffRight   = 0;
    diffUp      = 0;
    
  } else if( !huskylens.isLearned() ) {
#ifdef PRT_HUSKY
    if ( husky_state != HUSKY_UNKNOWN ) {
      Serial.println(F("Object not learned!"));
    }
#endif
    pre_husky_state = husky_state;
    husky_state = HUSKY_UNKNOWN;
    obj_captured = false;
    preX = -1;
    preY = -1;
    curX = -1;
    curY = -1;
    curWidth = -1;
    diffXcenter = 0;
    diffWidth   = 0;
    diffRight   = 0;
    diffUp      = 0;
    
  } else if( huskylens.available() ) {
    result = huskylens.read();
#ifdef PRT_HUSKY
    if ( husky_state != HUSKY_AVAIL ) { 
      Serial.println(F("Object available!")); 
    }
    printResult(result);
#endif
    pre_husky_state = husky_state;
    husky_state = HUSKY_AVAIL;
    obj_captured = false;
    preX = curX;
    preY = curY;
    curX = result.xCenter;
    curY = result.yCenter;
    curWidth = result.width;
    diffXcenter = (result.xCenter - expect_Xcenter);  //> 0 turn right; expect_Xcenter = 160
    diffWidth   = (expect_width - result.width);      //> 0 forwarding; expect_width = 40
    diffUp      = (preY - curY);
    diffRight   = (curX - preX);
    
  } else { //-- object lost
    if ( husky_state == HUSKY_AVAIL ) { //-- 之前 availavle, 但husky_state尚未更新
      if ( ( curWidth > 80) && (curY < 240) && (curY > 120) && (curX > 120) && (curX < 180) ) {
        obj_captured = true;
#ifdef PRT_HUSKY
        if ( husky_state != HUSKY_GONE ) { 
          Serial.println(F("Object captured!"));
        }
#endif
      } else {
        obj_captured = false;
#ifdef PRT_HUSKY
        if ( husky_state != HUSKY_GONE ) { 
          Serial.println(F("Object lost!"));
        }
#endif
      }
    } else {
      obj_captured = false;
#ifdef PRT_HUSKY
      if ( husky_state != HUSKY_GONE ) { 
        Serial.println(F("Object not available!"));
      }
#endif
    }
    pre_husky_state = husky_state;
    husky_state = HUSKY_GONE;
    preX = curX;
    preY = curY;
    curX = -1;
    curY = -1;
    curWidth = -1;
    diffXcenter = 0;
    diffWidth   = 0;
    diffRight   = 0;
    diffUp      = 0;
  }

#ifdef PRT_HUSKY
  if ( (preX != curX) || (preY != curY) ) {
    Serial.println(String()+F("   preX = ")+preX
                           +F("; preY = ")+preY
                           +F("; diffRight = ")+diffRight
                           +F("; diffUp = ")+diffUp);
                           
    Serial.println(String()+F("   curX = ")+curX
                           +F("; curY = ")+curY
                           +F("; diffXcenter = ")+diffXcenter
                           +F("; diffWidth = ")+diffWidth);
  }                  
#endif

//-- function setting
  if ( button_triggered && !long_press ) {
    beep(c, 500);
    func_enable = false;
    func_acted = true;
    if ( func_reset ) {
      func_reset = false;
      rpm_ctrl_en = true;
      pre_func_cntr = func_cntr;
      func_cntr = 1;
#ifdef PRT_BTN
      Serial.println(String()+F("reset_cntr = ")+func_cntr);
#endif
    } else if ( func_cntr > 5 ) {
      func_cntr = 1;
#ifdef PRT_BTN      
      Serial.println(String()+F("init_cntr = ")+func_cntr);
#endif
    } else  {
      func_cntr++;
#ifdef PRT_BTN
      Serial.println(String()+F("func_cntr = ")+func_cntr);
#endif
    }
    
    button_triggered = false;
  }

//-- function enable
  if ( button_triggered && long_press ) {
    beep(a, 500);
    func_acted = false;
    func_reset = true;
    func_enable = !func_enable;
    button_triggered = false;
#ifdef PRT_BTN
    Serial.println(String()+F("func_enable = ")+func_enable);
#endif
  }

//-- manual servo test
  if ( !func_acted && (func_cntr == FUNC_SERVO) ) {
    pickup();
    func_acted = true;
#ifdef PRT_BTN
    Serial.println(String()+F("func_servo = ")+func_cntr+F("func_enable = ")+func_enable);
#endif
  }
  
//-- rpm test
  if ( func_enable && (func_cntr == FUNC_RPM) ) {
    rpm_ctrl_en  = true;
    rpm_left_fw  = 40;
    rpm_left_bw  = 0; 
    rpm_right_fw = 40;
    rpm_right_bw = 0;
    
    if ( !func_acted ) {
      func_acted = true;
#ifdef PRT_BTN
      Serial.println(String()+F("func rpm on"));
#endif
    }
  } else if ( !func_enable &&
              ( ( !func_acted && (func_cntr == FUNC_RPM) ) || (pre_func_cntr == FUNC_RPM) ) ) {
    func_acted = true;
    pre_func_cntr = 0;
    stop_motor = true;
#ifdef PRT_BTN
    Serial.println(String()+F("func rpm off"));
#endif
  }
  
//-- circling
//  if ( func_enable && ( (func_cntr == FUNC_CIRCLE) || func_search_en ) ) {
  if ( func_enable && (func_cntr == FUNC_CIRCLE) ) {
    if ( (husky_state != HUSKY_AVAIL) && !obj_captured ) {
      rpm_left_fw  = 90;
      rpm_left_bw  = 0; 
      rpm_right_fw = 30; 
      rpm_right_bw = 0;
    }
    if ( !func_acted ) {
      func_acted = true;
#ifdef PRT_BTN
      Serial.println(String()+F("func_circle on"));
#endif
    }
  } else if ( !func_enable &&
              ( ( !func_acted && (func_cntr == FUNC_CIRCLE) ) || (pre_func_cntr == FUNC_CIRCLE) ) ) {
//            ( ( !func_acted && (func_cntr == FUNC_CIRCLE) ) || (pre_func_cntr == FUNC_CIRCLE) || func_search_en ) ) {
    func_acted = true;
    pre_func_cntr = 0;
    stop_motor = true;
#ifdef PRT_BTN
    Serial.println(String()+F("func_circle off"));
#endif
  }
  
//-- tracking
  if ( func_enable && ( (func_cntr == FUNC_TRACK) || func_search_en ) ) {
    object_tracking( curX, curY, diffXcenter, diffWidth );
    if ( !func_acted ) {
      func_acted = true;
#ifdef PRT_BTN
      Serial.println(String()+F("func_track on"));
#endif
    }
  } else if ( !func_enable &&
              ( ( !func_acted && (func_cntr == FUNC_TRACK) ) || (pre_func_cntr == FUNC_TRACK) || func_search_en ) ) {
    func_acted = true;
    pre_func_cntr = 0;    
    stop_motor = true;
#ifdef PRT_BTN
    Serial.println(String()+F("func_track off"));
#endif
  }
  
//-- Block:xCenter=159,yCenter=203,width=66,height=66,ID=1
  if ( func_enable && ( (func_cntr == FUNC_PICKUP) || func_search_en ) ) {
    if ( obj_captured ) {
      pickup();
      beep(a, 200);
    }  
    
    if ( !func_acted ) {
      func_acted = true;
#ifdef PRT_BTN
      Serial.println(String()+F("func pick on"));
#endif
    }
  } else if ( !func_enable &&
              ( ( !func_acted && (func_cntr == FUNC_PICKUP) ) || (pre_func_cntr == FUNC_PICKUP) || func_search_en ) ) {
    func_acted = true;
    pre_func_cntr = 0;
    stop_motor = true;
#ifdef PRT_BTN
    Serial.println(String()+F("func pick off"));
#endif
  }
  
//-- searching
  if ( func_enable && (func_cntr == FUNC_SEARCH) ) {
    func_search_en = true;
    if ( !func_acted ) {
      func_acted = true;
#ifdef PRT_BTN
      Serial.println(String()+F("func_search on"));
#endif
    }
  } else if ( !func_enable &&
              ( ( !func_acted && (func_cntr == FUNC_SEARCH) ) || (pre_func_cntr == FUNC_SEARCH) ) ) {
    func_acted = true;
    pre_func_cntr = 0;
    func_search_en = false;
#ifdef PRT_BTN
    Serial.println(String()+F("func_search off"));
#endif
  }


if ( stop_motor ) {
  rpm_left_fw  = 0;
  rpm_left_bw  = 0; 
  rpm_right_fw = 0; 
  rpm_right_bw = 0;
}
      
//-- main program for rpm control
//-- 每秒(rpm_interval = 1000ms)更新rpm
//-- max pwm is limited under 200
//-- rpm around 130 when pwm 200
  if ( (millis() - rpm_start_time) > rpm_interval ) {
     pre_dir_left  = cur_dir_left;
     pre_dir_right = cur_dir_right;
     pre_rpm_left  = cur_rpm_left;
     pre_rpm_right = cur_rpm_right;
     pre_pwm_left  = cur_pwm_left;
     pre_pwm_right = cur_pwm_right;

//-- 計算 rpm 時，停止計數
     detachInterrupt( PIN_EINT_LEFT );
     detachInterrupt( PIN_EINT_RIGHT );
     rpm_stop_time = millis();

//-- (偵測的格數count / 一圈網格數20) / (時間差 / 1000)  
     cur_rpm_left  = isr_count_left  * (60 * 1000 / grid_num ) / (rpm_stop_time - rpm_start_time);
     cur_rpm_right = isr_count_right * (60 * 1000 / grid_num ) / (rpm_stop_time - rpm_start_time);   
  
//-- Restart the interrupt processing
     isr_count_left = 0;
     isr_count_right = 0;
     attachInterrupt( PIN_EINT_LEFT,  ISR_counter_left,  FALLING);
     attachInterrupt( PIN_EINT_RIGHT, ISR_counter_right, FALLING);
     rpm_start_time = millis();
  
     if ( (rpm_left_fw == 0) && (rpm_left_bw == 0) ) {   //-- left stop
       cur_dir_left = true;
       dif_rpm_left = 0;
     } else if ( cur_dir_left && (rpm_left_bw == 0) ) {  //-- left forwarding
       cur_dir_left = true;
       dif_rpm_left = rpm_left_fw - cur_rpm_left;  
     } else if ( cur_dir_left && (rpm_left_bw > 0) ) {   //-- left forward to backward
       cur_dir_left = false;
       dif_rpm_left = rpm_left_bw;
     } else if ( !cur_dir_left && (rpm_left_fw > 0) ) {  //-- left backward to forward
       cur_dir_left = true;
       dif_rpm_left = rpm_left_fw;
     } else if ( !cur_dir_left && (rpm_left_fw == 0) ) { //-- left backwarding
       cur_dir_left = false;
       dif_rpm_left = rpm_left_bw - cur_rpm_left;
     }

     if ( (rpm_right_fw == 0) && (rpm_right_bw == 0) ) { //-- right stop
       cur_dir_right = true;
       dif_rpm_right = 0;
     } else if ( cur_dir_right && rpm_right_bw == 0 ) {  //-- right forwarding
       cur_dir_right = true;
       dif_rpm_right = rpm_right_fw - cur_rpm_right;
     } else if ( cur_dir_right && rpm_right_bw > 0 ) {   //-- right forward to backward
       cur_dir_right = false;
       dif_rpm_right = rpm_right_bw;
     } else if ( !cur_dir_right && rpm_right_fw > 0 ) {  //-- right backward to forward
       cur_dir_right = true;
       dif_rpm_right = rpm_right_fw;
     } else if ( !cur_dir_right && rpm_right_fw == 0 ) { //-- right backwarding
       cur_dir_right = false;
       dif_rpm_right = rpm_right_bw - cur_rpm_right;
     }

     if ( rpm_ctrl_en ) {
//-- pwm = 100 left kick off  
//-- pwm = 95  steady rpm=60
       if ( (rpm_left_fw == 0)  && (rpm_left_bw == 0) ) {        //-- left stop
         sty_rpm_left = false;
         cur_pwm_left = 0;
       } else if ( cur_pwm_left == 0 && (rpm_left_fw != 0 || rpm_left_bw != 0) ) { //-- kick off from stop
         sty_rpm_left = false;
         cur_pwm_left = 90;
       } else if ( dif_rpm_left >= 10 ) {
         sty_rpm_left = false;
         if ( (cur_pwm_left + dif_rpm_left) < 200 ) cur_pwm_left = cur_pwm_left + dif_rpm_left;
         else                                       cur_pwm_left = 200;
       } else if ( dif_rpm_left > 2 && dif_rpm_left < 10 ) {
         sty_rpm_left = false;
         if ( cur_pwm_left < 199 ) cur_pwm_left = cur_pwm_left + 1;
         else                      cur_pwm_left = 200;
       } else if ( dif_rpm_left > -10 && dif_rpm_left < -2 ) {
         sty_rpm_left = false;
         if ( cur_pwm_left > 1 )   cur_pwm_left = cur_pwm_left - 1;
         else                      cur_pwm_left = 0;
       } else if ( dif_rpm_left <= -10 ) {
         sty_rpm_left = false;
         if ( (cur_pwm_left + dif_rpm_left) > 0 )  cur_pwm_left = cur_pwm_left + dif_rpm_left;
         else                                      cur_pwm_left = 0;
       } else {
         sty_rpm_left = true;
       }

//-- pwm = 110 right kick off  
//-- pwm = 95  steady rpm=60
      if ( (rpm_right_fw == 0) && (rpm_right_bw == 0) ) {        //-- right stop
        sty_rpm_right = false;
        cur_pwm_right = 0;
      } else if ( cur_pwm_right == 0 && (rpm_right_fw != 0 || rpm_right_bw != 0)) { //-- kick off from stop
        sty_rpm_right = false;
        cur_pwm_right = 100;
      } else if ( dif_rpm_right >= 10 ) {
        sty_rpm_right = false;
        if ( (cur_pwm_right + dif_rpm_right) < 200 ) cur_pwm_right = cur_pwm_right + dif_rpm_right;
        else                                         cur_pwm_right = 200;
      } else if ( dif_rpm_right > 2 && dif_rpm_right < 10 ) {
        sty_rpm_right = false;
        if ( cur_pwm_right < 199 ) cur_pwm_right = cur_pwm_right + 1;
        else                       cur_pwm_right = 200;
      } else if ( dif_rpm_right > -10 && dif_rpm_right < -2 ) {
        sty_rpm_right = false;
        if ( cur_pwm_right > 1 )   cur_pwm_right = cur_pwm_right - 1;
        else                       cur_pwm_right = 0;
      } else if ( dif_rpm_right <= -10 ) {
        sty_rpm_right = false;
        if ( (cur_pwm_right + dif_rpm_right) > 0 )  cur_pwm_right = cur_pwm_right + dif_rpm_right;
        else                                        cur_pwm_right = 0;
      } else {
        sty_rpm_right = true;
      }
    } //-- rpm_ctrl_en
  
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
    if ( (pre_pwm_left != cur_pwm_left) || (pre_pwm_right != cur_pwm_right) ) {
      Serial.println(String()+F("sty_rpm_left  = ")+sty_rpm_left+F("; pre_rpm_left  = ")+pre_rpm_left+F("; pre_pwm_left  = ")+pre_pwm_left);
      Serial.println(String()+F("sty_rpm_right = ")+sty_rpm_right+F("; pre_rpm_right = ")+pre_rpm_right+F("; pre_pwm_right = ")+pre_pwm_right);
      Serial.println(String()+F("cur_dir_left  = ")+cur_dir_left+F("; cur_rpm_left  = ")+cur_rpm_left+F("; cur_pwm_left  = ")+cur_pwm_left);
      Serial.println(String()+F("cur_dir_right = ")+cur_dir_right+F("; cur_rpm_right = ")+cur_rpm_right+F("; cur_pwm_right = ")+cur_pwm_right);
    }
#endif
  }

//-- LED status
  if ( func_cntr == LED_ON ) {
    digitalWrite( LED_BUILTIN, HIGH );
    led_state = HIGH;
  } else if ( func_cntr == LED_BLINK ) {
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
