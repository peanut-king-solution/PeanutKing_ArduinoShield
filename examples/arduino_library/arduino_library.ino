#include <PeanutKingArduinoShield.h>



// Wire Master Reader
// by Nicholas Zambetti <http://www.zambetti.com>

// Demonstrates use of the Wire library
// Reads data from an I2C/TWI slave device
// Refer to the "Wire Slave Sender" example for use with this

// Created 29 March 2006

// This example code is in the public domain.


#include <Wire.h>
PeanutKingArduinoShield robot;
#define max3v(v1, v2, v3)   ((v1)<(v2)? ((v2)<(v3)?(v3):(v2)):((v1)<(v3)?(v3):(v1)))
#define min3v(v1, v2, v3)   ((v1)>(v2)? ((v2)>(v3)?(v3):(v2)):((v1)>(v3)?(v3):(v1)))
color_t colorMid[2];


void setup() {


 pinMode(2, OUTPUT);
 digitalWrite(2, HIGH);
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
}

void loop() {

  // hsl_t hslvalue= robot.readhsl(1);
  getcolorreading();
  printcolor(1);
   printcolor(2);
  delay(500);
}

void getcolorreading(){
    
    for (int i = 6; i < 8; i++) {
    rgbc_t rgbc = robot.readrgb(i);
    hsl_t hsl = {0, 0, 0};
    rgb_t rgb = {0, 0, 0};
    rgb2hsl(&rgbc, &hsl, &rgb);

    colorMid[i] = readAdvColor(hsl);
//    Serial.println(i); 
//       Serial.print("hsl: ");
//    Serial.print(hsl.h);
//    Serial.print(" ");
//    Serial.print(hsl.s);
//    Serial.print(" ");
//    Serial.println(hsl.l);

    switch ( colorMid[i] ) {
      
      case black:   Serial.print("black  ");  break;
      case white:   Serial.print("white  ");  break;
      case grey:    Serial.print("grey   ");  break;
      case red:     Serial.print("red    ");  break;
      case green:   Serial.print("green  ");  break;
      case blue:    Serial.print("blue   ");  break;
      case yellow:  Serial.print("yellow ");  break;
      case cyan:    Serial.print("cyan   ");  break;
      case magenta: Serial.print("magenta");  break;
      default: Serial.print("no color");  break;
    }

  }

  
}

void printcolor(int i){

  
    switch ( colorMid[i] ) {
      
      case black:   Serial.print("black  ");  break;
      case white:   Serial.print("white  ");  break;
      case grey:    Serial.print("grey   ");  break;
      case red:     Serial.print("red    ");  break;
      case green:   Serial.print("green  ");  break;
      case blue:    Serial.print("blue   ");  break;
      case yellow:  Serial.print("yellow ");  break;
      case cyan:    Serial.print("cyan   ");  break;
      case magenta: Serial.print("magenta");  break;
      default: Serial.print("no color");  break;
    }

  
}

void rgb2hsl(rgbc_t *rgbc, hsl_t *hsl, rgb_t *rgb) {
  uint8_t maxVal, minVal, difVal;
  uint8_t r = rgbc->r * 255 / rgbc->c; //[0-255]
  uint8_t g = rgbc->g * 255 / rgbc->c;
  uint8_t b = rgbc->b * 255 / rgbc->c;

  rgb->r = r;
  rgb->g = g;
  rgb->b = b;

  maxVal = max3v(r, g, b);
  minVal = min3v(r, g, b);
  difVal = maxVal - minVal;

  hsl->l = (maxVal + minVal) / 2; // [0-255]

  if (maxVal == minVal) {         // if max is 0, then r = g = b = 0
    hsl->h = 0;                   // s = 0
    hsl->s = 0;
  }
  else {
    if (maxVal == r)    {
      if (g >= b)
        hsl->h = 60 * (g - b) / difVal;
      else
        hsl->h = 60 * (g - b) / difVal + 360;
    }
    else {
      if (maxVal == g)
        hsl->h = 60 * (b - r) / difVal + 120;
      else if (maxVal == b)
        hsl->h = 60 * (r - g) / difVal + 240;
    }

    if (hsl->l <= 50)
      hsl->s = difVal * 100 / (maxVal + minVal); //[0-100]
    else
      hsl->s = difVal * 100 / (200 - (maxVal + minVal));
  }
}
color_t readAdvColor(hsl_t hsl) {



  if ( hsl.l < 80 && hsl.s < 60  )     return black;
  else if ( hsl.h < 80 && hsl.h > 50)                return yellow;
  else if ( hsl.h > 150 && hsl.s < 30 && hsl.l > 60 )  return white;
  else if ( hsl.h < 15 || hsl.h > 315 )  return red;

  else if ( hsl.h < 150 )               return green;
  else                                 return blue;

}
