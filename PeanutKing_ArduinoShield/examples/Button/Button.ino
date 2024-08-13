#include <PeanutKingArduinoShield.h>
PeanutKingArduinoShield robot;
void setup() {
  Serial.begin(9600);  // start serial for output
}
void loop() {
  Serial.print("  Button A: ");   Serial.print(robot.getButtonA());   // get value of Button A
  Serial.print("  Button B: ");   Serial.print(robot.getButtonB());   // get value of Button B         
  Serial.print("  Button C: ");   Serial.print(robot.getButtonC());   // get value of Button C
  Serial.print(" ");                                                  // 
  Serial.println();                                                   // next line
   
  delay(500);                                                         // delay 500ms
}