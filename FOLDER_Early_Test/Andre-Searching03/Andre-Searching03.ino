/*

*/
#include <HUSKYLENS.h>
#include <SoftwareSerial.h>
#include <LRemote.h>
#include <Servo.h>

#define Right_Wheel_A   12
#define Right_Wheel_B   10
#define Left_Wheel_A    17
#define Left_Wheel_B    13
#define BUZZER_PIN      14
#define BUTTON_PIN      6
//#define LED_BUILTIN   7
#define SERVO_PIN       9
#define S_UART_RX       2
#define S_UART_TX       3

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

HUSKYLENS      huskylens;

SoftwareSerial mySerial(2, 3); // RX, TX of linkit 7697
Servo          myservo;  
void printResult (HUSKYLENSResult result);
void direct_motor (int right_F, int right_B , int left_F, int left_B);

boolean motor_forward;
boolean motor_backward;
boolean obj_available;
boolean obj_found;
boolean obj_got;
boolean obj_lost;
boolean obj_search;
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

void setup() {
//-- Initialize serial and wait for port to open:
  Serial.begin(9600);
  Serial.println("Start configuring remote");

//-- Setup the Remote Control's UI canvas
  LRemote.setName("Andre-Searching02");
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
  Serial.println("begin() returned");
  
  pinMode(10, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(17, OUTPUT);
  direct_motor( 0, 0, 0, 0);

//-- Initialize GPIO
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object

//-- Buzzer PIN Set
  pinMode(BUZZER_PIN, OUTPUT);
//-- Button Read
  pinMode(BUTTON_PIN, INPUT);
//pinMode(LED_BUILTIN, OUTPUT);
//digitalWrite(LED_BUILTIN, LOW);

  mySerial.begin(9600);  
  while ( !huskylens.begin(mySerial) ) {
    Serial.println(F("Begin failed!"));
    Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>Serial 9600)"));
    Serial.println(F("2.Please recheck the connection."));
    delay(100);
  }
  
huskylens.writeAlgorithm(ALGORITHM_OBJECT_TRACKING); //Switch the algorithm to object tracking.
//huskylens.writeAlgorithm(ALGORITHM_COLOR_RECOGNITION);

//-- initialize the variables we're linked to
    obj_lost = false;
    obj_available = false;
    obj_got = false;
    obj_found = false;    
    obj_search = false;
    preX = 1000;
    preY = 1000;
    curX = 1000;
    curY = 1000;
    button_status = false;
    button_cntr = 0;
    search_cntr = 0;
    firstSection_a();
}

void direct_motor (int right_F, int right_B , int left_F, int left_B ) {
  int rightSpeed;
  int leftSpeed;
  if ( obj_search && obj_available ) {
    analogWrite(10, 0 );  //right INA +
    analogWrite(12, 0);  //right INB -
    analogWrite(13, 0);   //left  INB -
    analogWrite(17, 0);   //left  INA +
  } else {
    analogWrite(10, right_F);  //right INA +
    analogWrite(12, right_B);  //right INB -
    analogWrite(13, left_F);   //left  INB -
    analogWrite(17, left_B);   //left  INA +
  }
  
  if ( right_F == 0 && left_F == 0 ) motor_forward = false;
  else                               motor_forward = true; 

  if ( right_B == 0 && left_B == 0 ) motor_backward = false;
  else                               motor_backward = true; 

  if ( right_F == 0 ) rightSpeed = right_B;
  else                rightSpeed = right_F;

  if ( left_F == 0 )  leftSpeed = left_B;
  else                leftSpeed = left_F;
  
  labelRspeed.updateText(String(rightSpeed));
  labelLspeed.updateText(String(leftSpeed));
}

void forward_break() {
  direct_motor( 80, 0, 80, 0);
  delay(200);
  direct_motor( 50, 0, 50, 0);
  delay(100);
  direct_motor( 0, 0, 0, 0);
}

void backward_break() {
  direct_motor( 0, 80, 0, 80);
  delay(200);
  direct_motor( 0, 50, 0, 50);
  delay(100);
  direct_motor( 0, 0, 0, 0);
}

void motor_break () {
  if ( motor_forward ) { 
    forward_break();
  } else if ( motor_backward ) {
    backward_break();
  }
  direct_motor( 0, 0, 0, 0);
}

void checkAndUpdateMotor() {
  if( stickManual.isValueChanged() ) {
    LRemoteDirection d = stickManual.getValue();
    int f_right = 100+d.y-d.x;
    int f_left  = 100+d.y+d.x;
    int b_right = 100-d.y+d.x;
    int b_left  = 100-d.y-d.x;
    
    if ( d.y > 0 ) {
      direct_motor( f_right, 0, f_left, 0);
    } else if ( d.y < 0 ) {
      direct_motor( 0, b_right, 0, b_left);
    } else {
      direct_motor( 0, 0, 0, 0);
    }
  }
}

//-- object searching
void object_searching () {
  direct_motor( 130, 0, 200, 0);
  delay (100);
  direct_motor( 0, 0, 130, 0);
  delay (300);
  direct_motor( 0, 0, 0, 0);
  delay (1000);
}

#define MAX_WIDTH  20
#define MIN_WIDTH  3
#define MAX_OFFSET 15
#define MIN_OFFSET 3

#define MKICK      115

