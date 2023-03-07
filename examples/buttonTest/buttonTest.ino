#include <PeanutKingArduinoShield.h>
PeanutKingArduinoShield robot;
void setup() {
  Serial.begin(115200);  // start serial for output
}
void loop() {
  Serial.print( analogRead(A5) );
  Serial.print("Button A: ");   Serial.print(robot.button.get(0));
  Serial.print("  Button B: ");   Serial.print(robot.button.get(1));
  Serial.print("  Button C: ");   Serial.print(robot.button.get(2));
  Serial.print(" ");
  Serial.println();
  
  delay(500);
}