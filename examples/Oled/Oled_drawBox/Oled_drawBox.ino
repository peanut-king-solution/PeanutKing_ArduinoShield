#include <PeanutKingArduinoShield.h>

PeanutKingArduinoShield robot;

void setup() {
}

void oledDraw(){                            // declare update function for oled screen content update
                                            // the function is need to accept void parameter and return void

  robot.oled.drawBox(10,12,30,20);          // draw rectangular box with 
                                            // upper left edge at coordinates:(x=10,y=12)
                                            // with width 30 and height 20 pixels
} 
 
void loop() {  
  robot.oledUpdate(oledDraw);               // pass the update function to adapter for oled update
} 