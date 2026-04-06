/*

*/
#include <HUSKYLENS.h>
#include <SoftwareSerial.h>
#include <LRemote.h>
#include <Servo.h>
//#include <PID_v1.h>

//Define Variables we'll be connecting to
//double setPIDpoint;
//double rightPIDin, rightPIDout;
//double leftPIDin, leftPIDout;

//Specify the links and initial tuning parameters
//PID rightPID(&rightPIDin, &rightPIDout, &setPIDpoint,2,5,1, DIRECT);
//PID leftPID (&leftPIDin,  &leftPIDout,  &setPIDpoint,2,5,1, DIRECT);

LRemoteLabel    lableMessage;
LRemoteSlider   speedSlider;
LRemoteSlider   diffSlider;
LRemoteSwitch   switchStart;
LRemoteSwitch   switchAuto;
LRemoteButton   buttonPick;
LRemoteLabel    lableRightS;
LRemoteLabel    lableLeftS;
LRemoteJoyStick stickManual;

Servo          myservo;  
HUSKYLENS      huskylens;
SoftwareSerial mySerial(2, 3); // RX, TX of linkit 7697
void printResult(HUSKYLENSResult result);
void motor_control(int right_F, int right_B , int left_F, int left_B );

boolean previous_Auto;
boolean was_available;

void setup() {
//-- Initialize serial and wait for port to open:
  Serial.begin(9600);
  mySerial.begin(9600);
  while (!huskylens.begin(mySerial)) {
        Serial.println(F("Begin failed!"));
        Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>Serial 9600)"));
        Serial.println(F("2.Please recheck the connection."));
        delay(100);
  }
  
  huskylens.writeAlgorithm(ALGORITHM_OBJECT_TRACKING); //Switch the algorithm to object tracking.
  
//-- Setup the Remote Control's UI canvas
  Serial.println("Start configuring remote");
  LRemote.setName("Andre");
  LRemote.setOrientation(RC_LANDSCAPE);
  LRemote.setGrid(5, 3);

  lableMessage.setPos(0, 0);
  lableMessage.setSize(2, 1);
  lableMessage.setColor(RC_BLUE);
  lableMessage.setText("Press to Start");
  LRemote.addControl(lableMessage);
  
  speedSlider.setPos(0, 1);
  speedSlider.setSize(2, 1);
  speedSlider.setText("Speed");
  speedSlider.setValueRange(0, 100, 0);
  speedSlider.setColor(RC_BLUE);
  LRemote.addControl(speedSlider);
    
  diffSlider.setPos(0, 2);
  diffSlider.setSize(2, 1);
  diffSlider.setText("Difference");
  diffSlider.setValueRange(0, 100, 0);
  diffSlider.setColor(RC_BLUE);
  LRemote.addControl(diffSlider);

  switchStart.setPos(2, 0);
  switchStart.setSize(1, 1);
  switchStart.setColor(RC_ORANGE);
  switchStart.setText("Start");
  LRemote.addControl(switchStart);
  
  switchAuto.setPos(2, 1);
  switchAuto.setSize(1, 1);
  switchAuto.setColor(RC_ORANGE);
  switchAuto.setText("Auto Search");
  LRemote.addControl(switchAuto);

  buttonPick.setPos(2, 2);
  buttonPick.setSize(1, 1);
  buttonPick.setColor(RC_ORANGE);
  buttonPick.setText("Pick");
  LRemote.addControl(buttonPick);
  
  lableLeftS.setPos(3, 0);
  lableLeftS.setSize(1, 1);
  lableLeftS.setColor(RC_GREY);
  lableLeftS.setText("0");
  LRemote.addControl(lableLeftS);
  
  lableRightS.setPos(4, 0);
  lableRightS.setSize(1, 1);
  lableRightS.setColor(RC_GREY);
  lableRightS.setText("0");
  LRemote.addControl(lableRightS);
  
  stickManual.setPos(3, 1);
  stickManual.setSize(2, 2);
  stickManual.setColor(RC_BLUE);
  LRemote.addControl(stickManual);

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
  motor_control(0,0,0,0);
//-- Initialize GPIO
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  previous_Auto = false;
  was_available = false;

//-- initialize the variables we're linked to
//  rightPIDin = 0;
//  leftPIDin = 0;
//  setPIDpoint = 100;
//-- turn the PID on
//  rightPID.SetMode(AUTOMATIC);
//  leftPID.SetMode(AUTOMATIC);

}

