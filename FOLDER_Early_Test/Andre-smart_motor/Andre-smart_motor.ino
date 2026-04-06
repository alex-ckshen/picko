/*

*/
#include <HUSKYLENS.h>
#include <SoftwareSerial.h>
#include <LRemote.h>
#include <Servo.h>

LRemoteSlider   LSlowSlider;
LRemoteSlider   LFastSlider;
LRemoteSlider   RSlowSlider;
LRemoteSlider   RFastSlider;

LRemoteLabel    labelMessage;
LRemoteSlider   offsetSlider;
LRemoteSlider   widthSlider;

LRemoteButton   buttonPick;
LRemoteLabel    labelDoffset;
LRemoteLabel    labelDwidth;

LRemoteSwitch   switchStart;
LRemoteSwitch   switchTrack;
LRemoteSwitch   switchSearch;

LRemoteLabel    labelRspeed;
LRemoteLabel    labelLspeed;
LRemoteJoyStick stickManual;

LRemoteSlider   LKickSlider;
LRemoteSlider   RKickSlider;

Servo          myservo;  
HUSKYLENS      huskylens;
SoftwareSerial mySerial(2, 3); // RX, TX of linkit 7697
void printResult (HUSKYLENSResult result);
void direct_motor (int right_F, int right_B , int left_F, int left_B);
//void object_tracking();

boolean is_forwarding;
boolean is_backwarding;

boolean was_available;
boolean is_available;
int preXY_counter;
int search_counter;
int preX;
int preY;
int diffWidth;
int diffOffset;
int diffRight;
int diffDown;
int pre_right_F;
int pre_right_B;
int pre_left_F;
int pre_left_B;

