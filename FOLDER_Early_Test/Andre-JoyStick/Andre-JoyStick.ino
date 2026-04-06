/*
  This example configures LinkIt 7697 to a reciver of the iOS LinkIt Remote App

  created Aug 2017
*/
#include <LRemote.h>
#include <Servo.h>
//#include "HUSKYLENS.h"

LRemoteJoyStick stickLeft;
LRemoteJoyStick stickRight;
LRemoteLabel    labelLeft;
LRemoteLabel    labelRightX;
LRemoteLabel    labelRightY;
LRemoteSwitch   switchStart;
LRemoteLabel    labelStart;

Servo myservo;  // create servo object to control a servo
//HUSKYLENS huskylens;
//void printResult(HUSKYLENSResult result);

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  Serial.println("Start configuring remote");

  // Initialize GPIO
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);

  // Setup the Remote Control's UI canvas
  LRemote.setName("Andre");
  LRemote.setOrientation(RC_LANDSCAPE);
  LRemote.setGrid(5, 3);

  // Add left stick
  stickLeft.setPos(0, 1);
  stickLeft.setSize(2, 2);
  stickLeft.setColor(RC_ORANGE);
  LRemote.addControl(stickLeft);

  // Add a simple text label
  labelLeft.setPos(0, 0);
  labelLeft.setSize(2, 1);
  labelLeft.setColor(RC_GREY);
  labelLeft.setText("(0, 0)");
  LRemote.addControl(labelLeft);

  // Add Right stick
  stickRight.setPos(3, 1);
  stickRight.setSize(2, 2);
  stickRight.setColor(RC_BLUE);
  LRemote.addControl(stickRight);

  labelRightX.setPos(3, 0);
  labelRightX.setSize(1, 1);
  labelRightX.setColor(RC_GREY);
  labelRightX.setText("0");
  LRemote.addControl(labelRightX);

  labelRightY.setPos(4, 0);
  labelRightY.setSize(1, 1);
  labelRightY.setColor(RC_GREY);
  labelRightY.setText("0");
  LRemote.addControl(labelRightY);
  
  switchStart.setPos(2, 1);
  switchStart.setSize(1, 2);
  switchStart.setColor(RC_ORANGE);
  switchStart.setText("Switch");
  LRemote.addControl(switchStart);

  labelStart.setPos(2, 0);
  labelStart.setSize(1, 1);
  labelStart.setColor(RC_BLUE);
  labelStart.setText("Press to Start");
  LRemote.addControl(labelStart);

  // Start broadcasting our remote contoller
  // This method implicitly initialized underlying BLE subsystem
  // to create a BLE peripheral, and then
  // start advertisement on it.
  LRemote.begin();
//--Serial.println("begin() returned");
  pinMode(10, OUTPUT);   //設定PIN10為輸出
  pinMode(12, OUTPUT);   //設定PIN12為輸出
  pinMode(13, OUTPUT);   //設定PIN13為輸出
  pinMode(17, OUTPUT);   //設定PIN17為輸出
  
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
/*  
    Serial.begin(115200);
    Wire.begin();
    while (!huskylens.begin(Wire))
    {
        Serial.println(F("Begin failed!"));
        Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>I2C)"));
        Serial.println(F("2.Please recheck the connection."));
        delay(100);
    }
*/    
}

void checkAndUpdateMotor(LRemoteLabel& labelX, LRemoteLabel& labelY, LRemoteJoyStick& stick) {
//int old_dy = 0;
  int new_x;
  int new_y;

  if(stick.isValueChanged()){
    LRemoteDirection d = stick.getValue();
    int f_right = 100+d.y-d.x;
    int f_left  = 100+d.y+d.x;
    int b_right = 100-d.y+d.x;
    int b_left  = 100-d.y-d.x;
    
    if ( f_right > 150 ) f_right = 150;
    if ( f_left  > 150 ) f_left  = 150;
    if ( b_right > 150 ) b_right = 150;
    if ( b_left  > 150 ) b_left  = 150;
      
    if ( d.y > 0 ) {
      new_x = f_right;
      new_y = f_left;
      analogWrite(10, f_right ); //right INA +
      analogWrite(12, 0 );       //right INB -
      analogWrite(13, f_left );  //left  INB -
      analogWrite(17, 0 );       //left  INA +
    } else if ( d.y < 0 ) {
      new_x = b_right;
      new_y = b_left;
      analogWrite(10, 0 );       //right INA +
      analogWrite(12, b_right ); //right INB -
      analogWrite(13, 0 );       //left  INB -
      analogWrite(17, b_left );  //left  INA +
    } else {
      new_x = 0;
      new_y = 0;
      analogWrite(10, 0);  //right INA +
      analogWrite(12, 0);  //right INB -
      analogWrite(13, 0);  //left  INB -
      analogWrite(17, 0);  //left  INA +
    }
    labelX.updateText(String(new_x));
    labelY.updateText(String(new_y));
  }
}

void checkAndUpdateLabel(LRemoteLabel& label, LRemoteJoyStick& stick) {
  if(stick.isValueChanged()){
    LRemoteDirection d = stick.getValue();
    // d.x and d.y are the value from the Joystick component:
    // d.x : -100 ~ 100, where 0 is center, -100 is leftmost, and 100 is rightmost.
    // d.y : -100 ~ 100, where 0 is center, -100 is bottommost, and 100 is topmost.

    // you can print d directly.
    Serial.println(d);

    // in this example, we simply pass the value of d.x/d.y
    // back to the LinkIt Remote app.
    label.updateText(d.toString());
  } 
}
/*
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
*/
void loop() {
  // check if we are connect by some 
  // BLE central device, e.g. an mobile app
  if(!LRemote.connected()) {
    Serial.println("waiting for connection");
    delay(1000);
  } else {
    // The interval between button down/up
    // can be very short - e.g. a quick tap
    // on the screen.
    // We could lose some event if we
    // delay something like 100ms.
    delay(15);
  }
  
  // Process the incoming BLE write request
  // and translate them to control events
  LRemote.process();

  if ( !switchStart.getValue() ) {
//  labelStart.updateText("Press to Start");  //Linkit remote APP fail. don't use 
    analogWrite(10, 0);  //right INA +
    analogWrite(12, 0);  //right INB -
    analogWrite(13, 0);  //left  INB -
    analogWrite(17, 0);  //left  INA +
  } else {
//  labelStart.updateText("Press to Stop");  //Linkit remote APP fail. don't use 
    checkAndUpdateMotor(labelRightX, labelRightY, stickRight);
  }
  
  checkAndUpdateLabel(labelLeft, stickLeft);
/*
    if (!huskylens.request()) Serial.println(F("Fail to request data from HUSKYLENS, recheck the connection!"));
    else if(!huskylens.isLearned()) Serial.println(F("Nothing learned, press learn button on HUSKYLENS to learn one!"));
    else if(!huskylens.available()) Serial.println(F("No block or arrow appears on the screen!"));
    else
    {
        Serial.println(F("###########"));
        while (huskylens.available())
        {
            HUSKYLENSResult result = huskylens.read();
            printResult(result);
        }    
    }
*/
}
