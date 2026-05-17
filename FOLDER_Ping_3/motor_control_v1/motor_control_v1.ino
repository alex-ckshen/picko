/***************** Motor Control v1 *****************
  Release Notes:
  - Made controls more intuitive and simple.
  - Used FBRL characters to replace int
  - Use different combinations to control direction
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

  Upcoming Improvements:
  - Change to intuitive motor sequence and labels.
  - Motor kick start addition.
  - Independent motor speed controls.
  - Dynamic speed changes based on calibration.
****************************************************/


#include <AFMotor.h>

#define motor_speed 100

//====================================================================================================================
AF_DCMotor back_right (1, MOTOR12_64KHZ);
AF_DCMotor back_left  (2, MOTOR12_64KHZ);
AF_DCMotor front_right(3, MOTOR12_64KHZ);
AF_DCMotor front_left (4, MOTOR12_64KHZ);

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
//====================================================================================================================
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
//====================================================================================================================

void setup(){

}

void loop(){

}
