/*
 * Copyright (c) 2021 PeanutKing Solution
 *
 * @file        PeanutKingArduinoShield.h
 * @summary     
 * @version     1.0
 * @author      Jack Kwok
 * @data        1 May 2021
 */

#include <Arduino.h>
#include <nI2C.h>
#include "TimerOneOVF.h"
#include "SevenSegment.h"
#include "OLED.h"
#include "LedMatrix.h"

#pragma once
#ifndef PeanutKingArduinoShield_H
#define PeanutKingArduinoShield_H

/** Hardware
 * 1. multiplexer，TCARST係arduino D2，要寫個lib，好似中鳴咁，乜sensor咩名插I2C幾
 *    之後佢call個個名就call multiplexer address去轉channel幾
 * 2. 1 analog pin 3 button，A5個位，可以用Jumper連去button, 
 *    之後analog read可以知有邊d掣禁左
 * 3. Main i2c仲有個mon同compass既
 * 4. motor, Ain1 D10,Ain2 D11, Bin1 D12, Bin2 D13
 */

#define TCAADDR 0x70

#define PIN_BUTTON  1
#define PIN_MOTOR   1

#define ADDR_MONITOR 1
#define ADDR_COMPASS 1
#define ADDR_MULPLXR 1

// #define colorSensor

// PeanutkingCompass
// oled
// multiplexer
// PeanutkingColorSensor
// Ultrasound

typedef enum { front = 0, left, right, back } sensorNum;

typedef struct {
  uint32_t  c;      //[0-65536]
  uint32_t  r;
  uint32_t  g;
  uint32_t  b;
} rgbc_t; //RGBC

typedef enum {
  black=0,  white,   grey,
  red,      green,   blue, 
  yellow,   cyan,    magenta
} color_t;

typedef struct{
  uint16_t h;       //[0,360]
  uint8_t  s;       //[0,100]
  uint8_t  l;       //[0,100]
} hsl_t;  //HSL

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} rgb_t; 

void I2CSend(CI2C::Handle handle, uint8_t *data, uint8_t length);
void I2CRead(CI2C::Handle handle, uint8_t *data, uint8_t length);
void _statusError(const uint8_t _status);

// Multiplexer -----------------------------------------------------
class Multiplexer {
 public:
  Multiplexer() {
    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);
    mulPlxHandle = nI2C->RegisterDevice(TCAADDR, 1, CI2C::Speed::SLOW);
  }
  void select(uint8_t i) {
    if (i > 8) return;
    uint8_t message[1] = { (1 << idx[i]) };
    uint8_t _status = nI2C->Write(mulPlxHandle, message, 1);
    if (_status != 0) {
      Serial.print("write: ");
      _statusError(_status);
      return;
    }
  }
  // B1-B8: idx[1] - idx[8]
  const uint8_t idx[9] = {0, 7, 6, 5, 4, 0, 1, 2, 3};
  CI2C::Handle mulPlxHandle;
};

//color sensor
class colorSensor {
 public:
  Multiplexer Multiplexer;
  colorSensor() {
    clrSnrHandle = nI2C->RegisterDevice(0x11, 1, CI2C::Speed::SLOW);
  }

  color_t getcolor(uint8_t index) {
    uint8_t buff[1]={0};
    Multiplexer.select(index);
    I2CSend(clrSnrHandle, 0x01, 1);
    I2CRead(clrSnrHandle, buff, 1);

    return buff[0];
  }
  hsl_t gethsl(uint8_t index ) {
     hsl_t hsl;
    Multiplexer.select(index);
    //I2CSend(clrSnrHandle,0x03, 1);
    I2CRead(clrSnrHandle, (uint8_t *)&hsl, 4);
  // hsl->h = buff[0]|buff[1]<<8;
  // hsl->s = buff[2];
  // hsl->l = buff[3];
    return hsl;
  }
  rgbc_t getrgb(uint8_t index ) {
    rgbc_t rgbc;
    //  rgb_t rgb;
    Multiplexer.select(index);
    //I2CSend(clrSnrHandle,0x02, 1);
    I2CRead(clrSnrHandle, (uint8_t *)&rgbc, 16);
    // I2CRead(clrSnrHandle, (uint8_t *)&rgb, 3);
    // Serial.print("rgb: ");
    // Serial.print(rgbc.c);  Serial.print(" ");
    // Serial.print(rgbc.r);  Serial.print(" ");
    // Serial.print(rgbc.g);  Serial.print(" ");
    // Serial.println(rgbc.b);

    // Serial.print("rgb: ");
    // Serial.print(rgb.r);   Serial.print(" ");
    // Serial.print(rgb.g);   Serial.print(" ");
    // Serial.println(rgb.b);  
    return rgbc;
  }

