/* SevenSegment.h
 * digit 7-Segment display library4
 * Copyright (c) 2021 PeanutKing Solution
 * Author     : Jack Kwok
 * Create Time: 7 Jul 2021
 * Change Log :
 */

#ifndef SEVENSEGMENT_H
#define SEVENSEGMENT_H

#include <inttypes.h>
#include <Arduino.h>
// #include <TimerOneOVF.h>
/************definitions for SevenSegment ADDR*****************/
#define ADDR_AUTO  0x40
#define ADDR_FIXED 0x44
#define STARTADDR  0xc0
/**** definitions for the clock point of the digit tube *******/
#define POINT_ON   1
#define POINT_OFF  0
/**************definitions for brightness**********************/
#define BRIGHT_DARKEST 0
#define BRIGHT_TYPICAL 2
#define BRIGHTEST      7

class SevenSegment {
 public:
  uint8_t Cmd_SetData;
  uint8_t Cmd_SetAddr;
  uint8_t Cmd_DispCtrl;
  boolean _PointFlag;     //_PointFlag=1:the clock point on

  SevenSegment(uint8_t, uint8_t);
  int   writeByte(int8_t wr_data);//write 8bit data to tm1637
  void  start(void);//send start bits
  void  stop(void); //send stop bits
  void  display(uint8_t DispData[]);
  void  display(uint8_t BitAddr,int8_t DispData);
  void  clearDisplay(void);
  void  set(uint8_t = BRIGHT_TYPICAL,uint8_t = 0x40,uint8_t = 0xc0);//To take effect the next time it displays.
  void  coding(uint8_t DispData[]);
  uint8_t coding(uint8_t DispData);

  uint8_t invert(uint8_t DispData);

  void init(void) {        //To clear the display
    clearDisplay();
  }

  //whether to light the clock point ":".To take effect the next time it displays.
  inline void point(boolean PointFlag) {
    _PointFlag = PointFlag;
  }

  inline void bitDelay(void) {
    delayMicroseconds(50);
  }

 private:
  const uint8_t clkPin;
  const uint8_t dtaPin;
};
#endif
