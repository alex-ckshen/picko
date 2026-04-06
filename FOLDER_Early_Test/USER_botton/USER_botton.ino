#include "Arduino.h" 
 
const int led = 7; // USR LED pin is P7 
const int usr_btn = 6; // USR BTN pin is P6 
int val = 0; // Variable that stores LED state. 
 
void pin_change(void) 
{ 
    digitalWrite(led, val); 
    val = !val; 
    Serial.println("button pressed"); 
} 
 
void setup() { 
    pinMode(led, OUTPUT); 
    attachInterrupt(usr_btn, pin_change, RISING); 
    Serial.begin(9600); 
} 
 
void loop() { 
    delay(1000); 
}