void object_tracking ( int diffWidth, int diffXcenter ) {

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
    direct_motor( 0, MKICK+rKick+rSlow, 0, MKICK+20+20); //145, 155
  
  } else if ( (diffWidth > MAX_WIDTH*(-1)) && (diffWidth <= MIN_WIDTH*(-1)) ) {
    if ( motor_forward ) forward_break();
    direct_motor( 0, MKICK+rKick, 0, MKICK+20); //125, 135 

  } else if ( (diffWidth > MIN_WIDTH*(-1)) && (diffWidth < MIN_WIDTH) ) {
    motor_break();

  } else if ( diffWidth >=  MIN_WIDTH ) {
    if ( diffXcenter >= MAX_OFFSET ) {
      direct_motor( MKICK+rKick, 0, MKICK+20+20+20, 0); //125, 175

    } else if ( (diffXcenter < MAX_OFFSET) && (diffXcenter >= MIN_OFFSET) ) {
      direct_motor( MKICK+rKick, 0, MKICK+20+20, 0); //125, 155

    } else if ( (diffXcenter <= MIN_OFFSET*(-1)) && (diffXcenter > MAX_OFFSET*(-1)) ) {
      direct_motor( MKICK+rKick+rSlow, 0, MKICK+20, 0); //145, 135
      
    } else if ( diffXcenter <= MAX_OFFSET*(-1)) {
      direct_motor( MKICK+rKick+rSlow+rFast, 0, MKICK+20, 0); //165, 135
      
    } else {
      direct_motor( MKICK+rKick, 0, MKICK+20, 0); //125, 135
    
    }
  } else {
    motor_break();
    //delay(500);
    
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

// Buzzer
void buzzer () {  
  tone(BUZZER_PIN, 523, 500);
  delay(100);
  noTone(BUZZER_PIN);
}

void beep (int note, int duration)
{
  //Play tone on BUZZER_PIN
  tone(BUZZER_PIN, note, duration);
  delay(duration);
  //Stop tone on BUZZER_PIN
  noTone(BUZZER_PIN);
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

void secondSection()
{
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
boolean btn_start  = (button_cntr > 0);
boolean btn_search = (button_cntr == 1);
boolean btn_track  = (button_cntr == 2);

  if ( !button_status && digitalRead(BUTTON_PIN) == HIGH )  {
    beep(a, 500);
    button_status = true;
    if ( button_cntr > 1 ) {
      button_cntr = 0;
    } else {
      button_cntr++;
    }
  } else if ( button_status && digitalRead(BUTTON_PIN) == HIGH ) {
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
  
  if ( btn_nouse ) {
    expect_width   = sliderWidth.getValue();
    expect_Xcenter = sliderXoffset.getValue();
  } else {
    expect_width   = 150;
    expect_Xcenter = 160;
  }
//-- check if connected by BLE central devices,
//-- e.g. an mobile app
  if( !LRemote.connected() ) {
    Serial.println("waiting for connection");
    delay(1000);
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
//  Serial.println(F("Fail to request objects from HUSKYLENS!"));
    labelMessage.updateText("No Huskeylens!");
    obj_available = false;
    preX = 1000;
    preY = 1000;
    curX = 1000;
    curY = 1000;

  } else if( !huskylens.isLearned() ) {
//  Serial.println(F("Object not learned!"));
    labelMessage.updateText("Not Tracking!");
    obj_available = false;
    preX = 1000;
    preY = 1000;
    curX = 1000;
    curY = 1000;
  } else if( huskylens.available() ) {
//  Serial.println(F("Object Found!"));
    result = huskylens.read();
    //printResult(result);
    obj_available = true;

//-- tracking
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
    obj_lost = false;
  } else {
    diffRight = 0;
    diffUp  = 0;
    obj_lost = true;
  }

//labelUp.updateText(String(diffUp));
//labelDright.updateText(String(diffRight));
  labelUp.updateText(String(curX));
  labelDright.updateText(String(curY));
 
  if ( !obj_got && !obj_available && obj_found && (diffUp < 0) && (curY > 150) && (curX > 50) && (curX < 270) ) {
    obj_got = true;
    beep(a, 500);    
    beep(c, 500);
  } else {
    obj_got = false; 
  }

  if ( obj_available ) {
    obj_found = true;
  } else if ( obj_got || obj_lost ) {
    obj_found = false;
  }

  if ( obj_available ) { //&& obj_found )
    search_cntr = 0;
  } else if ( (switchSearch.getValue() || btn_start) && !obj_available && (search_cntr < 5) ) { //&& !obj_found 
    search_cntr++;
  }

  if ( (switchSearch.getValue() || btn_start) && search_cntr > 0 ) {
    obj_search = true;
  } else {
    obj_search = false;
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
/*  
  Serial.println(String()+F(" ,curX=")+curX
                         +F(" ,curY=")+curY
                         +F(" ,preX=")+preX
                         +F(" ,preY=")+preY
                         +F(" ,diffRight=")+diffRight
                         +F(" ,diffUp=")+diffUp);
*/

//-- Action
  if ( switchStart.getValue() || btn_start ) {
    if ( !switchSearch.getValue() && !switchTrack.getValue() && btn_nouse ) {
      checkAndUpdateMotor();
    } else if ( ((switchSearch.getValue() && switchTrack.getValue()) || btn_search) && (!obj_available && obj_found) ) { //-- retry
//    object_tracking( diffUp*10, diffRight*10 );
      motor_break();
      delay(1000);
    } else if ( (switchSearch.getValue() || btn_search) && !obj_available && !obj_found  && (search_cntr < 6) ) {
      object_searching();
    } else if ( (switchTrack.getValue() || btn_start) && obj_available && obj_found ) {
      object_tracking( diffWidth, diffXcenter );
    } else {
      motor_break();
    } 
  } else {
    motor_break();
  } //-- end of }
}
