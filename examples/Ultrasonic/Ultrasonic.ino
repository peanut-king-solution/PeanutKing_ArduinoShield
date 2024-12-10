#include <PeanutKingArduinoShield.h>
PeanutKingArduinoShield robot;

void setup() {
  Serial.begin(9600);                                      // start serial for output
}

void loop() {
  Serial.print(" ultrasonic: "); 
  Serial.print(robot.getUltrasonic(13));                   // get value of Ultrasonic with trig pin 13
  Serial.print(" ultrasonic: ");                  
  Serial.print(robot.getUltrasonic(8));                    // get value of Ultrasonic with trig pin 8       
  Serial.print(" ultrasonic: ");                  
  Serial.print(robot.getUltrasonic(5));                    // get value of Ultrasonic with trig pin 7
  Serial.print(" ");                                       // 
  Serial.println();                                        // next line
   
  delay(500);                                              // delay 500ms
}