void setup() {
//-- Initialize serial and wait for port to open:
  Serial.begin(9600);
  Serial.println("Start configuring remote");

//-- Setup the Remote Control's UI canvas
  LRemote.setName("Andre-smart_motor");
  LRemote.setOrientation(RC_LANDSCAPE);
  LRemote.setGrid(6, 4);
//
  LSlowSlider.setPos(0, 0);
  LSlowSlider.setSize(1, 1);
  LSlowSlider.setText("LS");
  LSlowSlider.setValueRange(0, 50, 0);
  LSlowSlider.setColor(RC_BLUE);
  LRemote.addControl(LSlowSlider);

  LFastSlider.setPos(0, 1);
  LFastSlider.setSize(1, 1);
  LFastSlider.setText("LF");
  LFastSlider.setValueRange(0, 100, 0);
  LFastSlider.setColor(RC_BLUE);
  LRemote.addControl(LFastSlider);

  RSlowSlider.setPos(0, 2);
  RSlowSlider.setSize(1, 1);
  RSlowSlider.setText("RS");
  RSlowSlider.setValueRange(0, 50, 0);
  RSlowSlider.setColor(RC_BLUE);
  LRemote.addControl(RSlowSlider);

  RFastSlider.setPos(0, 3);
  RFastSlider.setSize(1, 1);
  RFastSlider.setText("RF");
  RFastSlider.setValueRange(0, 100, 0);
  RFastSlider.setColor(RC_BLUE);
  LRemote.addControl(RFastSlider);
  
  labelMessage.setPos(1, 0);
  labelMessage.setSize(2, 1);
  labelMessage.setColor(RC_BLUE);
  labelMessage.setText("Press to Start");
  LRemote.addControl(labelMessage);
  
  offsetSlider.setPos(1, 1);
  offsetSlider.setSize(2, 1);
  offsetSlider.setText("Target X");
  offsetSlider.setValueRange(140, 180, 140);
  offsetSlider.setColor(RC_BLUE);
  LRemote.addControl(offsetSlider);

  widthSlider.setPos(1, 2);
  widthSlider.setSize(2, 1);
  widthSlider.setText("Target width");
  widthSlider.setValueRange(0, 320, 0);
  widthSlider.setColor(RC_BLUE);
  LRemote.addControl(widthSlider);

  buttonPick.setPos(3, 0);
  buttonPick.setSize(1, 1);
  buttonPick.setColor(RC_ORANGE);
  buttonPick.setText("Pick");
  LRemote.addControl(buttonPick);

  labelDoffset.setPos(3, 1);
  labelDoffset.setSize(1, 1);
  labelDoffset.setColor(RC_BLUE);
  labelDoffset.setText("0");
  LRemote.addControl(labelDoffset);
  
  labelDwidth.setPos(3, 2);
  labelDwidth.setSize(1, 1);
  labelDwidth.setColor(RC_BLUE);
  labelDwidth.setText("0");
  LRemote.addControl(labelDwidth);
    
  switchStart.setPos(1, 3);
  switchStart.setSize(1, 1);
  switchStart.setColor(RC_ORANGE);
  switchStart.setText("Start");
  LRemote.addControl(switchStart);
  
  switchTrack.setPos(2, 3);
  switchTrack.setSize(1, 1);
  switchTrack.setColor(RC_ORANGE);
  switchTrack.setText("Track");
  LRemote.addControl(switchTrack);

  switchSearch.setPos(3, 3);
  switchSearch.setSize(1, 1);
  switchSearch.setColor(RC_ORANGE);
  switchSearch.setText("Search");
  LRemote.addControl(switchSearch);

  labelLspeed.setPos(4, 0);
  labelLspeed.setSize(1, 1);
  labelLspeed.setColor(RC_GREY);
  labelLspeed.setText("0");
  LRemote.addControl(labelLspeed);
 
  labelRspeed.setPos(5, 0);
  labelRspeed.setSize(1, 1);
  labelRspeed.setColor(RC_GREY);
  labelRspeed.setText("0");
  LRemote.addControl(labelRspeed);
  
  stickManual.setPos(4, 1);
  stickManual.setSize(2, 2);
  stickManual.setColor(RC_BLUE);
  LRemote.addControl(stickManual);

  LKickSlider.setPos(0, 0);
  LKickSlider.setSize(1, 1);
  LKickSlider.setText("LK");
  LKickSlider.setValueRange(100, 150, 100);
  LKickSlider.setColor(RC_BLUE);
  LRemote.addControl(LKickSlider);

  RKickSlider.setPos(0, 1);
  RKickSlider.setSize(1, 1);
  RKickSlider.setText("RK");
  RKickSlider.setValueRange(100, 150, 100);
  RKickSlider.setColor(RC_BLUE);
  LRemote.addControl(RKickSlider);
  
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
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object

  mySerial.begin(9600);  
  while ( !huskylens.begin(mySerial) ) {
        Serial.println(F("Begin failed!"));
        Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>Serial 9600)"));
        Serial.println(F("2.Please recheck the connection."));
        delay(100);
  }
  
  huskylens.writeAlgorithm(ALGORITHM_OBJECT_TRACKING); //Switch the algorithm to object tracking.
//-- initialize the variables we're linked to
  was_available = false;
  is_available = false;
  diffWidth = 0;
  diffOffset = 0;
  diffRight = 0;
  diffDown = 0;
  
  pre_right_F = 0;
  pre_right_B = 0;
  pre_left_F  = 0;
  pre_left_B  = 0;
}

void direct_motor (int right_F, int right_B , int left_F, int left_B ) {
  int rightSpeed;
  int leftSpeed;
  
  analogWrite(10, right_F);  //right INA +
  analogWrite(12, right_B);  //right INB -
  analogWrite(13, left_F);   //left  INB -
  analogWrite(17, left_B);   //left  INA +

  if ( right_F == 0 && left_F == 0 ) is_forwarding = false;
  else                               is_forwarding = true; 

  if ( right_B == 0 && left_B == 0 ) is_backwarding = false;
  else                               is_backwarding = true; 

  if ( right_F == 0 ) rightSpeed = right_B;
  else                rightSpeed = right_F;

  if ( left_F == 0 )  leftSpeed = left_B;
  else                leftSpeed = left_F;
  
  labelRspeed.updateText(String(rightSpeed));
  labelLspeed.updateText(String(leftSpeed));
}

