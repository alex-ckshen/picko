#define DBG_HUS
#define PRT_HUSKY
#define PIN_BUTTON 17    //按鍵接腳
#define PIN_LED    13    //LED接腳
#define PIN_SERVO  10    //伺服接腳
#define SERVO_DOWN 8     //伺服最低角度
#define SERVO_UP   40    //伺服最高角度

#include <AFMotor.h>
#include <Servo.h>
#include "HUSKYLENS.h"

//HUSKYLENS I2C green line >> SDA; blue line >> SCL
HUSKYLENS  huskylens;    //huskylens
HUSKYLENSResult result;  //宣告huskylens結果變數
int husky_state;         //紀錄huskylens狀態
int last_xCenter;
int last_yCenter;
int last_width;
int last_height;
int curr_xCenter;
int curr_yCenter;
int curr_width;
int curr_height;

// AF_DCMotor 變數名稱(第幾個馬達控制, 頻率)
// Motor1 & Motor2  有這幾個頻率:
// MOTOR12_64KHZ, MOTOR12_8KHZ, MOTOR12_2KHZ, MOTOR12_1KHZ
// Motor3 & Motor4  有這幾個頻率:
// MOTOR12_64KHZ, MOTOR12_8KHZ, MOTOR12_1KHZ
//
AF_DCMotor motor4_left(4, MOTOR12_64KHZ);
AF_DCMotor motor3_right(3, MOTOR12_64KHZ);
Servo      pick_servo;   //宣告伺服

const int motor_speed = 160;
boolean   picko_start;
boolean   motor_kickoff;
int       buttonState;   //按鈕的狀態

//馬達控制副程式
void motor_control( int left_speed, int right_speed ) {  
  if( left_speed > 0 ) {                 //左輪設定是正值就前進
    motor4_left.setSpeed( left_speed );
    motor4_left.run(FORWARD);            //設定為前進
  } else if ( left_speed < 0 ) {         //左輪設定是負值就後退
    motor4_left.setSpeed( -left_speed ); //設定值是負值, 加負號變為正值
    motor4_left.run(BACKWARD);           //設定為後退
  } else {                               //左輪設定是零就停止
    motor4_left.run(RELEASE);
  }
    
  if( right_speed > 0 ) {
    motor3_right.setSpeed(right_speed);
    motor3_right.run(FORWARD);
  } else if ( right_speed < 0 ) {
    motor3_right.setSpeed(-right_speed);
    motor3_right.run(BACKWARD);
  } else {
    motor3_right.run(RELEASE);
  }
}

//沒有找到物體時, 變數都設成-1以便知道沒找到球要轉圈圈找球
void no_object() {   
   curr_xCenter = -1;
   curr_yCenter = -1;
   curr_width   = -1;
   curr_height  = -1;
}

//只列印有變化的位置以便觀察
void print_position() {
  #ifdef DBG_HUS
  if ( (last_xCenter != curr_xCenter) || (last_yCenter != curr_yCenter) ) {
    Serial.println(String()+F("last X= ")+last_xCenter
                           +F(", last Y= ")+last_yCenter
                           +F(", X= ")+curr_xCenter
                           +F(", Y= ")+curr_yCenter );
  }
  #endif
      
  #ifdef PRT_HUSKY
  if ( (last_xCenter != curr_xCenter) || (last_yCenter != curr_yCenter) ) {
//    huskylens.clearCustomText(); //Remove all custom texts
    if ( curr_xCenter != -1) {
      huskylens.customText(String(curr_xCenter), 120,  10);   //Add first custom text at (120,120)
      huskylens.customText(String(curr_yCenter), 170,  10);  //Add second custom text at (50,200)
    } else {
      huskylens.customText("?", 120,  10);   //Add first custom text at (120,120)
      huskylens.customText("?", 170,  10);  //Add second custom text at (50,200)
    }
  }
  #endif
}
                     
void setup() {
  Serial.begin(115200); //uart baud rate, 沒看到訊息要先檢查
  Wire.begin(); //用I2C連線huskylens
  while (!huskylens.begin(Wire)) { //檢查連線是否成功? 不成功才會列印以下訊息
    Serial.println(F("Begin failed!"));
    Serial.println(F("1.Please recheck the \"Protocol Type\" in HUSKYLENS (General Settings>>Protocol Type>>I2C)"));
    Serial.println(F("2.Please recheck the connection."));
    delay(100);
  }
    
  huskylens.clearCustomText(); //Remove all custom texts
  huskylens.writeAlgorithm(ALGORITHM_OBJECT_TRACKING); //Switch the algorithm to object tracking.
  husky_state = 0;                   //husky的狀態
  no_object();                       //沒找到物體之前位置都設為-1
  last_xCenter = -1;
  last_yCenter = -1;
  last_width   = -1;
  last_height  = -1;
   
  pick_servo.attach(PIN_SERVO);      //attaches the servo pin to the servo object
  pick_servo.write(SERVO_UP);        //伺服起始位置
  motor_kickoff = false;
  
  pinMode(PIN_LED, OUTPUT);          //設定LED的PIN腳為輸出
  digitalWrite(PIN_LED, LOW);        //初始時關掉LED
  
  pinMode(PIN_BUTTON, INPUT_PULLUP); //設定按鈕的接腳為輸入，PULLUP指的是沒有接輸入會是HIGH
  buttonState = HIGH;                //按鈕初始是HIGH
  picko_start = false;                //初始時車子不動
  
}