  CI2C::Handle clrSnrHandle;
};



// Button -----------------------------------------------------
class Button {
 public:
  Button() {
    pinMode(pin, INPUT);
  }
  Button(uint8_t pin) : pin(pin) {
    pinMode(pin, INPUT);
  }

  bool get(uint8_t i) {
    uint8_t button;
    uint16_t value = analogRead(pin);
    if      (value > 700) button  = 0;
    else if (value > 400) button  = 1;
    else if (value > 300) button  = 2;
    else if (value > 220) button  = 3;
    else if (value > 175) button  = 4;
    else if (value > 150) button  = 5;
    else if (value > 130) button  = 6;
    else                  button  = 7;

    return (button&(1<<i));
  }

    // if      (value > 550) button  = 0;
    // else if (value > 330) button  = 1;
    // else if (value > 230) button  = 2;
    // else if (value > 180) button  = 3;
    // else if (value > 140) button  = 4;
    // else if (value > 120) button  = 5;
    // else if (value > 105) button  = 6;
    // else                  button  = 7;
  const uint8_t pin = A3;
};


// getCompass -----------------------------------------------------
class Compass {
 public:
  Compass() : addr(8) {
    cpsHandle = nI2C->RegisterDevice(addr, 1, CI2C::Speed::SLOW);
  }
  Compass(int8_t _addr) : addr(_addr) {
    cpsHandle = nI2C->RegisterDevice(addr, 1, CI2C::Speed::SLOW);
  }
  float get(uint8_t cmd = 0x55) {
    uint16_t temp=0;
    uint8_t txBuff[1] = {cmd};
    uint8_t _status;

    _status = nI2C->Write(cpsHandle, txBuff, 1);
    if (_status != 0) {
      Serial.print("write: ");
      _statusError(_status);
      return;
    }

    _status = nI2C->Read(cpsHandle, data, 3);
    if (_status != 0) {
      Serial.print("write: ");
      _statusError(_status);
      return;
    }
    temp  = data[1] & 0xFF;
    temp |= (data[2] << 8);
    answer = temp/100.0;
    return answer;
  }

  const int8_t addr;
  CI2C::Handle cpsHandle;

  uint8_t data[3] = {0};
  float answer = 888;
};

// getUltrasonic --------------------------------------------------
class Ultrasonic {
 public:
  Ultrasonic(uint8_t tx, uint8_t rx) :
  txPin(tx), rxPin(rx) {
    pinMode(tx, OUTPUT);
    pinMode(rx, INPUT);
  }
  uint16_t get() {
    uint32_t duration=0;
    uint16_t distance=0;
    digitalWrite(txPin, LOW);
    delayMicroseconds(2);
    digitalWrite(txPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(txPin, LOW);
    duration = pulseIn(rxPin, HIGH, 13000);
    distance = ( duration==0 ) ? 888 : duration*0.017; //0.034/2;
    return distance;
  }
  const uint8_t txPin, rxPin;
};


class Motor {
 public:
  Motor() :
  dirPin  {6, 10},
  dir2Pin {9, 11} {
    pinMode(dirPin[0], OUTPUT);
    pinMode(dirPin[1], OUTPUT);
    pinMode(dir2Pin[0], OUTPUT);
    pinMode(dir2Pin[1], OUTPUT);
  }

  void set(int16_t left, int16_t right) {
    int16_t speed[2] = {left, right};
    for(uint8_t i=0; i<2; i++) {
      if ( speed[i]>0 && speed[i]<256 ) {
        analogWrite(dirPin[i], speed[i]);
        digitalWrite(dir2Pin[i], LOW);
      } else
      if ( speed[i]<0 && speed[i]>-256 ) {
        digitalWrite(dirPin[i], LOW);
        analogWrite(dir2Pin[i], -speed[i]);
      }
      else {
        digitalWrite(dirPin[i], LOW);
        digitalWrite(dir2Pin[i], LOW);
      }
    }
  }

  const uint8_t dirPin[2], dir2Pin[2];
};




// PeanutKingArduinoShield -----------------------------------------------------
class PeanutKingArduinoShield {
 public:
  PeanutKingArduinoShield() {
  }
  Button      button;
  Compass     compass;
  colorSensor rgbcolor;
  
  hsl_t   readhsl(uint8_t index);
  color_t readcolor(uint8_t index);
  rgbc_t  readrgb(uint8_t index);
};

#endif

