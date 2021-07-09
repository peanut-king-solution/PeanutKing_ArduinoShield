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
#include <Wire.h>
//#include <nI2C.h>

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

#define SENSOR 1

#define PIN_BUTTON 1
#define PIN_MOTOR 1

#define ADDR_MONITOR 1
#define ADDR_COMPASS 1
#define ADDR_MULPLXR 1

#define colorSensor

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

void I2CSend(int8_t addr, uint8_t *data, uint8_t length);
void I2CRead(int8_t addr, uint8_t *data, uint8_t length);







// Multiplexer -----------------------------------------------------
class Multiplexer {
 public:

  Multiplexer() {
  }

  void select(uint8_t i) {
    if (i > 7) return;
    uint8_t  message[1];
    message[0]=(1 << i);
   
    I2CSend(TCAADDR, message, 1);

  }
};


//color sensor
class color_sensor {
 public:
   Multiplexer Multiplexer;
  color_sensor() {
  }

  color_t getcolor(uint8_t index) {
    uint8_t buff[1]={0};
    Multiplexer.select(index);
    I2CSend(0x11,0x01, 1);
    I2CRead(0x11, buff, 1);


    return buff[0];
  }
  hsl_t gethsl(uint8_t index ) {
     hsl_t hsl;
    Multiplexer.select(index);
    //I2CSend(0x11,0x03, 1);
    I2CRead(0x11, (uint8_t *)&hsl, 4);
  // hsl->h = buff[0]|buff[1]<<8;
  // hsl->s = buff[2];
  // hsl->l = buff[3];
    return hsl;
  }
  rgbc_t getrgb(uint8_t index ) {
     rgbc_t rgbc;
    //  rgb_t rgb;
    Multiplexer.select(index);
    //I2CSend(0x11,0x02, 1);
    I2CRead(0x11, (uint8_t *)&rgbc, 16);
    // I2CRead(0x11, (uint8_t *)&rgb, 3);
//  Serial.print("rgb: ");
//     Serial.print(rgbc.c);
//   Serial.print(" ");
//    Serial.print(rgbc.r);
//   Serial.print(" ");
//   Serial.print(rgbc.g);
//   Serial.print(" ");
//   Serial.println(rgbc.b);  
//    Serial.print("rgb: ");
  
  //  Serial.print(rgb.r);
  // Serial.print(" ");
  // Serial.print(rgb.g);
  // Serial.print(" ");
  // Serial.println(rgb.b);  
    return rgbc;
     //return rgbc;
  }



};




// Button -----------------------------------------------------
class Button {
 public:
  Button() {
  }

  bool get(uint8_t i) {
    uint8_t value, button;

    value = analogRead(pin);

    if      (value > 690)  button  = 0;
    else if (value > 660) button  = 1;
    else if (value > 630) button  = 2;
    else if (value > 600) button  = 4;
    else if (value > 570) button  = 3;
    else if (value > 550) button  = 5;
    else if (value > 530) button  = 6;
    else                  button  = 7;

    return (button&(1<<i));
  }

  byte pin = A5;

};


// getCompass -----------------------------------------------------
class Compass {
 public:
  Compass() {
  }
  float get(int8_t addr = 8, int8_t cmd = 0x55) {
    uint8_t received_byte[3] = {0,0,0};
    uint8_t i=0;
    uint16_t temp=0;
    float answer = 888;
    Wire.beginTransmission(addr);
    Wire.write(cmd);
    Wire.endTransmission();
    Wire.requestFrom(addr, 3);
    while (Wire.available()) {
      received_byte[i++] = Wire.read();
    }
    temp  = received_byte[1] & 0xFF;
    temp |= (received_byte[2] << 8);
    answer = temp/100.0;

    return answer;
  }

};

// getUltrasonic --------------------------------------------------
class Ultrasonic {
 public:
  Ultrasonic(uint8_t tx, uint8_t rx) :
  txPin(tx), rxPin(rx) {
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
  
  uint8_t txPin, rxPin;
};

// PeanutKingArduinoShield -----------------------------------------------------
class PeanutKingArduinoShield {
 public:
  PeanutKingArduinoShield() {
     
  }
  hsl_t readhsl(uint8_t index);
  color_sensor rgbcolor;
  Button Button;
  color_t readcolor(uint8_t index);
  rgbc_t readrgb(uint8_t index);
};

#endif

