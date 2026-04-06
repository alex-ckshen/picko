/*
 Example sketch for the PS3 Bluetooth library - developed by Kristian Lauszus
 For more information visit my blog: http://blog.tkjelectronics.dk/ or
 send me an e-mail:  kristianl@tkjelectronics.com
 */
#define PIN_SERVO       9

#include <PS3BT.h>
#include <usbhub.h>
#include <AFMotor.h>
#include <Servo.h>

AF_DCMotor back_right (1, MOTOR12_64KHZ);
AF_DCMotor back_left  (2, MOTOR12_64KHZ);
AF_DCMotor front_right(3, MOTOR12_64KHZ);
AF_DCMotor front_left (4, MOTOR12_64KHZ);

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

USB Usb;
//USBHub Hub1(&Usb); // Some dongles have a hub inside

BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so
/* You can create the instance of the class in two ways */
PS3BT PS3(&Btd); // This will just create the instance
//PS3BT PS3(&Btd, 0x00, 0x15, 0x83, 0x3D, 0x0A, 0x57); // This will also store the bluetooth address - this can be obtained from the dongle when running the sketch
/*
bool printTemperature, printAngle;
int pre_left_x;
int pre_left_y;
int pre_right_x;
int pre_right_y;
*/
int left_x;
int left_y;
int right_x;
int right_y;

//-- servo
Servo     servo_pick;
const int pick_down = 130;
const int pick_up   = 180;

void pickup() {
  servo_pick.write(pick_down);
  delay(500);
  servo_pick.write(pick_up);
}

void setup() {
  Serial.begin(115200);
#if !defined(__MIPSEL__)
  while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif
if (Usb.Init() == -1) {
    Serial.print(F("\r\nOSC did not start"));
    while (1); //halt
  }
  Serial.print(F("\r\nPS3 Bluetooth Library Started"));

  servo_pick.attach(PIN_SERVO);
  servo_pick.write(pick_up);
  /*
    pre_right_x = 127;
    pre_right_y = 127;   
    pre_left_x  = 127;   
    pre_left_y  = 127;   
  */
    right_x = 127;
    right_y = 127;   
    left_x  = 127;   
    left_y  = 127;   
}

