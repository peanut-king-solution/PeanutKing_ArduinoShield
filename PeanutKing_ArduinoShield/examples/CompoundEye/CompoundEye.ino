#include <PeanutKingArduinoShield.h>
PeanutKingArduinoShield robot;
int value=0;
int BALL_Angle=0;

void setup() {
  Serial.begin(9600); // start serial for output
}

void loop() {

  robot.compoundEyeReadAll();               // this will serial print all value from Compound Eye, only for debugging

  value = robot.compoundEyeRead(0);         // get the value of eye no 1
  value = robot.compoundEyeRead(1);         // get the value of eye no 2
                                            // ...
  value = robot.compoundEyeRead(11);        // get the value of eye no 12

  value = robot.compoundEyeRead(12);        // get the value of the max eye
  value = robot.compoundEyeRead(13);        // get the no. of the max eye

                                            // get and serial print specified Compound Eye data
  BALL_Angle = robot.compoundEyeRead(14);   // get the angle of the ball relative to Compound Eye,
                                            // the front is 0, counting anticlockwise,
                                            // 0-180 for Single mode Compound Eye,
                                            // 0-359 for Double mode Compound Eye

  Serial.println(BALL_Angle);               // if you get -1 from compoundEyeRead, means the value is Not a Number(NaN),
                                            // In this case, meaning no ball detected anywhere.

  robot.compoundEyeRead(15);                // get the mode of Compound Eye: Single mode:0, Double mode:1
  
}
