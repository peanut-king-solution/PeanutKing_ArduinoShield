#include <PeanutKingArduinoShield.h>

PeanutKingArduinoShield robot;

void setup() {
    Serial.begin(9600);
}

void loop() {
    
    int compass_value = robot.compassRead();    // get the angle of the compass
                                                // 0 means pointing to North,
                                                // 0-359 degree in clockwise
                                                // press button can Reset the current direction as the North point
    Serial.print("compass_value: ");
    Serial.println(compass_value);
}