void loop() {
  Usb.Task();
  int speed_y;
  int speed_x;
  
  if (PS3.PS3Connected || PS3.PS3NavigationConnected) {
    if (PS3.getButtonClick(START)) {
        Serial.print(F("\r\nStart"));
        pickup();
        //printAngle = !printAngle;
    }
    
    right_x = PS3.getAnalogHat(RightHatX);
    right_y = PS3.getAnalogHat(RightHatY);   
    left_x  = PS3.getAnalogHat(LeftHatX);   
    left_y  = PS3.getAnalogHat(LeftHatY); 

    if ( right_y < 127 ) speed_y = 254 - right_y;
    else                 speed_y = right_y;
    
    front_right.setSpeed( speed_y );
    front_left.setSpeed( speed_y );
    back_right.setSpeed( speed_y );
    back_left.setSpeed( speed_y );


    if ( left_x < 127 ) speed_x = 254 - left_x;
    else                speed_x = left_x;
      
    front_right.setSpeed( speed_x );
    front_left.setSpeed( speed_x );
    back_right.setSpeed( speed_x );
    back_left.setSpeed( speed_x );

    if ( right_y < 117 ) {
      front_right.run(FORWARD);
      front_left.run(FORWARD);
      back_right.run(FORWARD);
      back_left.run(FORWARD);
    } else if ( right_y > 137 ) {
      front_right.run(BACKWARD);
      front_left.run(BACKWARD);
      back_right.run(BACKWARD);
      back_left.run(BACKWARD);
    } /*else {
      front_right.run(RELEASE);
      front_left.run(RELEASE);
      back_right.run(RELEASE);
      back_left.run(RELEASE);
    }*/
    else if ( left_x < 117 ) {
      front_right.run(FORWARD);
      front_left.run(BACKWARD);
      back_right.run(FORWARD);
      back_left.run(BACKWARD);
    } else if ( left_x > 137 ) {
      front_right.run(BACKWARD);
      front_left.run(FORWARD);
      back_right.run(BACKWARD);
      back_left.run(FORWARD);
    } else {
      front_right.run(RELEASE);
      front_left.run(RELEASE);
      back_right.run(RELEASE);
      back_left.run(RELEASE);
    }    
/*    
    pre_right_x = right_x;
    pre_right_y = right_y;   
    pre_left_x  = left_x;   
    pre_left_y  = left_y;
*/ 
/*
    if (PS3.getAnalogHat(LeftHatX) > 137 || PS3.getAnalogHat(LeftHatX) < 117 ||
        PS3.getAnalogHat(LeftHatY) > 137 || PS3.getAnalogHat(LeftHatY) < 117 || 
        PS3.getAnalogHat(RightHatX) > 137 || PS3.getAnalogHat(RightHatX) < 117 ||
        PS3.getAnalogHat(RightHatY) > 137 || PS3.getAnalogHat(RightHatY) < 117) {
        
      Serial.print(F("\r\nLeftHatX: "));
      Serial.print(PS3.getAnalogHat(LeftHatX));
      Serial.print(F("\tLeftHatY: "));
      Serial.print(PS3.getAnalogHat(LeftHatY));
      
      if (PS3.PS3Connected) { // The Navigation controller only have one joystick
        Serial.print(F("\tRightHatX: "));
        Serial.print(PS3.getAnalogHat(RightHatX));
        Serial.print(F("\tRightHatY: "));
        Serial.print(PS3.getAnalogHat(RightHatY));
      }
    }
*/
    // Analog button values can be read from almost all buttons
/*
    if (PS3.getAnalogButton(L2) || PS3.getAnalogButton(R2)) {
      Serial.print(F("\r\nL2: "));
      Serial.print(PS3.getAnalogButton(L2));
      if (PS3.PS3Connected) {
        Serial.print(F("\tR2: "));
        Serial.print(PS3.getAnalogButton(R2));
      }
    }
*/
/*
    if (PS3.getButtonClick(PS)) {
      Serial.print(F("\r\nPS"));
      PS3.disconnect();
    } else {
      if (PS3.getButtonClick(TRIANGLE)) {
        Serial.print(F("\r\nTriangle"));
        PS3.setRumbleOn(RumbleLow);
      }
      if (PS3.getButtonClick(CIRCLE)) {
        Serial.print(F("\r\nCircle"));
        PS3.setRumbleOn(RumbleHigh);
      }
      if (PS3.getButtonClick(CROSS)) {
        back_right.run(RELEASE);  
        back_left.run(RELEASE);  
        Serial.print(F("\r\nCross"));
      }
      if (PS3.getButtonClick(SQUARE))
        Serial.print(F("\r\nSquare"));

      if (PS3.getButtonClick(UP)) {
        Serial.print(F("\r\nUp"));
        if (PS3.PS3Connected) {
          PS3.setLedOff();
          PS3.setLedOn(LED4);
        }
      }
      if (PS3.getButtonClick(RIGHT)) {
        Serial.print(F("\r\nRight"));
        if (PS3.PS3Connected) {
          PS3.setLedOff();
          PS3.setLedOn(LED1);
        }
      }
      if (PS3.getButtonClick(DOWN)) {
        Serial.print(F("\r\nDown"));
        if (PS3.PS3Connected) {
          PS3.setLedOff();
          PS3.setLedOn(LED2);
        }
      }
      if (PS3.getButtonClick(LEFT)) {
        Serial.print(F("\r\nLeft"));
        if (PS3.PS3Connected) {
          PS3.setLedOff();
          PS3.setLedOn(LED3);
        }
      }

      if (PS3.getButtonClick(L1))
        Serial.print(F("\r\nL1"));
      if (PS3.getButtonClick(L3))
        Serial.print(F("\r\nL3"));
      if (PS3.getButtonClick(R1))
        Serial.print(F("\r\nR1"));
      if (PS3.getButtonClick(R3))
        Serial.print(F("\r\nR3"));

      if (PS3.getButtonClick(SELECT)) {
        Serial.print(F("\r\nSelect - "));
        PS3.printStatusString();
      }
      if (PS3.getButtonClick(START)) {
        Serial.print(F("\r\nStart"));
        printAngle = !printAngle;
      }
    }
*/
/*
#if 0 // Set this to 1 in order to see the angle of the controller
    if (printAngle) {
      Serial.print(F("\r\nPitch: "));
      Serial.print(PS3.getAngle(Pitch));
      Serial.print(F("\tRoll: "));
      Serial.print(PS3.getAngle(Roll));
    }
#endif
#if 0 // Set this to 1 in order to enable support for the Playstation Move controller
  } else if (PS3.PS3MoveConnected) {
    if (PS3.getAnalogButton(T)) {
      Serial.print(F("\r\nT: "));
      Serial.print(PS3.getAnalogButton(T));
    }
    if (PS3.getButtonClick(PS)) {
      Serial.print(F("\r\nPS"));
      PS3.disconnect();
    }
    else {
      if (PS3.getButtonClick(SELECT)) {
        Serial.print(F("\r\nSelect"));
        printTemperature = !printTemperature;
      }
      if (PS3.getButtonClick(START)) {
        Serial.print(F("\r\nStart"));
        printAngle = !printAngle;
      }
      if (PS3.getButtonClick(TRIANGLE)) {
        Serial.print(F("\r\nTriangle"));
        PS3.moveSetBulb(Red);
      }
      if (PS3.getButtonClick(CIRCLE)) {
        Serial.print(F("\r\nCircle"));
        PS3.moveSetBulb(Green);
      }
      if (PS3.getButtonClick(SQUARE)) {
        Serial.print(F("\r\nSquare"));
        PS3.moveSetBulb(Blue);
      }
      if (PS3.getButtonClick(CROSS)) {
        Serial.print(F("\r\nCross"));
        PS3.moveSetBulb(Yellow);
      }
      if (PS3.getButtonClick(MOVE)) {
        PS3.moveSetBulb(Off);
        Serial.print(F("\r\nMove"));
        Serial.print(F(" - "));
        PS3.printStatusString();
      }
    }
    if (printAngle) {
      Serial.print(F("\r\nPitch: "));
      Serial.print(PS3.getAngle(Pitch));
      Serial.print(F("\tRoll: "));
      Serial.print(PS3.getAngle(Roll));
    }
    else if (printTemperature) {
      Serial.print(F("\r\nTemperature: "));
      Serial.print(PS3.getTemperature());
    }
#endif
*/
  } else { 
    front_right.run(RELEASE);
    front_left.run(RELEASE);
    back_right.run(RELEASE);
    back_left.run(RELEASE);
  }
}
