/***************** Motor Control v0 *****************
  Release Notes:
  - First iteration of motor control
  - Motor States:
    0 - stop
    1 - forward
    2 - backwards
    3 - slide right
    4 - slide left
    5 - right up
    6 - left up
    7 - left down
    8 - right down
    9 - turn right
    10 - turn left
****************************************************/

#include <AFMotor.h>

AF_DCMotor back_right (1, MOTOR12_64KHZ);
AF_DCMotor back_left  (2, MOTOR12_64KHZ);
AF_DCMotor front_right(3, MOTOR12_64KHZ);
AF_DCMotor front_left (4, MOTOR12_64KHZ);

const int motor_speed = 180;
boolean   picko_start;
boolean   motor_kickoff;
int       button_state = 1;

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
void setup(){
  }
void loop(){
}
