#include <PeanutKingArduinoShield.h>

// color sensor example

PeanutKingArduinoShield robot;

color_t colorMid[8];
hsl_t hsl = {0, 0, 0};
rgb_t rgb = {0, 0, 0};
rgbc_t rgbc;

void setup(void) {
  pinMode(2, OUTPUT);
  digitalWrite(2, HIGH);
  Serial.begin(115200);  // start serial for output
}

void loop(void) {
  getcolorreading();
  delay(500);
}

void getcolorreading(void) {
  for (int i = 1; i < 9; i++) {
    rgbc = robot.readrgb(i);
    delay(10);
    
    Serial.println(i); 
    Serial.print(", rgbc: ");
    Serial.print(rgbc.r);
    Serial.print(" ");
    Serial.print(rgbc.g);
    Serial.print(" ");
    Serial.print(rgbc.b);
    Serial.println(" ");
    Serial.print(rgbc.c);
    Serial.println(" ");

    hsl = robot.readhsl(i);
    Serial.print("hsl: ");
    Serial.print(hsl.h);
    Serial.print(" ");
    Serial.print(hsl.s);
    Serial.print(" ");
    Serial.println(hsl.l);

    colorMid[i] = readAdvColor(hsl);
    printcolor(colorMid[i]);

//    rgb2hsl(&rgbc, &hsl, &rgb);
    delay(100);
  }
}

void printcolor(uint8_t x) {
  switch (x) {
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

color_t readAdvColor(hsl_t hsl) {
  if      ( hsl.l < 80 && hsl.s < 60  ) return black;
  else if ( hsl.h < 80 && hsl.h > 50)   return yellow;
  else if ( hsl.h > 150 && hsl.s < 30 && hsl.l > 60 )  return white;
  else if ( hsl.h < 15 || hsl.h > 315 ) return red;
  else if ( hsl.h < 150 )               return green;
  else                                  return blue;
}