void loop() {  
//-- check if connected by BLE central devices,
//-- e.g. an mobile app
  if(!LRemote.connected()) {
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
  
  if ( !switchStart.getValue() ) {
    motor_control(0,0,0,0);
  } else if ( !switchAuto.getValue() ) {
    if ( previous_Auto ) motor_control(0,0,0,0);
    checkAndUpdateMotor();
    previous_Auto = false;
  } else {
    object_tracking();
    previous_Auto = true;
  }
}

void motor_control(int right_F, int right_B , int left_F, int left_B ) {
  int rightSpeed;
  int leftSpeed;

  analogWrite(10, right_F);  //right INA +
  analogWrite(12, right_B);  //right INB -
  analogWrite(13, left_F);   //left  INB -
  analogWrite(17, left_B);   //left  INA +

  if ( right_F == 0 ) rightSpeed = right_B;
  else                rightSpeed = right_F;

  if ( left_F == 0 )  leftSpeed = left_B;
  else                leftSpeed = left_F;
  
  lableRightS.updateText(String(rightSpeed));
  lableLeftS.updateText(String(leftSpeed));
}

void forward_kick () {
  motor_control( 130, 0, 130, 0);
  delay(200);
}

void backward_kick () {
  motor_control( 0, 130, 0, 130);
  delay(200);
}

void forward_break () {
  motor_control( 80, 0, 80, 0);
  delay(200);
  motor_control( 50, 0, 50, 0);
  delay(100);
  motor_control( 0, 0, 0, 0);
}

void forward_break () {
  motor_control( 0, 80, 0, 80);
  delay(200);
  motor_control( 0, 50, 0, 50);
  delay(100);
  motor_control( 0, 0, 0, 0);
}

void forward2backward () {
  motor_control( 80, 0, 80, 0);
  delay(200);
  motor_control( 50, 0, 50, 0);
  delay(100);
  motor_control( 0, 0, 0, 0);
  delay(100);
  motor_control( 0, 100, 0, 10);
  delay(200);
  motor_control( 0, 80, 0, 80);
  delay(100);
}



void checkAndUpdateMotor() {

  if( stickManual.isValueChanged() ) {
    LRemoteDirection d = stickManual.getValue();
    int f_right = 100+d.y-d.x;
    int f_left  = 100+d.y+d.x;
    int b_right = 100-d.y+d.x;
    int b_left  = 100-d.y-d.x;
    
    if ( d.y > 0 ) {
      motor_control( f_right, 0, f_left, 0);
    } else if ( d.y < 0 ) {
      motor_control( 0, b_right, 0, b_left);
    } else {
      motor_control(0,0,0,0);
    }
  }
}

void object_tracking() {
  int minWidth = 160;
  int maxWidth = 160;
  int diff_backward;
  int diff_forward;
  int add_left2right;
  int add_right2left;
  int speed_backward;
  int speed_forward;
  int speed_right;
  int speed_left;

  if (!huskylens.request()) {
    Serial.println(F("Fail to request objects from HUSKYLENS!"));
    lableMessage.updateText("No Huskeylens!");
    was_available = false;
  } else if(!huskylens.isLearned()) {
    Serial.println(F("Object not learned!"));
    lableMessage.updateText("Not Tracking!");
    motor_control(0,0,0,0);
    was_available = false;
  } else if(!huskylens.available()) {
    Serial.println(F("Object disappeared!"));
    lableMessage.updateText("No Object!");
    if ( was_available ) {      
      motor_control( 80, 0, 80, 0);
      delay(100);
      motor_control( 50, 0, 50, 0);
      delay(100);
      motor_control( 0, 80, 0, 80);
      delay(100);
      motor_control( 0, 100, 0, 100);
      delay(100);
      motor_control( 0, 80, 0, 80);
      delay(100);
    }
    motor_control(0,0,0,0);
    was_available = false;
  } else {
    HUSKYLENSResult result = huskylens.read();
    printResult(result);

    diff_backward  = (result.width - minWidth); //max=240-160: min=0
    diff_forward   = (minWidth - result.width); //max=160+10: min=0
    add_left2right = (result.xCenter - 160);    //move right
    add_right2left = (160 - result.xCenter);    //move left

    if ( result.width > minWidth ) {
      lableMessage.updateText("MOVE_BACKWARD!");
      speed_backward = speedSlider.getValue()+diff_backward*0.2;
      if ( speed_backward < 100 ) {
        speed_backward = 100;
      } else if ( speed_backward > 120) {
        speed_backward = 120;
      }
      motor_control( 0, speed_backward, 0, speed_backward);
            
    } else if ( result.width < minWidth ) {
      speed_forward = speedSlider.getValue()+diff_forward*0.2;
      if ( speed_forward < 100 ) {
        speed_forward = 100;
      } else if ( speed_forward > 120 ) {
        speed_forward = 120;
      }

      if ( result.xCenter > 160 ) {
        lableMessage.updateText("MOVE_RIGHT!");
        speed_left = speedSlider.getValue()+add_left2right*0.5;
        if ( speed_left < 120 ) {
          speed_left = 120;
        } else if ( speed_left > 200 ) {
          speed_left = 200;
        }
        motor_control( speed_forward, 0, speed_left, 0);

      } else if ( result.xCenter < 160 ) {
        lableMessage.updateText("MOVE_LEFT!");
        speed_right = speedSlider.getValue()+add_right2left*0.5;
        if ( speed_right < 120 ) {
          speed_right = 120;
        } else if ( speed_right > 200 ) {
          speed_right = 200;
        }
        motor_control( speed_right, 0, speed_forward, 0);
        
      } else {
        lableMessage.updateText("MOVE_FORWARD!");
        if ( speed_forward < 100 ) {
          speed_forward = 100;
        } else if ( speed_forward > 100 ) {
          speed_forward = 100;
        }
        motor_control( speed_forward, 0, speed_forward, 0);
      } 
    } else {
      lableMessage.updateText("Stop!");
      motor_control(0,0,0,0);
      
    }
    was_available = true;
  }
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

/*
void PID_object_tracking() {
  int minWidth  = 160; //
  int maxWidth  = 190; //190+10

//rightPID.Compute();
//leftPID.Compute();

    if (!huskylens.request()) {
      Serial.println(F("Fail to request objects from HUSKYLENS!"));
      lableMessage.updateText("No Huskeylens!");
      was_available = false;
    } else if(!huskylens.isLearned()) {
      Serial.println(F("Object not learned!"));
      lableMessage.updateText("Not Tracking!");
      NO_MOVE;
      was_available = false;
    } else if(!huskylens.available()) {
      Serial.println(F("Object disappeared!"));
      lableMessage.updateText("No Object!");
      if ( was_available ) MOVE_BACKWARD;
      delay(100);
      NO_MOVE;
      was_available = false;
    } else {
      HUSKYLENSResult result = huskylens.read();
      printResult(result);

//rightPIDin = 0;
//leftPIDin = 0;
//analogWrite(3,leftPIDout);
//analogWrite(3,rightPIDout);

      diff_backward = 90 + (result.width - maxWidth); //max=240-200:min=
      diff_forward  = 50 + (minWidth - result.width); /160-10
      diff_right = 160 - result.xCenter;
      diff_left  = result.xCenter - 160;
 
      if ( result.width > maxWidth ) {
        lableMessage.updateText("MOVE_BACKWARD!");
        motor_control( 0, , 0, b_left);

      } else if ( (result.width < minWidth) && (result.xCenter > 165)) {
        lableMessage.updateText("MOVE_RIGHT!");
        motor_control( , b_right, 0);

      } else if ( (result.width < minWidth) && (result.xCenter < 155)) {
        lableMessage.updateText("MOVE_LEFT!");
        motor_control( 0, b_right, 0);

      } else if ( result.width < minWidth ) {
        lableMessage.updateText("MOVE_FORWARD!");
        motor_control( 0, b_right, 0);

      } else {
        lableMessage.updateText("NO_MOVE!");
        NO_MOVE;
      }
      was_available = true;
 }
}
*/

/*
    int32_t error;
    int left = 0;
    int right = 0;
int widthLevel = 50;
int xLeft = 160-40;
int xRight = 160+40;
bool isTurning = false;
bool isTurningLeft = true;

bool isInside(int value, int min, int max){
    return (value >= min && value <= max);
}

void search_ball_ex () {
    if (!huskylens.request()) {
      Serial.println(F("Fail to request objects from HUSKYLENS!"));
    } else if(!huskylens.isLearned()) {
      Serial.println(F("Object not learned!"));
      motor_control(0,0,0,0); // (0,0);
    } else if(!huskylens.available()) {
      Serial.println(F("Object disappeared!"));
    } else {
        HUSKYLENSResult result = huskylens.read();
        printResult(result);
        
        if (result.width < widthLevel){
            widthLevel = 65;
            if (isInside(result.xCenter, 0, xLeft)){
                if (isTurningLeft){
                    if (!isTurning){
                        motor_control(110,0,130,0); // (200-50,200+50);
                    }
                }
                else{
                    if (isTurning){
                        isTurning = false;
                        isTurningLeft = !isTurningLeft;
                    }
                    motor_control(110,0,130,0); // (200-50,200+50);
                }
            }
            else if (isInside(result.xCenter, xLeft, xRight)){
                if (isTurning){
                    isTurning = false;
                    isTurningLeft = !isTurningLeft;
                }
                motor_control(120,0,120,0); // (200,200);
            }
            else if (isInside(result.xCenter, xRight, 320)){
                if (isTurningLeft){
                    if (isTurning){
                        isTurning = false;
                        isTurningLeft = !isTurningLeft;
                    }
                    motor_control(130,0,110,0); // (200+50,200-50);
                }
                else{
                    if (!isTurning){
                        motor_control(1300,0,110,0); // (200+50,200-50);
                    }
                }
            }
        }
        else //result.width >= widthLevel
        {
            widthLevel = 55;
            isTurning = true;
            if (isTurningLeft) {
              motor_control(0,0,0,0); // (0,200);
            } else {
              motor_control(0,0,0,0); // (200,0);
            }
        }
    }
}
*/
