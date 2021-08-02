/*
  
*/
#include "PeanutKingArduinoShield.h"

#define ON 1
#define OFF 0

int8_t TimeDisp[] = {0x00,0x00,0x00,0x00};
unsigned char ClockPoint = 1;
unsigned char Update;
unsigned char halfsecond = 0;
unsigned char second;
unsigned char minute = 0;
unsigned char hour = 12;

SevenSegment tm1637(8,9);     // (CLK, DIO)

OLED_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);  // I2C / TWI 

LedMatrix lc=LedMatrix(3,5,4,1);    

PeanutKingArduinoShield robot;

Button button = Button();
Compass compass = Compass();
Ultrasonic ultrasonic = Ultrasonic(6,7);
colorSensor rgbcolor = colorSensor();
Multiplexer multiplexer = Multiplexer();
Motor motor = Motor();


void setup() {
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(115200);  // start serial for output

  tm1637.set();
  tm1637.init();
  Timer1.initialize(500000);//timing for 500ms
  Timer1.attachInterrupt(TimingISR);//declare the interrupt serve routine:TimingISR

  /*
   The MAX72XX is in power-saving mode on startup,
   we have to do a wakeup call
   */
  lc.shutdown(0,false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0,8);
  /* and clear the display */
  lc.clearDisplay(0);

  multiplexer.select(6);
  
  
  // flip screen, if required
  // u8g.setRot180();

  // assign default color value
  if ( u8g.getMode() == U8G_MODE_R3G3B2 ) {
    u8g.setColorIndex(255);     // white
  } else
  if ( u8g.getMode() == U8G_MODE_GRAY2BIT ) {
    u8g.setColorIndex(3);         // max intensity
  } else
  if ( u8g.getMode() == U8G_MODE_BW ) {
    u8g.setColorIndex(1);         // pixel on
  } else
  if ( u8g.getMode() == U8G_MODE_HICOLOR ) {
    u8g.setHiColorByRGB(255,255,255);
  }
//  pinMode(8, OUTPUT);
}

void loop(void) {
//  if(Update == ON) {
//    TimeUpdate();
//    tm1637.display(TimeDisp);
//    writeArduinoOnMatrix();
//  }
//  
//  multiplexer.select(6);
//  // picture loop
//  u8g.firstPage();  
//  do {
//    draw();
//  } while( u8g.nextPage() );
  
  multiplexer.select(8);
  uint16_t c = compass.get();
  Serial.print("  Compass: "); Serial.print(c);
  Serial.println();
  delay(500);
//  motor.set(100, 100);      // set motor speed (left right)
//  buttonTest();
}



void buttonTest(void) {
  Serial.print( analogRead(A3) );
  Serial.print("  Button A: "); Serial.print(robot.button.get(0));
  Serial.print("  Button B: "); Serial.print(robot.button.get(1));
  Serial.print("  Button C: "); Serial.print(robot.button.get(2));
  Serial.print(" ");
  Serial.println();
}



/* we always wait a bit between updates of the display */
unsigned long delaytime=500;

void draw(void) {
  // graphic commands to redraw the complete screen should be placed here  
  u8g.setFont(u8g_font_unifont);
  //u8g.setFont(u8g_font_osb21);
  u8g.drawStr( 0, 22, "ujyftyuk");
}


void TimingISR(void) {
  halfsecond ++;
  Update = ON;
  if (halfsecond == 2) {
    second ++;
    if (second == 60) {
      minute ++;
      if(minute == 60) {
        hour ++;
        if(hour == 24)
          hour = 0;
        minute = 0;
      }
      second = 0;
    }
    halfsecond = 0;
  }
 // Serial.println(second);
  ClockPoint = (~ClockPoint) & 0x01;
}

void TimeUpdate(void) {
  if(ClockPoint)  tm1637.point(POINT_ON);
  else            tm1637.point(POINT_OFF);
  TimeDisp[0] = hour / 10;
  TimeDisp[1] = hour % 10;
  TimeDisp[2] = minute / 10;
  TimeDisp[3] = minute % 10;
  Update = OFF;
}



/* here is the data for the word "Arduino" */
byte a[5]={B01111110,B10001000,B10001000,B10001000,B01111110};
byte r[5]={B00111110,B00010000,B00100000,B00100000,B00010000};
byte d[5]={B00011100,B00100010,B00100010,B00010010,B11111110};
byte u[5]={B00111100,B00000010,B00000010,B00000100,B00111110};
byte i[5]={B00000000,B00100010,B10111110,B00000010,B00000000};
byte n[5]={B00111110,B00010000,B00100000,B00100000,B00011110};
byte o[5]={B00011100,B00100010,B00100010,B00100010,B00011100};

byte *arduino[7] = {a,r,d,u,i,n,o};

byte charIdx = 0;
/*
 This method will display the characters for the
 word "Arduino" one after the other on the matrix. 
 (you need at least 5x7 leds to see the whole chars)
 */
 
void writeArduinoOnMatrix() {
  for (byte i=0; i<5; i++)
    lc.setRow(0,i,arduino[charIdx][i]);
  charIdx++;
  if (charIdx>=7)charIdx = 0;
}
