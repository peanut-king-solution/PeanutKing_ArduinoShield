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
#include "IICIT.h"
#include "OLED.h"
#include "CompoundEye.h"
//#include "Wire.h"
#include "TimerOneOVF.h"                          
#include "SevenSegment.h"
#include "LedMatrix.h" 
#include "U8glib.h"
//#include <Servo.h>
#include <Accelerometer.h>

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

#define max3v(v1, v2, v3)   ((v1)<(v2)? ((v2)<(v3)?(v3):(v2)):((v1)<(v3)?(v3):(v1)))
#define min3v(v1, v2, v3)   ((v1)>(v2)? ((v2)>(v3)?(v3):(v2)):((v1)>(v3)?(v3):(v1)))

#define TCAADDR     0x70
#define PIN_BUTTON    1
#define PIN_MOTOR     1
#define ADDR_MONITOR  1
#define ADDR_COMPASS  1
#define ADDR_MULPLXR  1
// #define ColorSensor

// PeanutkingCompass
// oled
// servo

// multiplexer
#define C8 8
#define C7 7
#define C6 6
#define C5 5
#define C4 4
#define C3 3
#define C2 2
#define C1 1
// PeanutkingColorSensor
// Ultrasound
#define D8 8
#define D7 7
#define D5 5
#define D4 4
#define D3 3

typedef enum { front = 0, left, right, back } sensorNum;

typedef enum {
  black=0,  white,  
  red,      green,   blue, 
  yellow
} color_t;

typedef enum {buttonA=0,buttonB,buttonC} button_t;

typedef struct {
  uint32_t  c;      //[0-65536]
  uint32_t  r;
  uint32_t  g;
  uint32_t  b;
} rgbc_t; //RGBC

typedef struct {
  uint16_t h;       //[0,360]
  uint8_t  s;       //[0,100]
  uint8_t  l;       //[0,100]
} hsl_t;  //HSL

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} rgb_t; 

void i2cRxCallback(const uint8_t _status);
void I2CSend(IICIT::Handle handle, uint8_t *data, uint8_t length);
void I2CRead(IICIT::Handle handle, uint8_t *data, uint8_t length);
void _statusError(const uint8_t _status);
void rgb2hsl(rgbc_t *rgbc, hsl_t *hsl, rgb_t *rgb);


// Multiplexer -----------------------------------------------------
class Multiplexer {
 public:
  Multiplexer(void) {
    pinMode(2, OUTPUT);
    digitalWrite(2, HIGH);
    mulPlxHandle = gIIC->RegisterDevice(TCAADDR, 1, IICIT::Speed::SLOW);
  }
  void select(uint8_t i) {
    if (i >= 9) {
      Serial.begin(9600);
      Serial.print("non exist pin selected");
      return;
    }
    uint8_t message[1] = { 0x01 << (idx[i]) };
    uint8_t _status = gIIC->Write(mulPlxHandle, message, 1);
    if (_status != 0) {
      Serial.print("write: ");
      _statusError(_status);
      return;
    }
  }
  // B1-B8: idx[1] - idx[8]
  const uint8_t idx[9] = {0, 7, 6, 5, 4, 0, 1, 2, 3};
  IICIT::Handle mulPlxHandle;
};

//color sensor
class ColorSensor {
 public:
  Multiplexer multiplexer;
  ColorSensor(void) {
    clrSnrHandle = gIIC->RegisterDevice(0x11, 1, IICIT::Speed::SLOW);
  }

