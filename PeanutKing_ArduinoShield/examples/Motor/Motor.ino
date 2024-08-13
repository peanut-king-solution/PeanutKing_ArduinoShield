#include <PeanutKingArduinoShield.h>
PeanutKingArduinoShield robot;

void setup() {
  Serial.begin(9600);
}

void loop(){
  robot.setMotor(100,150);    // set left motor as 100 and right motor as 150 
                              // both moves forward
  delay(1500);                // delay 1500ms
  robot.setMotor(-100,-150);  // set left motor as -100 which moves backward,
                              // set right motor as 100 which moves forward
  delay(1500);                // delay 1500ms

}