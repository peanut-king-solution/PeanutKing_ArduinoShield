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


void I2CSend(int8_t addr, uint8_t *data, uint8_t length) {
  Wire.beginTransmission(addr);
  Wire.write(data, length);
  Wire.endTransmission();
}

void I2CRead(int8_t addr, uint8_t *data, uint8_t length) {
  uint8_t i=0;
  Wire.requestFrom((int)addr, (int)length);
  while (Wire.available()) {
    data[i++] = Wire.read();
  }
}


color_t PeanutKingArduinoShield::readcolor(uint8_t index) {
  
  return rgbcolor.getcolor(index);
}
hsl_t PeanutKingArduinoShield::readhsl(uint8_t index) {
  
  return rgbcolor.gethsl(index);
}
rgbc_t PeanutKingArduinoShield::readrgb(uint8_t index) {
  
  return rgbcolor.getrgb(index);
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



