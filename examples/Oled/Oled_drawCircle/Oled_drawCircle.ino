#include <PeanutKingArduinoShield.h>

PeanutKingArduinoShield robot;

void setup() {
}

void oledDraw(){                            //declare update function for oled screen content update
                                            //the function is need to accept void parameter and return void
                                            
  robot.oled.drawCircle(64,32,14);          //draw Cicle with radius 14 pixels and
                                            //center at coordinates:(x=64,y=32)
}

void loop() {
  robot.oledUpdate(oledDraw);               //pass the update function to adapter for oled update
}