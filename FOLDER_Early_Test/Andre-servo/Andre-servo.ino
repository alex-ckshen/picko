#include <Servo.h>
#define PIN_USER_BTN    6
Servo   myservo;  
boolean button_status;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Start configuring remote");
  pinMode(PIN_USER_BTN, INPUT);
  button_status = false;
  myservo.attach(5);  // attaches the servo on pin 9 to the servo object

}

void loop() {
  // put your main code here, to run repeatedly:
  if ( !button_status && digitalRead(PIN_USER_BTN) == HIGH )  {
    button_status = true;
    myservo.write(180);
    delay(1000);
    myservo.write(150);
    delay(1000);
  } else if ( button_status && digitalRead(PIN_USER_BTN) == HIGH ) {
    button_status = false;
  }
}
