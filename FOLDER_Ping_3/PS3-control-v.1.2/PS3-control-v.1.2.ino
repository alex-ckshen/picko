/*
 Example sketch for the PS3 Bluetooth library - developed by Kristian Lauszus
 For more information visit my blog: http://blog.tkjelectronics.dk/ or
 send me an e-mail:  kristianl@tkjelectronics.com
*/
#include <PS3BT.h>
#include <usbhub.h>
#include <AFMotor.h>
// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

AF_DCMotor back_right (1, MOTOR12_64KHZ);
AF_DCMotor back_left  (2, MOTOR12_64KHZ);
AF_DCMotor front_right(3, MOTOR12_64KHZ);
AF_DCMotor front_left (4, MOTOR12_64KHZ);

USB Usb; //USBHub Hub1(&Usb); // Some dongles have a hub inside
BTD Btd(&Usb); // You have to create the Bluetooth Dongle instance like so // You can create the instance of the class in two ways
PS3BT PS3(&Btd); // This will just create the instance //PS3BT PS3(&Btd, 0x00, 0x15, 0x83, 0x3D, 0x0A, 0x57); // This will also store the bluetooth address - this can be obtained from the dongle when running the sketch

#define motor_speed 200
int left_x;
int left_y;
int right_x;
int right_y;
boolean right_button = false
boolean left_button  = false

void motor_movement( int front_left_speed, int front_right_speed, int back_left_speed, int back_right_speed ) {
  
  if( front_left_speed > 0 ) {
    front_left.setSpeed( front_left_speed );
    front_left.run(FORWARD);
  } else if ( front_left_speed < 0 ) {
    front_left.setSpeed( -front_left_speed );
    front_left.run(BACKWARD);
  } else {
    front_left.run(RELEASE);
  }
    
  if( front_right_speed > 0 ) {
    front_right.setSpeed( front_right_speed );
    front_right.run(FORWARD);
  } else if ( front_right_speed < 0 ) {
    front_right.setSpeed( -front_right_speed );
    front_right.run(BACKWARD);
  } else {
    front_right.run(RELEASE);
  }
  
  if( back_left_speed > 0 ) {
    back_left.setSpeed( back_left_speed );
    back_left.run(FORWARD);
  } else if ( back_left_speed < 0 ) {
    back_left.setSpeed( -back_left_speed );
    back_left.run(BACKWARD);
  } else {
    back_left.run(RELEASE);
  }
  
    if( back_right_speed > 0 ) {
    back_right.setSpeed( back_right_speed );
    back_right.run(FORWARD);
  } else if ( back_right_speed < 0 ) {
    back_right.setSpeed( -back_right_speed );
    back_right.run(BACKWARD);
  } else {
    back_right.run(RELEASE);
  }
}

void motor_control(int motor_state) {
  if (motor_state == 0){        //stop
    motor_movement(0 , 0 , 0 , 0);
    }
    else if (motor_state == 1){ //foward
    motor_movement(motor_speed , motor_speed , motor_speed , motor_speed);
      }
    else if (motor_state == 2){ //backwards
    motor_movement(-motor_speed , -motor_speed , -motor_speed , -motor_speed);
      }
    else if (motor_state == 3){ //slide right
    motor_movement(motor_speed , -motor_speed , -motor_speed , motor_speed);
      }
    else if (motor_state == 4){ //slide left
    motor_movement(-motor_speed , motor_speed , motor_speed , -motor_speed);
      }
    else if (motor_state == 5){ //right up
    motor_movement(motor_speed , 0 , 0 , motor_speed);
      }
    else if (motor_state == 6){ //left up
    motor_movement(0 , motor_speed , motor_speed , 0);
      }
    else if (motor_state ==7){ //left down
    motor_movement(-motor_speed , 0 , 0 , -motor_speed);
      }
    else if (motor_state == 8){ //right down
    motor_movement(0 , -motor_speed , -motor_speed , 0);
      }
    else if (motor_state == 9){ //turn right
    motor_movement(motor_speed , -motor_speed , motor_speed , -motor_speed);
      }
    else if (motor_state == 10){ //turn left
    motor_movement(-motor_speed , motor_speed , -motor_speed , motor_speed);
      }
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

    right_x = 127;
    right_y = 127;
    left_x  = 127;
    left_y  = 127;
}

void loop() {
  Usb.Task();
  if (PS3.PS3Connected || PS3.PS3NavigationConnected) {
    right_x = PS3.getAnalogHat(RightHatX);
    right_y = PS3.getAnalogHat(RightHatY);
    left_x  = PS3.getAnalogHat(LeftHatX);
    left_y  = PS3.getAnalogHat(LeftHatY);
    if (PS3.getButtonClick(L1)) left_button = true;
    if (PS3.getButtonClick(R1)) right_button = false;
    Serial.print(F("\r\nLeftHatX: "));
    Serial.print(PS3.getAnalogHat(LeftHatX));
    Serial.print(F("\tLeftHatY: "));
    Serial.print(PS3.getAnalogHat(LeftHatY));
    Serial.print(F("\tRightHatX: "));
    Serial.print(PS3.getAnalogHat(RightHatX));
    Serial.print(F("\tRightHatY: "));
    Serial.print(PS3.getAnalogHat(RightHatY));
    if (PS3.getButtonClick(L1)) Serial.print(F("\r\nL1"));
    if (PS3.getButtonClick(R1)) Serial.print(F("\r\nR1"));

    if     ( right_x > 137 && right_y < 137 )             motor_control(5);//up right
    else if( right_x < 117 && right_y < 137 )             motor_control(6);//up left
    else if( right_x > 137 && right_y > 117 )             motor_control(8);//down right
    else if( right_x < 117 && right_y > 117 )             motor_control(7);//down left
    else if( 107 < right_x < 147 && right_y > 117 )       motor_control(2);//down
    else if( 107 < right_x < 147 && right_y < 137 )       motor_control(1);//up
    else if( right_x > 137 && 107 < right_y < 147 )       motor_control(3);//right
    else if( right_x < 117 && 107 < right_y < 147 )       motor_control(4);//left
    else                                                  motor_control(0); 

    if( left_button = true )                           motor_control(9);//left joystick right
    if( right_button = true )                          motor_control(10);//left joystick left
  } else {
  motor_control(0)
  }
}
