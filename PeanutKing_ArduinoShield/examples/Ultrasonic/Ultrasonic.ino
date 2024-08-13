#include <PeanutKingArduinoShield.h>
PeanutKingArduinoShield robot; 
Ultrasonic usA=Ultrasonic(13,12);                          // ultrasonic sensor with Trig pin 13 & echo pin 12
Ultrasonic usB=Ultrasonic(8,7);                            // ultrasonic sensor with Trig pin 8 & echo pin 7
Ultrasonic usC=Ultrasonic(5,4);                            // ultrasonic sensor with Trig pin 5 & echo pin 4
 
void setup() { 
  Serial.begin(9600);                                      //  start serial for output
} 
 
void loop() { 
  Serial.print(" ultrasonic: "); Serial.print(usA.get());  // get value of Ultrasonic A
  Serial.print(" ultrasonic: "); Serial.print(usB.get());  // get value of Ultrasonic B         
  Serial.print(" ultrasonic: "); Serial.print(usC.get());  // get value of Ultrasonic C
  Serial.print(" ");                                       // 
  Serial.println();                                        // next line
   
  delay(500);                                              // delay 500ms
}

//#include <PeanutKingArduinoShield.h>
//PeanutKingArduinoShield robot;
//
//void setup() {
//  Serial.begin(9600);                                      // start serial for output
//}
//
//void loop() {
//  Serial.print(" ultrasonic: "); 
//  Serial.print(robot.getUltrasonic(13));                   // get value of Ultrasonic with trig pin 13
//  Serial.print(" ultrasonic: ");                  
//  Serial.print(robot.getUltrasonic(8));                    // get value of Ultrasonic with trig pin 8       
//  Serial.print(" ultrasonic: ");                  
//  Serial.print(robot.getUltrasonic(5));                    // get value of Ultrasonic with trig pin 7
//  Serial.print(" ");                                       // 
//  Serial.println();                                        // next line
//   
//  delay(500);                                              // delay 500ms
//}

