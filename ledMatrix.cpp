/* ledMatrix.cpp
 * digit 7-Segment display library4
 * Copyright (c) 2021 PeanutKing Solution
 * Author     : Jack Kwok
 * Create Time: 7 Jul 2021
 * Change Log :
 */

#include "ledMatrix.h"

ledMatrix::ledMatrix(int dataPin, int clkPin, int csPin, int numDevices) {
  SPI_MOSI  = dataPin;
  SPI_CLK   = clkPin;
  SPI_CS    = csPin;
  if (numDevices<=0 || numDevices>8 )
    numDevices=8;
  maxDevices=numDevices;
  pinMode(SPI_MOSI,OUTPUT);
  pinMode(SPI_CLK,OUTPUT);
  pinMode(SPI_CS,OUTPUT);
  digitalWrite(SPI_CS,HIGH);
  SPI_MOSI=dataPin;
  for (uint8_t i=0;i<64;i++) 
    status[i]=0x00;
  for (uint8_t i=0; i<maxDevices; i++) {
    spiTransfer(i,OP_DISPLAYTEST,0);  //scanlimit is set to max on startup
    setScanLimit(i,7);                //decode is done in source
    spiTransfer(i,OP_DECODEMODE,0);
    clearDisplay(i);                  //we go into shutdown-mode on startup
    shutdown(i,true);
  }
}

void ledMatrix::shutdown(int addr, bool b) {
  if (addr<0 || addr>=maxDevices) return;
  spiTransfer(addr, OP_SHUTDOWN, b ? 0 : 1);
}

void ledMatrix::setScanLimit(int addr, int limit) {
  if (addr<0 || addr>=maxDevices) return;
  if (limit>=0 && limit<8)
    spiTransfer(addr, OP_SCANLIMIT, limit);
}

void ledMatrix::setIntensity(int addr, int intensity) {
  if (addr<0 || addr>=maxDevices) return;
  if (intensity>=0 && intensity<16)	
    spiTransfer(addr, OP_INTENSITY, intensity);
}

void ledMatrix::clearDisplay(int addr) {
  int offset = addr*8;
  if (addr<0 || addr>=maxDevices) return;
  for (uint8_t i=0; i<8; i++) {
    status[offset+i]=0;
    spiTransfer(addr, i+1, status[offset+i]);
  }
}

void ledMatrix::setLed(int addr, int row, int column, boolean state) {
  int offset = addr*8;
  byte val = 0x00;

  if (addr<0 || addr>=maxDevices) return;
  if (row<0 || row>7 || column<0 || column>7) return;
  val = B10000000 >> column;
  if (state)
    status[offset+row]=status[offset+row]|val;
  else {
    val=~val;
    status[offset+row]=status[offset+row]&val;
  }
  spiTransfer(addr, row+1,status[offset+row]);
}

void ledMatrix::setRow(int addr, int row, byte value) {
  int offset = addr*8;
  if (addr<0 || addr>=maxDevices) return;
  if (row<0 || row>7)   return;
  status[offset+row]=value;
  spiTransfer(addr, row+1,status[offset+row]);
}

void ledMatrix::setColumn(int addr, int col, byte value) {
  byte val;
  if (addr<0 || addr>=maxDevices) return;
  if (col<0  || col>7)            return;
  for (uint8_t row=0; row<8; row++) {
    val = value >> (7-row);
    val = val & 0x01;
    setLed(addr,row,col,val);
  }
}

void ledMatrix::spiTransfer(int addr, volatile byte opcode, volatile byte data) {
  //Create an array with the data to shift out
  int offset=addr*2;
  int maxbytes=maxDevices*2;

  for (uint8_t i=0; i<maxbytes; i++)
    spidata[i]=(byte)0;
  //put our device data into the array
  spidata[offset+1]=opcode;
  spidata[offset]=data;
  //enable the line 
  digitalWrite(SPI_CS,LOW);
  //Now shift out the data 
  for (uint8_t i=maxbytes; i>0; i--)
    shiftOut(SPI_MOSI,SPI_CLK,MSBFIRST,spidata[i-1]);
  //latch the data onto the display
  digitalWrite(SPI_CS,HIGH);
}
 