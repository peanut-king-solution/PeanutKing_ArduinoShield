/*
  "Hello World!" example code.
*/

#include "oledScreen.h"

OLEDLIB_SSD1306_128X64 oled(OLED_I2C_OPT_NONE|OLED_I2C_OPT_DEV_0);	// I2C / TWI 

void draw(void) {
  // graphic commands to redraw the complete screen should be placed here  
  oled.setFont(oled_font_unifont);
  //oled.setFont(oled_font_osb21);
  oled.drawStr( 0, 22, "Hello World!");
}

void setup(void) {
  // flip screen, if required
  // oled.setRot180();
  
  // set SPI backup if required
  //oled.setHardwareBackup(oled_backup_avr_spi);

  // assign default color value
  if ( oled.getMode() == OLED_MODE_R3G3B2 ) {
    oled.setColorIndex(255);     // white
  } else
  if ( oled.getMode() == OLED_MODE_GRAY2BIT ) {
    oled.setColorIndex(3);         // max intensity
  } else
  if ( oled.getMode() == OLED_MODE_BW ) {
    oled.setColorIndex(1);         // pixel on
  } else
  if ( oled.getMode() == OLED_MODE_HICOLOR ) {
    oled.setHiColorByRGB(255,255,255);
  }
  
  pinMode(8, OUTPUT);
}

void loop(void) {
  // picture loop
  oled.firstPage();  
  do {
    draw();
  } while( oled.nextPage() );
  
  // rebuild the picture after some delay
  //delay(50);
}
