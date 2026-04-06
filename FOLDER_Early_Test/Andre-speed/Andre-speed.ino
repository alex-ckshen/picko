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
#define BUTTON_PIN      6
//#define LED_BUILTIN   7
#define SERVO_PIN       9
#define S_UART_RX       2
#define S_UART_TX       3

#define P_EINT0         0

//-
unsigned int left_count=0;
unsigned int right_count=0;
unsigned long start_time;
unsigned long stop_time;
unsigned long last_time;
unsigned int left_rpm;
unsigned int right_rpm;
unsigned int left_value;    
unsigned int right_value;      
unsigned int grid_num = 20;

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
  left_value = 140;
  right_value = 140;
}

void loop() {
 analogWrite(13, 0); //left  INB -
 analogWrite(17, left_value);           //left  INA +
 analogWrite(10, 0);  //right INA +
 analogWrite(12, right_value);  //right INB -

 if (millis() - start_time >= 1000){   /* 每秒更新 */
   // 計算 rpm 時，停止計數
   detachInterrupt( 2 );
   detachInterrupt( 3 );
   stop_time = millis();

   // (偵測的格數count / 一圈網格數20) / (時間差/1000)  
   left_rpm = left_count * (60 * 1000 / grid_num ) / (stop_time - start_time);
   right_rpm = right_count * (60 * 1000 / grid_num ) / (stop_time - start_time);
/*
   if ( left_rpm > 72 && left_value > 10 )       left_value = left_value - 3;
   else if ( left_rpm < 70 && left_value < 200)  left_value = left_value + 3;
   else                                          left_value = left_value;
   
   if ( right_rpm > 72 && right_value > 50 )       right_value = right_value - 3;
   else if ( right_rpm < 70 && right_value < 200)  right_value = right_value + 3;
   else                                            right_value = right_value;
*/
   // 輸出至Console
   Serial.print("left RPM = ");
   Serial.println(left_rpm, DEC);
   
   Serial.print("left_motor = ");
   Serial.println(left_value, DEC);

   Serial.print("right RPM = ");
   Serial.println(right_rpm, DEC);
   
   Serial.print("right_motor = ");
   Serial.println(right_value, DEC);
   
   //Restart the interrupt processing
   left_count = 0;
   right_count = 0;
   attachInterrupt( 2, left_ISR_counter,  FALLING);
   attachInterrupt( 3, right_ISR_counter, FALLING);
   start_time = millis();
 }
}
