/*
 * Copyright (c) 2021 PeanutKing Solution
 *
 * @file        PeanutKingArduinoShield.h
 * @summary     
 * @version     1.0
 * @author      Jack Kwok
 * @data        1 May 2021
 */

#include "PeanutKingArduinoShield.h"

void i2cRxCallback(const uint8_t _status) {

}

void I2CSend(IICIT::Handle handle, uint8_t *data, uint8_t length) {
  uint8_t _status = gIIC->Write(handle, data, length);
}

void I2CRead(IICIT::Handle handle, uint8_t *data, uint8_t length) {
  for (uint8_t i=0; i<length; i++) data[i] = 0;
  uint8_t _status = gIIC->Read(handle, data, length);
  // uint8_t i=0;
  // Wire.requestFrom((int)addr, (int)length);
  // while (Wire.available()) {
  //   data[i++] = Wire.read();
  // }
}

void PeanutKingArduinoShield::pinSelect(uint8_t index){
  multiplexer.select(index);
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

void _statusError(const uint8_t _status) {
  Serial.print("Communication _status #: ");
  Serial.println(_status);
}

//ColorSensor
color_t PeanutKingArduinoShield::readcolor(uint8_t index) {
  return rgbcolor.getcolor(index);
}

hsl_t PeanutKingArduinoShield::readhsl(uint8_t index) {
  return rgbcolor.gethsl(index);
}

rgbc_t PeanutKingArduinoShield::readrgb(uint8_t index) {
  return rgbcolor.getrgbc(index);
}

//Compass
//get compass with (optional) multiplexer index and (optional) cmd, otherwise get by main I2C and commend=0x55
uint16_t PeanutKingArduinoShield::compassRead(uint8_t index=NULL, uint8_t cmd=0x55) {
  return compass.get(index,cmd);
}

//CompoundEye
//get compound eye value with (optional) multiplexer index, otherwise get by main I2C
int PeanutKingArduinoShield::compoundEyeRead(uint8_t data_id,uint8_t index=NULL){
  if(index!=NULL){
    multiplexer.select(index);
  }
  return compoundEye.compoundEyeRead(data_id);
}
//serial print ALL compound eye value
void PeanutKingArduinoShield::compoundEyeRead(){
  compoundEye.compoundEyeRead();
}
//serial print ALL compound eye value with (optional) multiplexer index
void PeanutKingArduinoShield::compoundEyeReadAll(uint8_t index=NULL){
  if(index!=NULL){
    multiplexer.select(index);
  }
  compoundEye.compoundEyeRead();
}

//Button
uint16_t PeanutKingArduinoShield::getButtonA(void){
  return button.get(0);
}

uint16_t PeanutKingArduinoShield::getButtonB(void){
  return button.get(1);
}

uint16_t PeanutKingArduinoShield::getButtonC(void){
  return button.get(2);
}

//Motor
void PeanutKingArduinoShield::setMotor(int16_t left,int16_t right){
  motor.set(left,right);
}

//Ultrasonic
uint16_t PeanutKingArduinoShield::getUltrasonic(uint16_t tx_pin_number,uint16_t rx_pin_number=NULL){
  //in most case, rx_pin_number can be ignore as trig and echo pin must be together
  //rx_pin_number are for situation that student got wrong wire that shifted trig and echo pin
  return ultrasonic.get(tx_pin_number,rx_pin_number);
}

//Servo
uint16_t PeanutKingArduinoShield::setServo(uint16_t pin_number,uint16_t servo_degree){
  switch (pin_number){
    case A0:
      servoA0.attach(A0);
      servoA0.write(servo_degree);
      break;
    case A1:
      servoA1.attach(A1);
      servoA1.write(servo_degree);
      break;
    case A2:
      servoA2.attach(A2);
      servoA2.write(servo_degree);
      break;
    case A5:
      servoA3.attach(A5);
      servoA3.write(servo_degree);
      break;
    case D3:
      servoD3.attach(D3);
      servoD3.write(servo_degree);
      break;
    case D4:
      servoD4.attach(D4);
      servoD4.write(servo_degree);
      break;
    case D5:
      servoD5.attach(D5);
      servoD5.write(servo_degree);
      break;
    case D7:
      servoD7.attach(D7);
      servoD7.write(servo_degree);
      break;
    case D8:
      servoD8.attach(D8);
      servoD8.write(servo_degree);
      break;
    case 12:
      servo12.attach(12);
      servo12.write(servo_degree);
      break;
    case 13:
      servo13.attach(13);
      servo13.write(servo_degree);
      break;
    default:
      return -1;
  }
}
//Accelerometer
float PeanutKingArduinoShield::getGyroscope(axisXYZ axis, gyroSen sensitivity,uint8_t index=NULL){\
  isGyroscopeInited(index);
  return accelerometer.gyroscope(axis, sensitivity);
}
float PeanutKingArduinoShield::getAxisRotation(axisXYZ axis, accelSen sensitivity,uint8_t index=NULL){
  isGyroscopeInited(index);
  return accelerometer.axisRotation(axis, sensitivity);
}
float PeanutKingArduinoShield::getAxisAcceleration(axisXYZ axis, accelSen sensitivity,uint8_t index=NULL){
  isGyroscopeInited(index);
  return accelerometer.axisAcceleration(axis, sensitivity);
}


/*
void PeanutKingArduinoShield::oledPrintInteger(int num,uint8_t space,uint8_t line){//print line
  String string=String(num);
  uint8_t i = (string.length()),j;
  //if(space<3)space=3;
  oled.clearline(line);
  if(space>15)space=15;
  if(line>=8)line=7;
  if(i<space){//default space=3 letters
    for(j=0;j<space-i;j++){
      oled.setTextXY(line,j);
      oled.putChar('0');
    }
  }
  oled.putString(string);
  //oled.setTextXY(0,0);
}

void PeanutKingArduinoShield::oledPrintString(String string,uint8_t line){
  if(line>=8)line=7;
  oled.clearline(line);
  oled.setTextXY(line,0);
  oled.putString(string);
}
*/

/*                             Advance Functions
 * ============================================================================= */

// void PeanutKingArduinoShield::definePin(uint8_t index, uint8_t sensor) {
//   multiplexerPin[index] = sensor;
// }


// int16_t PeanutKingArduinoShield::getSensor(uint8_t name) {
//   byte addr, txBuff[10], rxBuff[10], length;

//   txBuff[0] = addr;
//   I2CSend(addr, txBuff, 1);
//   I2CRead(addr, rxBuff, length);
// }

// void PeanutKingArduinoShield::setSensor(uint8_t name) {
//   byte addr, txBuff[10], rxBuff[10], length;

//   txBuff[0] = addr;
  //I2CSend(addr, txBuff, 1);
  //I2CRead(addr, rxBuff, 1);
//  if (rxBuff[0]!=2) return;

  // for (uint8_t i=0;i<length;i++) {
  //   txBuff[i+1] = data[i];
  // }
  // I2CSend(addr, txBuff, length+1);
// }

/*
void PeanutKingArduinoShield::debugconsole(const char str[]) {
  size_t n = 0;
  size_t l = strlen(str);
  while (1) {
    if (n==l)   break;
    if (write(*str++)) n++;
    else break;
  }
  return n;
}

uint16_t PeanutKingArduinoShield::readLaserSensor(uint8_t i) {
  tcaselect(i);
  return readRangeSingleMillimeters();
}

colorSensor_t PeanutKingArduinoShield::readcolorSensor(uint8_t i) {
  static uint32_t colorTimer[5] = {0};
  uint16_t r, g, b, c, colorTemp, lux;

  tcaselect(i);
  if ( millis() - colorTimer[i] > 24 ) {
    getRawData(&r, &g, &b, &c);
    colorTemp = calculateColorTemperature_dn40(r, g, b, c);
    lux = calculateLux(r, g, b);
    colorSensor_t s = {r, g, b, c, colorTemp, lux};
    return s;
  }
}
*/
//PeanutKingArduinoShield robot;