void forward_break () {
  direct_motor( 80, 0, 80, 0);
  delay(200);
  direct_motor( 50, 0, 50, 0);
  delay(100);
  direct_motor( 0, 0, 0, 0);
}

void backward_break () {
  direct_motor( 0, 80, 0, 80);
  delay(200);
  direct_motor( 0, 50, 0, 50);
  delay(100);
  direct_motor( 0, 0, 0, 0);
}

void motor_break () {
  if ( is_forwarding ) { 
    forward_break();
  } else if ( is_backwarding ) {
    backward_break();
  } else {
    direct_motor( 0, 0, 0, 0);
  }
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

#define MAX_WIDTH  30
#define MIN_WIDTH  5
#define MAX_OFFSET 15
#define MIN_OFFSET 3

void object_message ( int diffWidth, int diffOffset ) {
    
  if ( diffWidth <= MAX_WIDTH*(-1) ) {
    labelMessage.updateText("Fast Backward!");
  
  } else if ( (diffWidth > MAX_WIDTH*(-1)) && (diffWidth <= MIN_WIDTH*(-1)) ) {
    labelMessage.updateText("Slow Backward!");
 
  } else if ( (diffWidth > MIN_WIDTH*(-1)) && (diffWidth < MIN_WIDTH) ) {
    labelMessage.updateText("Motor Break!");

  } else if ( diffWidth >= MIN_WIDTH ) {
    if ( diffOffset >= MAX_OFFSET ) {
      labelMessage.updateText("Fast Right!");
    } else if ( (diffOffset < MAX_OFFSET) && (diffOffset >= MIN_OFFSET) ) {
      labelMessage.updateText("Slow Right!");
    } else if ( (diffOffset <= MIN_OFFSET*(-1)) && (diffOffset > MAX_OFFSET*(-1)) ) {
      labelMessage.updateText("Slow Left!");
    } else if ( diffOffset <= MAX_OFFSET*(-1) ) {
      labelMessage.updateText("Fast Left!");
    } else {
      labelMessage.updateText("Slow Forward!");
    }
    
  } else {
    labelMessage.updateText("Stop!");
    
  }
}

void object_tracking( int diffWidth, int diffOffset ) {

  if ( diffWidth <= MAX_WIDTH*(-1) ) {
    if ( is_forwarding ) forward_break();
    direct_motor( 0, 30+RKickSlider.getValue(), 0, 30+LKickSlider.getValue());
  
  } else if ( (diffWidth > MAX_WIDTH*(-1)) && (diffWidth <= MIN_WIDTH*(-1)) ) {
    if ( is_forwarding ) forward_break();
    direct_motor( 0, RKickSlider.getValue(), 0, LKickSlider.getValue());

  } else if ( (diffWidth > MIN_WIDTH*(-1)) && (diffWidth < MIN_WIDTH) ) {
    motor_break();

  } else if ( diffWidth >=  MIN_WIDTH ) {
    if ( diffOffset >= MAX_OFFSET ) {
      direct_motor( RKickSlider.getValue(), 0, LKickSlider.getValue()+RFastSlider.getValue(), 0);

    } else if ( (diffOffset < MAX_OFFSET) && (diffOffset >= MIN_OFFSET) ) {
      direct_motor( RKickSlider.getValue(), 0, LKickSlider.getValue()+RSlowSlider.getValue(), 0);

    } else if ( (diffOffset <= MIN_OFFSET*(-1)) && (diffOffset > MAX_OFFSET*(-1)) ) {
      direct_motor( RKickSlider.getValue()+LSlowSlider.getValue(), 0, LKickSlider.getValue(), 0);
      
    } else if ( diffOffset <= MAX_OFFSET*(-1)) {
      direct_motor( RKickSlider.getValue()+LFastSlider.getValue(), 0, LKickSlider.getValue(), 0);
      
    } else {
      direct_motor( RKickSlider.getValue(), 0, LKickSlider.getValue(), 0);
    
    }
  } else {
    motor_break();
    
  }
}
void object_searching () {

/*
  if ( is_available ) {

  }
    if ( preXY_counter == 0 ) {
      preX = result.xCenter;
      preY = result.yCenter;
    } else if ( preXY_counter == 10 ) {
      preXY_counter = 0;
    } else {
      preXY_counter = preXY_counter + 1;
    }
    if ( result.yCenter > 220 ) {
      was_available = false;
    } else {
      was_available = true;
    }
  if ( switchSearch.getValue() && was_available ) {
    if ( diffRight > 0 ) {
      diffOffset = 100;
    } else {
      diffOffset = -100;
    }
/*
    if ( diffDown > 0 ) {
      diffWidth  = -100;
    } else {
      diffWidth  = 100;
    }
*/  
/*
  } else if ( ~is_available ) {
    diffWidth  = 0;
    diffOffset = 0;
    diffRight  = 0;
    diffDown   = 0;
  }
*/
}
void printResult(HUSKYLENSResult result){
    if (result.command == COMMAND_RETURN_BLOCK){
        Serial.println(String()+F("Block:xCenter=")+result.xCenter+F(",yCenter=")+result.yCenter+F(",width=")+result.width+F(",height=")+result.height+F(",ID=")+result.ID);
    }
    else if (result.command == COMMAND_RETURN_ARROW){
        Serial.println(String()+F("Arrow:xOrigin=")+result.xOrigin+F(",yOrigin=")+result.yOrigin+F(",xTarget=")+result.xTarget+F(",yTarget=")+result.yTarget+F(",ID=")+result.ID);
    }
    else{
        Serial.println("Object unknown!");
    }
}

void loop() {
  boolean was_tracking;
  HUSKYLENSResult result;
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
  
  if ( !huskylens.request() ) {
    Serial.println(F("Fail to request objects from HUSKYLENS!"));
    labelMessage.updateText("No Huskeylens!");
    was_available = false;
    is_available = false;
    preXY_counter == 0;
    search_counter = 0;

  } else if( !huskylens.isLearned() ) {
    Serial.println(F("Object not learned!"));
    labelMessage.updateText("Not Tracking!");
    was_available = false;
    is_available = false;
    preXY_counter == 0;
    search_counter = 0;

  } else if( huskylens.available() ) {
    result = huskylens.read();
    printResult(result);
    was_available = true;
    is_available = true;
    search_counter = 0;
    diffRight  = (result.xCenter - preX);
    diffDown   = (result.yCenter - preY);
    diffWidth  = (widthSlider.getValue() - result.width);     //> 0 forwarding
    diffOffset = (result.xCenter - offsetSlider.getValue());  //> 0 turn right
    labelDoffset.updateText( String(diffOffset) );
    labelDwidth.updateText( String(diffWidth) );
    object_message ( diffWidth, diffOffset);
    
  } else {
    Serial.println(F("Object disappeared!"));
    labelMessage.updateText("No Object!");
//  labelMessage.updateText( String(search_counter) );
    is_available = false;
/*    
    if ( was_available && (search_counter > 5) ) {
      search_counter = 0;
      was_available = false;
    } else {
      search_counter = search_counter + 1;
    }
    preX = 160;
    preY = 120;
    preXY_counter == 0;
    diffRight  = (result.xCenter - preX);
    diffDown   = (result.yCenter - preY);
*/
  }
  
  if ( switchStart.getValue() ) {
    if ( switchTrack.getValue() ) {
      if ( is_available ) {
        object_tracking( diffWidth, diffOffset );
      } else {
        motor_break();
      }
      was_tracking = true;
    } else {
      if ( was_tracking ) {
        motor_break();
      }
      checkAndUpdateMotor();
      was_tracking = false;
    }
  } else {
    motor_break();
    was_tracking = false;
  }
}
