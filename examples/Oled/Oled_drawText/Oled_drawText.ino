#include <PeanutKingArduinoShield.h>

PeanutKingArduinoShield robot;

int x =0;

void setup() {
}

void oledDraw(){                              //declare update function for oled screen content update
                                              //the function is need to accept void parameter and return void
                                              
  robot.oled.setFont(u8g_font_unifont);       //set string font
  robot.oled.drawStr(0,12,"Hello World!");    //print "Hello World!" at (x=0,y=12)
  robot.oled.setPrintPos(0, 24);              //set next print position to (x=0,y=24)
  robot.oled.print("X is ");                  //print "X is" at current position
  robot.oled.print(x);                        //print integer x
}

void loop() {
  robot.oledUpdate(oledDraw);                 //pass the update function to adapter for oled update
}