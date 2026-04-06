#include "Math.h"

void setup() {
  Serial.begin(9600);
}

void loop() {
  float angle = 0;
  float sineData;
  float cosineData;
  float sum;

  for(angle = 0.0; angle <= 90; angle += 0.1) {
    sineData = sin(angle);
    cosineData = cos(angle);
    sum = sineData + cosineData;
    Serial.print(sineData);
    Serial.print(" ");
    Serial.print(cosineData);
    Serial.print(" ");
    Serial.print(sum);
    Serial.println();
    delay(1);
  }
}
