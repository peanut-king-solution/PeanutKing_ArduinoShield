/*

*/
#include "PeanutKingArduinoShield.h"

#define ON 1
#define OFF 0

uint8_t TimeDisp[] = {0x00, 0x00, 0x00, 0x00};
unsigned char ClockPoint = 1;
unsigned char Update;
unsigned char halfsecond = 0;
unsigned char second;
unsigned char minute = 0;
unsigned char hour = 12;

SevenSegment tm1637(8, 9);    // (CLK, DIO)
LedMatrix lc = LedMatrix(3, 4, 5, 1);
PeanutKingArduinoShield robot;
Multiplexer multiplexer = Multiplexer();

Button button = Button();
Compass compass = Compass();
//Ultrasonic ultrasonic = Ultrasonic(6, 7);
//colorSensor rgbcolor = colorSensor();
Motor motor = Motor();

void setup(void) {
  Serial.begin(115200);  // start serial for output
  
  delay(10);
  
  tm1637.set();
  tm1637.init();
  Timer1.initialize(500000);//timing for 500ms
  Timer1.attachInterrupt(TimingISR);//declare the interrupt serve routine:TimingISR

  delay(10);
  /*
    The MAX72XX is in power-saving mode on startup,
    we have to do a wakeup call
  */
  lc.shutdown(0, false);
  lc.setIntensity(0, 8);  /* Set the brightness to a medium values */
  lc.clearDisplay(0);     /* and clear the display */

  delay(10);

  oled.init();                      // Initialze SSD1306 OLED display
  oled.clearDisplay();              // Clear screen
  oled.setTextXY(0,0);              // Set cursor position, start of line 0
  oled.putString("aisufsdo");
  oled.setTextXY(1,0);              // Set cursor position, start of line 1
  oled.putString("asdfgb");
  oled.setTextXY(2,0);              // Set cursor position, start of line 2
  oled.putString("----");
  oled.setTextXY(2,10);             // Set cursor position, line 2 10th character
  oled.putString("CA");
//  multiplexer.select(6);
  //  pinMode(8, OUTPUT);
}

void loop(void) {
  static bool p = true;

  if (Update == ON) {
    TimeUpdate();
    tm1637.display(TimeDisp);
    writeArduinoOnMatrix();

    multiplexer.select(2);
    p = !p;

    multiplexer.select(8);
    uint16_t c = compass.get();
    Serial.print("  Compass: "); Serial.print(c);
    Serial.println();
  }
  delay(1);
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
unsigned long delaytime = 500;

void TimingISR(void) {
  halfsecond ++;
  Update = ON;
  if (halfsecond == 2) {
    second ++;
    if (second == 60) {
      minute ++;
      if (minute == 60) {
        hour ++;
        if (hour == 24)
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
  if (ClockPoint)  tm1637.point(POINT_ON);
  else            tm1637.point(POINT_OFF);
  TimeDisp[0] = hour / 10;
  TimeDisp[1] = hour % 10;
  TimeDisp[2] = minute / 10;
  TimeDisp[3] = minute % 10;
  Update = OFF;
}


/* here is the data for the word "Arduino" */
byte a[5] = {B01111110, B10001000, B10001000, B10001000, B01111110};
byte r[5] = {B00111110, B00010000, B00100000, B00100000, B00010000};
byte d[5] = {B00011100, B00100010, B00100010, B00010010, B11111110};
byte u[5] = {B00111100, B00000010, B00000010, B00000100, B00111110};
byte i[5] = {B00000000, B00100010, B10111110, B00000010, B00000000};
byte n[5] = {B00111110, B00010000, B00100000, B00100000, B00011110};
byte o[5] = {B00011100, B00100010, B00100010, B00100010, B00011100};

byte *arduino[7] = {a, r, d, u, i, n, o};
byte charIdx = 0;

void writeArduinoOnMatrix(void) {
  for (byte i = 0; i < 5; i++)
    lc.setRow(0, i, arduino[charIdx][i]);
  charIdx++;
  if (charIdx >= 7)charIdx = 0;
}