  uint8_t getcolor(uint8_t index=NULL) {
    uint8_t cmd = 0x01, ans;
    if(index!=NULL){
      multiplexer.select(index);
    }
    I2CSend(clrSnrHandle, &cmd, 1);
    I2CRead(clrSnrHandle, &ans, 1);
    return ans;
  }
  rgb_t getrgb(uint8_t index=NULL) {
    uint8_t cmd = 0x08;
    rgb_t rgb;
    if(index!=NULL){
      multiplexer.select(index);
    }
    I2CSend(clrSnrHandle, &cmd, 1);
    I2CRead(clrSnrHandle, (uint8_t *)&rgb, 3);
    return rgb;
  }
  hsl_t gethsl(uint8_t index=NULL) {
    uint8_t cmd = 0x03;
    hsl_t hsl;
    if(index!=NULL){
      multiplexer.select(index);
    }
    I2CSend(clrSnrHandle, &cmd, 1);
    I2CRead(clrSnrHandle, (uint8_t *)&hsl, 4);
  // hsl->h = buff[0]|buff[1]<<8;
  // hsl->s = buff[2];
  // hsl->l = buff[3];
    return hsl;
  }
  rgbc_t getrgbc(uint8_t index=NULL) {
    uint8_t cmd = 0x02;
    uint8_t _status;
    rgbc_t rgbc;
    if(index!=NULL){
      multiplexer.select(index);
    }
    I2CSend(clrSnrHandle, &cmd, 1);
    I2CRead(clrSnrHandle, (uint8_t *)&rgbc, 16);

    // _status = gIIC->Write(clrSnrHandle, &cmd, 1);
    // if (_status != 0) {
    //   Serial.print("write: ");
    //   _statusError(_status);
    //   return;
    // }

    // _status = gIIC->Read(clrSnrHandle, (uint8_t *)&rgbc, 16);
    // if (_status != 0) {
    //   Serial.print("write: ");
    //   _statusError(_status);
    //   return;
    // }

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

  IICIT::Handle clrSnrHandle;
};



// Button -----------------------------------------------------
class Button {
 public:
  Button(void) {
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
  Multiplexer multiplexer;
  Compass(void) : addr(8) {
    cpsHandle = gIIC->RegisterDevice(addr, 1, IICIT::Speed::SLOW);
  }
  Compass(int8_t _addr) : addr(_addr) {
    cpsHandle = gIIC->RegisterDevice(addr, 1, IICIT::Speed::SLOW);
  }
  float get(uint8_t index=NULL, uint8_t cmd = 0x55) {
    if(index!=NULL ){
      multiplexer.select(index);
    }
    uint16_t temp=0;
    uint8_t _status;

    _status = gIIC->Write(cpsHandle, &cmd, 1);
    if (_status != 0) {
      Serial.print("write: ");
      _statusError(_status);
      return 0;
    }

    _status = gIIC->Read(cpsHandle, data, 3);
    if (_status != 0) {
      Serial.print("write: ");
      _statusError(_status);
      return 0;
    }
    temp  = data[1] & 0xFF; // 1111 & data[1]            -> 1111data[1]
    temp |= (data[2] << 8); // 1111data[1] | data[2]0000 -> data[2]data[1]
    answer = temp/100.0;
    return answer;
  }

  const int8_t addr;
  IICIT::Handle cpsHandle;

  uint8_t data[3] = {0};
  float answer = 888;
};

// getUltrasonic --------------------------------------------------
class Ultrasonic {
 public:
  Ultrasonic(void):txPin(NULL), rxPin(NULL){

  }
  Ultrasonic(uint8_t tx, uint8_t rx) :
  txPin(tx), rxPin(rx) {
    pinMode(tx, OUTPUT);
    pinMode(rx, INPUT);
  }

  //for PeanutKingArduinoShield object internally use, need tx pin parameter
  uint16_t get(uint16_t tx_pin,uint16_t rx_pin=NULL){
    if(rx_pin==NULL){
      switch(tx_pin){
        case 13: rx_pin=12; break;
        case 12: rx_pin=8;  break;
        case 8:  rx_pin=7;  break;
        case 7:  rx_pin=5;  break;
        case 5:  rx_pin=4;  break;
        case 4:  rx_pin=3;  break;
        default:
          return -1;
      }
    }
    pinMode(tx_pin, OUTPUT);
    pinMode(rx_pin, INPUT);
    digitalWrite(tx_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(tx_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(tx_pin, LOW);
    return _cal_distance(pulseIn(rx_pin, HIGH));
  }

  //for ultrasonic object externally init and use, Tx,Rx pin are init ed at object initialization
  uint16_t get(void) {
    digitalWrite(txPin, LOW);
    delayMicroseconds(2);
    digitalWrite(txPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(txPin, LOW);
    return _cal_distance(pulseIn(rxPin, HIGH));
  }

  double _cal_distance(unsigned long durationMicroSec){
    double speedOfSoundInCmPerMs = 0.03313 + 0.0000606 * 19.307; // Cair ≈ (331.3 + 0.606 ⋅ ϑ) m/s
    double distanceCm = durationMicroSec / 2.0 * speedOfSoundInCmPerMs;
    if (distanceCm == 0 || distanceCm > 400) {
        return -1;
    } else {
        return distanceCm;
    }
  }
  const uint8_t txPin, rxPin;
};

// set Motor -----------------------------------------------------
class Motor {
 public:
  Motor(void) :
  dirPin  {6, 11},
  dir2Pin {9, 10} {
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
        analogWrite(dir2Pin[i], LOW);
      } else
      if ( speed[i]<0 && speed[i]>-256 ) {
        analogWrite(dirPin[i], LOW);
        analogWrite(dir2Pin[i], speed[i]);
      }
      else {
        analogWrite(dirPin[i], LOW);
        analogWrite(dir2Pin[i], LOW);
      }
    }
  }

  const uint8_t dirPin[2], dir2Pin[2];
};

// PeanutKingArduinoShield -----------------------------------------------------
class PeanutKingArduinoShield {
 public:
  Button      button;
  Compass     compass;
  ColorSensor rgbcolor;
  Multiplexer multiplexer;
  CompoundEye compoundEye;
  Ultrasonic ultrasonic;
  Motor motor;
  //Servo  servoA0,servoA1,servoA2,servoA3,servoD3,servoD4,servoD5,servoD7,servoD8,servo12,servo13;
  Accelerometer accelerometer;
  U8GLIB_SSD1306_128X64 oled;
  PeanutKingArduinoShield(void) {
    Serial.begin(115200);
    U8GLIB_SSD1306_128X64 oled(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);
  }

