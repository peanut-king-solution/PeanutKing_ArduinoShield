/* SevenSegment.cpp
 * digit 7-Segment display library4
 * Copyright (c) 2021 PeanutKing Solution
 * Author     : Jack Kwok
 * Create Time: 7 Jul 2021
 * Change Log :
 */

#include "SevenSegment.h"
// #include <Wire.h>

static int8_t TubeTab[] = {
  0x3f,0x06,0x5b,0x4f,
  0x66,0x6d,0x7d,0x07,
  0x7f,0x6f,0x77,0x7c,
  0x39,0x5e,0x79,0x71
};  //0~9,A,b,C,d,E,F

SevenSegment::SevenSegment(uint8_t Clk, uint8_t Data) :
  clkPin(Clk), dtaPin(Data) {
  pinMode(clkPin, OUTPUT);
  pinMode(dtaPin, OUTPUT);
}

int SevenSegment::writeByte(int8_t wr_data) {
  uint8_t i, count1;
  for(i=0;i<8;i++) {              //sent 8bit data
    digitalWrite(clkPin, LOW);
    if (wr_data & 0x01)           //LSB first
      digitalWrite(dtaPin, HIGH);
    else
      digitalWrite(dtaPin, LOW);
    wr_data >>= 1;
    digitalWrite(clkPin, HIGH);
  }
  digitalWrite(clkPin, LOW);       //wait for the ACK
  digitalWrite(dtaPin, HIGH);
  digitalWrite(clkPin, HIGH);
  pinMode(dtaPin, INPUT);

  bitDelay();
  uint8_t ack = digitalRead(dtaPin);
  if (ack == 0) {
    pinMode(dtaPin, OUTPUT);
    digitalWrite(dtaPin, LOW);
  }
  bitDelay();
  pinMode(dtaPin, OUTPUT);
  bitDelay();
  return ack;
}

//send start signal to SevenSegment
void SevenSegment::start(void) {
  digitalWrite(clkPin,HIGH);//send start signal to SevenSegment
  digitalWrite(dtaPin,HIGH);
  digitalWrite(dtaPin,LOW);
  digitalWrite(clkPin,LOW);
}

//End of transmission
void SevenSegment::stop(void) {
  digitalWrite(clkPin,LOW);
  digitalWrite(dtaPin,LOW);
  digitalWrite(clkPin,HIGH);
  digitalWrite(dtaPin,HIGH);
}

//display function.Write to full-screen.
void SevenSegment::display(uint8_t DispData[]) {
  uint8_t SegData[4];
  uint8_t i;
  for(i=0; i<4; i++) {
    SegData[i] = DispData[i];
  }
  coding(SegData);
  // Wire.beginTransmission(ADDR_AUTO);
  // Wire.write(Cmd_SetAddr);
  // Wire.write(SegData, 4);
  // Wire.write(Cmd_DispCtrl);
  // Wire.endTransmission();

  start();          //start signal sent to SevenSegment from MCU
  writeByte(ADDR_AUTO);//
  stop();           //
  start();          //
  writeByte(Cmd_SetAddr);//
  for(i=0; i<4; i++) {
    writeByte(SegData[i]);        //
  }
  stop();           //
  start();          //
  writeByte(Cmd_DispCtrl);//
  stop();           //
}

//******************************************
void SevenSegment::display(uint8_t BitAddr,int8_t DispData) {
  int8_t SegData;
  SegData = coding(DispData);
  start();          //start signal sent to SevenSegment from MCU
  writeByte(ADDR_FIXED);//
  stop();           //
  start();          //
  writeByte(BitAddr|0xc0);//
  writeByte(SegData);//
  stop();            //
  start();          //
  writeByte(Cmd_DispCtrl);//
  stop();           //
}

void SevenSegment::clearDisplay(void) {
  display(0x00, 0x7f);
  display(0x01, 0x7f);
  display(0x02, 0x7f);
  display(0x03, 0x7f);
}

//To take effect the next time it displays.
void SevenSegment::set(uint8_t brightness,uint8_t SetData,uint8_t SetAddr) {
  Cmd_SetData = SetData;
  Cmd_SetAddr = SetAddr;
  Cmd_DispCtrl = 0x88 + brightness;//Set the brightness and it takes effect the next time it displays.
}

void SevenSegment::coding(uint8_t DispData[]) {
  uint8_t PointData = (_PointFlag == POINT_ON) ? 0x80 : 0;
  for(uint8_t i=0; i<4; i++) {
    if (DispData[i] == 0x7f) DispData[i] = 0x00;
    else DispData[i] = TubeTab[DispData[i]] + PointData;
  }
}

uint8_t SevenSegment::coding(uint8_t DispData) {
  uint8_t PointData = (_PointFlag == POINT_ON) ? 0x80 : 0;
  if (DispData == 0x7f) DispData = 0x00 + PointData; //The bit digital tube off
  else DispData = TubeTab[DispData] + PointData;
  return DispData;
}


uint8_t SevenSegment::invert(uint8_t DispData) {
  uint8_t invertData = 0x00;
  for(uint8_t i=0; i<8; i++) {
    invertData |= ( (DispData >> i) & 0x01) << (7-i);
  }
  return invertData;
}