void loop() {
  if ( buttonState && digitalRead(PIN_BUTTON) == LOW )  {  //按下按鈕時輸入會跟GND連通, 所以讀回LOW
    buttonState = LOW;    //記住有按過按鈕
    if ( !picko_start ) {  //按奇數次時(例如第一次)會啟動
      picko_start = true;
      digitalWrite(PIN_LED, HIGH);
      pick_servo.write(SERVO_DOWN);
      delay(1000);
      pick_servo.write(SERVO_UP);
      Serial.println(String()+F("Pico start"));
    } else {              //按偶數次時(例如第二次)會停止
      picko_start = false;
      digitalWrite(PIN_LED, LOW);
      Serial.println(String()+F("Pico stop"));
    }
  } else if ( !buttonState && digitalRead(PIN_BUTTON) == HIGH ) { //按鈕放開時輸入會跟5V連通, 所以讀回LOW
    buttonState = HIGH; //按鈕被放開
  }
   
  last_xCenter = curr_xCenter;//記住上次資訊
  last_yCenter = curr_yCenter;
  last_width   = curr_width;
  last_height  = curr_height;

  if ( !huskylens.request() ) {
      no_object();                //沒找到物體之前位置都設為-1
      #ifdef DBG_HUS              //只有打開DBG_HUS define時會列印訊息
        if ( husky_state != 1 ) { //husky的狀態改變才列印訊息
          Serial.println( F("Fail to request objects from HUSKYLENS!") );
          husky_state = 1;        //記住這次husky的狀態是1
        }
      #endif
  } else if ( !huskylens.isLearned() ) {
      no_object();
      #ifdef DBG_HUS
        if ( husky_state != 2 ) {
          Serial.println( F("Object not learned!") );
          husky_state = 2;
        }  
      #endif
  } else if ( !huskylens.available() ) {
      no_object();
      #ifdef DBG_HUS
        if ( husky_state != 3 ) {
          Serial.println( F("Object disappeared!") );
          husky_state = 3;
        }  
      #endif
  } else {
      result = huskylens.read();      //用副程式讀回位置資訊. 用以下變數記住, 方便沒找到時修改成-1 (不要直接修改result.xCenter等變數)
      curr_xCenter = result.xCenter;  //物體橫向中心位置. 左上角x=0, 最右邊為320
      curr_yCenter = result.yCenter;  //物體直向中心位置. 左上角y=0, 最下面為240
      curr_width   = result.width;    //物體寬度, 目前程式沒使用
      curr_height  = result.height;   //物體高度, 目前程式沒使用
      husky_state  = 5;               //記住這次husky的狀態是5
  }
    
  print_position(); //列印有變化的位置以便觀察
  
  if ( !picko_start ) {    //初始狀態或是偶數次按鈕會停止動作
    motor_control( 0, 0); //停止馬達
    motor_kickoff = false; //馬達沒有kickoff
    
  } else {                                                    //picko_start是true時會開始動作
    //algorithm: 有到到球先轉彎對準再前進
    if ( (curr_xCenter == -1) && (curr_yCenter == -1) ) {     //現在沒看到球
      if ( (last_xCenter == -1) && (last_yCenter == -1) ) {   //之前也沒看到球
        if ( !motor_kickoff ) {
          motor_control( motor_speed, motor_speed);      //左輪正值前進, 後輪負值後退, 轉圈圈找球
          delay(2000);                                        //啟動1秒
          motor_control( motor_speed - 20, 20 - motor_speed);                         //啟動後減速
        }
      } else {                                                //之前往前走時有看到球 
        delay(1000);                                          //前進1秒
        motor_control( 0, 0);                                //停止馬達
        pick_servo.write(SERVO_DOWN);
        delay(1000);
        pick_servo.write(SERVO_UP);
        motor_kickoff = false;
      }
    } else if ( (curr_xCenter > 0) && (curr_xCenter < 140) ) {      //球在左邊
      motor_kickoff = false;
      if ( (curr_yCenter > 0) && (curr_yCenter < 140) ) {           //球在左上要左轉
        motor_control( 0, motor_speed);
      } else if ( (curr_yCenter >= 140) && (curr_yCenter < 240) ) { //球在左下要左退    
        motor_control( 0 - motor_speed, 0);  
      }      
    } else if ( (curr_xCenter > 180) && (curr_xCenter < 320) ) {    //球在右邊
      motor_kickoff = false;
      if ( (curr_yCenter > 0) && (curr_yCenter < 140) ) {           //球在右上要右轉
        motor_control( motor_speed, 0);
      } else if ( (curr_yCenter >= 140) && (curr_yCenter < 240) ) { //球在右下要右退    
        motor_control(0, 0 - motor_speed); 
      }  
    } else if ( (curr_xCenter >= 140) && (curr_xCenter <= 180) ) {  //球在中間要直走到看不見
      motor_kickoff = false;
      motor_control( motor_speed, motor_speed);  
    }
  }
}
