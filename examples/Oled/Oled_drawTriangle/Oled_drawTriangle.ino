#include <PeanutKingArduinoShield.h>

PeanutKingArduinoShield robot;

void setup() {
}

void oledDraw(){                                // declare update function for oled screen content update
                                                // the function is need to accept void parameter and return void

  robot.oled.drawTriangle( 14,9, 45,32, 9,42 ); // draw triangular box with 3 set of coordinates which
                                                // representing 3 edges of triangle
                                                // in this case,
                                                // the triangle will have point 0 at (14,9)
                                                // point 1 at (45,32)
                                                // point 2 at (9,42)

} 
 
void loop() { 
     
  robot.oledUpdate(oledDraw);                   // pass the update function to adapter for oled update
} 