/***************************************************
 HUSKYLENS An Easy-to-use AI Machine Vision Sensor
 <https://www.dfrobot.com/product-1922.html>
 
 ***************************************************
 This example shows how to play with object tracking.
 
 Created 2020-03-13
 By [Angelo qiao](Angelo.qiao@dfrobot.com)
 
 GNU Lesser General Public License.
 See <http://www.gnu.org/licenses/> for details.
 All above must be included in any redistribution
 ****************************************************/

/*************** Camera To Action v0 ***************
  Goals:
  - Ball is always in front of the robot.
  - Using basic FBRL controls from Motor Control v1:
    X X : Stop
    F X : Forward
    B X : Backward
    R X : Right
      F : Front Right
      B : Back Right
      O : Rotate Right
    L X : Left
      F : Front Left
      B : Back Left
      O : Rotate Left
****************************************************/

#include <AFMotor.h>
#include <HUSKYLENS.h>
#include <Wire.h>

AF_DCMotor back_right (1, MOTOR12_64KHZ);
AF_DCMotor back_left  (2, MOTOR12_64KHZ);
AF_DCMotor front_right(3, MOTOR12_64KHZ);
AF_DCMotor front_left (4, MOTOR12_64KHZ);

HUSKYLENS huskylens;

#define motor_speed 100
#define center_size 40

#define DBG 1

int getxCenter;
int getyCenter;
int xBorder0;
int xBorder1;
int yBorder0;
int yBorder1;

int xStatus = 3; //0 for less than wanted, 1 good, 2 for more
int yStatus = 3; //0 for less than wanted, 1 good, 2 for more

/***************** Motor Control v1 *****************/
void movement( int front_left_speed , int back_left_speed , int front_right_speed , int back_right_speed ) {

  if( front_left_speed > 0 ) {
    front_left.setSpeed( front_left_speed );
    front_left.run(FORWARD);
  } else if ( front_left_speed < 0 ) {
    front_left.setSpeed( -front_left_speed );
    front_left.run(BACKWARD);
  } else {
    front_left.run(RELEASE);
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

  if( front_right_speed > 0 ) {
    front_right.setSpeed( front_right_speed );
    front_right.run(FORWARD);
  } else if ( front_right_speed < 0 ) {
    front_right.setSpeed( -front_right_speed );
    front_right.run(BACKWARD);
  } else {
    front_right.run(RELEASE);
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
void course(char motor_state_main, char motor_state_side) {
  int i = motor_speed;

  switch(motor_state_main){
    case 'X':
      movement(0,0,0,0); break;
    case 'F':
      movement(+i,+i,+i,+i); break;
    case 'B':
      movement(-i,-i,-i,-i); break;
    case 'R':
      switch(motor_state_side){
        case 'X':
          movement(+i,+i,-i,-i); break;
        case 'F':
          movement(+i,0,0,+i); break;
        case 'B':
          movement(0,-i,-i,0); break;
        case 'O':
          movement(+i,-i,-i,+i); break;
      } break;
    case 'L':
      switch(motor_state_side){
        case 'X':
          movement(-i,-i,+i,+i); break;
        case 'F':
          movement(0,+i,+i,0); break;
        case 'B':
          movement(-i,0,0,-i); break;
        case 'O':
          movement(-i,+i,+i,-i); break;
      } break;
  }
}
/****************************************************/


void setup() {
  Serial.begin(115200);     //I2C: board (to laptop)
  Wire.begin();             //I2C: board (to Huskeylens)
  huskylens.begin(Wire);    //I2C: Huskeylens (to board)
  huskylens.writeAlgorithm(ALGORITHM_OBJECT_TRACKING); //set to feature

  xBorder0 = ( 320 - center_size )/2;
  xBorder1 = ( 320 + center_size )/2;
  yBorder0 = ( 240 - center_size )/2;
  yBorder1 = ( 240 + center_size )/2;
}

void loop() {
  HUSKYLENSResult result = huskylens.read();

  if(huskylens.request() != 0){
    getxCenter = result.xCenter;
    getyCenter = result.yCenter;

    if( getxCenter < xBorder0 ){xStatus = 0;}
    else if( xBorder0 <= getxCenter && getxCenter <= xBorder1 ){xStatus = 1;}
    else if( xBorder1 < getxCenter ){xStatus = 2;}

    if( getyCenter < yBorder0){yStatus = 0;}
    else if( yBorder0 <= getyCenter && getyCenter <= yBorder1 ){yStatus = 1;}
    else if( yBorder1 < getyCenter ){yStatus = 2;}

    if (xStatus == 1 && yStatus == 1) {
      Serial.println("Course: X, X");
      course('X','X');
    } else if (xStatus == 0) {
      if      (yStatus == 0) { Serial.println("Course: L, F"); course('L','F'); }
      else if (yStatus == 1) { Serial.println("Course: L, X"); course('L','X'); }
      else if (yStatus == 2) { Serial.println("Course: L, B"); course('L','B'); }
    } else if (xStatus == 2) {
      if      (yStatus == 0) { Serial.println("Course: R, F"); course('R','F'); }
      else if (yStatus == 1) { Serial.println("Course: R, X"); course('R','X'); }
      else if (yStatus == 2) { Serial.println("Course: R, B"); course('R','B'); }
    }
  }else{
      course('X','X');
  }

  #if(DBG==1)
    Serial.print("X: ");
    Serial.print(result.xCenter);
    Serial.print("   Y: ");
    Serial.print(result.yCenter);
    Serial.print("   Width: ");
    Serial.print(result.width);
    Serial.print("   Height: ");
    Serial.print(result.height);
    Serial.print("\n");
  #endif

  delay(200);
}
