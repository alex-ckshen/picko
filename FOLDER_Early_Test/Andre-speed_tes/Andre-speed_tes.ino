/*

*/
//#include <HUSKYLENS.h>
//#include <SoftwareSerial.h>
#include <LRemote.h>
#include <Servo.h>

#define Right_Wheel_A   12
#define Right_Wheel_B   10
#define Left_Wheel_A    17
#define Left_Wheel_B    13
#define BUZZER_PIN      14
//#define BUTTON_PIN      6
#define PIN_USER_BTN    6
//#define LED_BUILTIN   7
//-

unsigned int left_count=0;
unsigned int right_count=0;
unsigned long start_time;
unsigned long stop_time;
unsigned long last_time;
unsigned int left_rpm;
unsigned int right_rpm;
unsigned int grid_num = 20;
unsigned int pwm_value;
//boolean button_status;

void left_ISR_counter() {
   left_count++;
}

void right_ISR_counter() {
   right_count++;
}

void setup() {
  Serial.begin(9600);
   
  pinMode(10, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(17, OUTPUT);
  
//pinMode(2, INPUT_PULLUP);
  attachInterrupt( 2, left_ISR_counter,  FALLING);
  attachInterrupt( 3, right_ISR_counter, FALLING);
  left_count = 0;
  right_count = 0;
  left_rpm = 0;
  right_rpm = 0;
  start_time = 0;
  stop_time = 0;
  last_time = 0;
  pwm_value = 60;
//button_status = false;
}

void loop() {
  analogWrite(13, 0);            //left  INB -
  analogWrite(17, pwm_value);    //left  INA +
  analogWrite(10, 0);            //right INA +
  analogWrite(12, pwm_value);    //right INB -

  if ( millis() - start_time >= 2000 ) {   /* 每秒更新 */

    // 計算 rpm 時，停止計數
    detachInterrupt( 2 );
    detachInterrupt( 3 );
    stop_time = millis();

    // (偵測的格數count / 一圈網格數20) / (時間差/1000)  
    left_rpm = left_count * (60 * 1000 / grid_num ) / (stop_time - start_time);
    right_rpm = right_count * (60 * 1000 / grid_num ) / (stop_time - start_time);

    // 輸出至Console
    Serial.print("\n pwm_value = ");
    Serial.print(pwm_value, DEC);

    Serial.print(";left RPM = ");
    Serial.print(left_rpm, DEC);
   
    Serial.print(";right RPM = ");
    Serial.print(right_rpm, DEC);
   
    //Restart the interrupt processing
    left_count = 0;
    right_count = 0;
    attachInterrupt( 2, left_ISR_counter,  FALLING);
    attachInterrupt( 3, right_ISR_counter, FALLING);
    start_time = millis();
    
    if ( pwm_value < 200) pwm_value = pwm_value + 1;
    else                  pwm_value = pwm_value;
  }
}
