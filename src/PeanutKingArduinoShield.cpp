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

color_t PeanutKingArduinoShield::readcolor(uint8_t index) {
  return rgbcolor.getcolor(index);
}

hsl_t PeanutKingArduinoShield::readhsl(uint8_t index) {
  return rgbcolor.gethsl(index);
}

rgbc_t PeanutKingArduinoShield::readrgb(uint8_t index) {
  return rgbcolor.getrgbc(index);
}

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



