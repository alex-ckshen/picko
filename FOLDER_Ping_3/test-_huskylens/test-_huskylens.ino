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

#include <HUSKYLENS.h>
#include <Wire.h>

HUSKYLENS huskylens;

void setup() {
    Serial.begin(115200);
    Wire.begin();
    
    while (!huskylens.begin(Wire)) {
        Serial.println(F("No connection"));
        delay(100);
    }
    
    huskylens.writeAlgorithm(ALGORITHM_OBJECT_TRACKING);
    Serial.println(F("HuskyLens Ready - Object Tracking Started"));
    //Serial.println(F("Learn an object on the HuskyLens first!"));
}

void loop() {
    if (!huskylens.request()) {
        Serial.println(F("Request failed"));
    }
    else if (!huskylens.isLearned()) {
        Serial.println(F("No object learned yet"));
    }
    else if (!huskylens.available()) {
        Serial.println(F("No object detected"));
    }
    else {
        HUSKYLENSResult result = huskylens.read();
        
        // === MAIN XY COORDINATES ===
        Serial.print("X: ");
        Serial.print(result.xCenter);
        Serial.print("   Y: ");
        Serial.print(result.yCenter);
        Serial.print("   Width: ");
        Serial.print(result.width);
        Serial.print("   Height: ");
        Serial.print(result.height);
        Serial.print("\n");
/*        
        // Optional: Show which zone the object is in
        if (result.xCenter < 120) {
            Serial.println("→ Object is on the LEFT");
        } else if (result.xCenter > 200) {
            Serial.println("→ Object is on the RIGHT");
        } else {
            Serial.println("→ Object is in the CENTER");
        }
*/
    }
    
    delay(100);   // Adjust this (50 = faster, 200 = slower)
}