/* Calibration: Set Front Left speed as default, lookup 100 110 120 130 140 150.

  State:
    0 -> idle
    1 -> check min speed
    2 -> 100
    3
    4    ...
    5
    6 -> 150
*/
#include <AFMotor.h>
AF_DCMotor back_right (1, MOTOR12_64KHZ);
AF_DCMotor back_left  (2, MOTOR12_64KHZ);
AF_DCMotor front_right(3, MOTOR12_64KHZ);
AF_DCMotor front_left (4, MOTOR12_64KHZ);

// 2 | 1
//-------  motor labeling
// 3 | 4

void motor(int motor_no, int motor_speed){
  if(abs(motor_speed)< 230 ){                       //set speed limit as 230
    switch(motor_no){
      case 1:
        if( motor_speed > 0 ) {
          front_right.setSpeed( motor_speed );
          front_right.run(FORWARD);
        } else if ( motor_speed < 0 ) {
          front_right.setSpeed( -motor_speed );
          front_right.run(BACKWARD);
        } else {
          front_right.run(RELEASE);
        }
        break;
      case 2:
        if( motor_speed > 0 ) {
          front_left.setSpeed( motor_speed );
          front_left.run(FORWARD);
        } else if ( motor_speed < 0 ) {
          front_left.setSpeed( -motor_speed );
          front_left.run(BACKWARD);
        } else {
          front_left.run(RELEASE);
        }
        break;
      case 3:
        if( motor_speed > 0 ) {
          back_left.setSpeed( motor_speed );
          back_left.run(FORWARD);
        } else if ( motor_speed < 0 ) {
          back_left.setSpeed( -motor_speed );
          back_left.run(BACKWARD);
        } else {
          back_left.run(RELEASE);
        }
        break;
      case 4:
          if( motor_speed > 0 ) {
          back_right.setSpeed( motor_speed );
          back_right.run(FORWARD);
        } else if ( motor_speed < 0 ) {
          back_right.setSpeed( -motor_speed );
          back_right.run(BACKWARD);
        } else {
          back_right.run(RELEASE);
        }
        break;
      default:
        back_right.run(RELEASE);
        back_left.run(RELEASE);
        front_right.run(RELEASE);
        front_left.run(RELEASE);
        break;
    }
  }
}

//====================================================================================================================
int set_state = 0;
int state(){
  Serial.print("Next Step?");
  if (Serial.available() > 0) {
    int input = Serial.read();
    if (input != '\n' && input != '\r') {
      set_state++;
      Serial.print("\n");
    }
  }
  return set_state;
}

int get_input(){
  int return_value;
  if (Serial.available() > 0) {
    int input = Serial.read();
    if (input != '\n' && input != '\r') {
      return_value = input;
    }
  }
  return return_value;
}
//====================================================================================================================

int speed;
int motor_position;
char motor_no = 1;

void setup() {
  Serial.begin(9600);
  Serial.print("Start Calibration!\n");
}

void loop() {
  switch(state()){
    case 0:
      break;
    case 1:
      Serial.print("If movement detected, Press 1\n");
      speed = 80;
      while(get_input() != 1){
        motor(motor_no,speed);
        Serial.print(speed);Serial.print("\r");
        speed++;
        delay(500);
      }
      break;
  }
}