  //Multiplexer
  void pinSelect(uint8_t);

  //ColorSensor //current color sensor providing incorrect rgb value in percentage and worng hsl, and it related to firmware
  //if the firmware are already updated, please use the function in next column //  |
  hsl_t   readhsl(uint8_t pin_number=NULL);                                     //  |            
  color_t readcolor(uint8_t pin_number=NULL);                                   //  |            
  rgb_t  readrgb(uint8_t pin_number=NULL);                                      //  |            
  rgbc_t  readrgbc(uint8_t pin_number=NULL);                                    //  |            
  color_t readAdvColor(hsl_t hsl) {                                             //  |            
    if      ( hsl.l < 80 && hsl.s < 60  ) return black;                         //  |            
    else if ( hsl.h < 80 && hsl.h > 50)   return yellow;                        //  |            
    else if ( hsl.h > 150 && hsl.s < 30 && hsl.l > 60 )  return white;          //  |            
    else if ( hsl.h < 15 || hsl.h > 315 ) return red;                           //  |            
    else if ( hsl.h < 150 )               return green;                         //  |            
    else                                  return blue;                          //  |            
  }                                                                             //  V this                  
  //ColorSensor for updated firmware that provided correct rgb in 0 - 255 and correct hsl
  hsl_t   gethsl(uint8_t pin_number=NULL);
  rgb_t  getrgb(uint8_t pin_number=NULL);

  //Compass
  uint16_t compassRead(uint8_t index=NULL, uint8_t cmd=0x55);                   //index for multiplexer choice (optional) C1 to C8
                                                                                //cmd normally fixed at 0x55 to take yaw data only

  //CompoundEye                 
  void compoundEyeRead(void);                                                   //for compatibility,better keep this
  void compoundEyeReadAll(uint8_t index=NULL);                                  //functionally identical to compoundEyeRead(void) but provided multiplexer(optional)
  int compoundEyeRead(uint8_t data_id,uint8_t index=NULL);                      //get data by data id, provided multiplexer(optional)

  //Button                  
  uint16_t getButtonA(void);                                                    //simply get button boolean value, 0:OFF; 1:ON
  uint16_t getButtonB(void);                  
  uint16_t getButtonC(void);                  

  //Motor                 
  void setMotor(int16_t left ,int16_t right);                                   //set left and right motor from -255 to 255

  //Ultrasonic
  uint16_t getUltrasonic(uint16_t tx_pin_number,uint16_t rx_pin_number=NULL);   //get ultrasonic by input trig pin as tx_pin_number
                                                                                //in most case, rx_pin_number can be ignore as trig and echo pin must be together
                                                                                //rx_pin_number are for situation that student got wrong wire that shifted trig and echo pin

  //Servo
  //uint16_t setServo(uint16_t pin_number,uint16_t servo_degree);               //directly drive servo by input pin_number and servo_degree
                                                                                //curretly not included because
                                                                                //servo library conflict with motor that pin 9, pin 10 pwm control 
                                                                                //took as timer by servo lib, when motor control by pin 9 and pin 10
                                                                                //method1:
                                                                                //set the timer only when servo enable and reset the timer immediately after
                                                                                //the servo reach the settled angle
                                                                                //method2:
                                                                                //software timer
                                                                                //the pwm control for servo motor only need 50hz so a async task loop with 0.02 interval
                                                                                //should be ok for controllment too

  //Accelerometer
  private:
  bool inited[9];                                                               //main I2C + multiplexer 1 to 8 (optional)
  bool isGyroscopeInited(uint16_t index=NULL){                                  //check for init of mpu6050, init it by index if not inited 
    if(index!=NULL){                                                            //return false if it has not inited yet and just inited 
      multiplexer.select(index); 
      if(!inited[index]){
        inited[index]=1;
        accelerometer.initMPU6050();
      }
      return false;
    }else if((index==NULL || index==0) && !inited[0]){
      inited[index]=1;
      accelerometer.initMPU6050();
      return false;
    }
    return true;
  }
  public:                                                                       //usage same as microbit library except no need to init MPU6050
                                                                                //axis: axis_x/axis_y/axis_z
  float getGyroscope(axisXYZ axis, gyroSen sensitivity,uint8_t index=NULL);     //gyroSen:range_250_dps/range_500_dps/range_1000_dps/range_2000_dps
  float getAxisRotation(axisXYZ axis, accelSen sensitivity,uint8_t index=NULL); //accelSen: range_2_g/range_4_g/range_8_g/range_16_g
  float getAxisAcceleration(axisXYZ axis, accelSen sensitivity,uint8_t index=NULL);

                  
  //Oled                  
                                                                                //student need to pass a update function as a parameter to update oled screen content
  void oledUpdate(void (*updateFun)(void)){
    oled.firstPage();
    do{
      oled.setPrintPos(0,10);                                                   //pre set pos at (0,10)
      oled.setFont(u8g_font_unifont);                                           //in case student didn't set font
      updateFun();
    }while(oled.nextPage());
  }

};
#endif
