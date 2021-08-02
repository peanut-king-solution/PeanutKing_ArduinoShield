/* LedMatrix.cpp
 * digit 7-Segment display library
 * Copyright (c) 2021 PeanutKing Solution
 * Author     : Jack Kwok
 * Create Time: 7 Jul 2021
 * Change Log :
 */



#ifndef LEDMATRIX_H
#define LEDMATRIX_H

#include <avr/pgmspace.h>
#include <Arduino.h>

//the opcodes for the MAX7221 and MAX7219
#define OP_NOOP       0
#define OP_DIGIT0     1
#define OP_DIGIT1     2
#define OP_DIGIT2     3
#define OP_DIGIT3     4
#define OP_DIGIT4     5
#define OP_DIGIT5     6
#define OP_DIGIT6     7
#define OP_DIGIT7     8
#define OP_DECODEMODE  9
#define OP_INTENSITY   10
#define OP_SCANLIMIT   11
#define OP_SHUTDOWN    12
#define OP_DISPLAYTEST 15

class LedMatrix {
 private:
  byte spidata[16]; // shifte data array to the devices
  byte status[64];  // led-status array for all 8 devices
  int SPI_MOSI;     // Data shift outs pin
  int SPI_CLK;      // clock signal pin
  int SPI_CS;       // Driven LOW for chip selectzion
  int maxDevices;   // Maximum number of devices

  /* Send out a single command to the device */
  void spiTransfer(int addr, byte opcode, byte data);

 public:
  LedMatrix(int dataPin, int csPin, int clkPin, int numDevices=1);

  // Returns the number of devices on this LedMatrix
  int getDeviceCount(void) {
    return maxDevices;
  }
  // status(true: power-down mode, false: normal operation
  void shutdown(int addr, bool status);

  /* 
   * Set the number of digits (or rows) to be displayed.
   * See datasheet for sideeffects of the scanlimit on the brightness
   * of the display. limit  number of digits to be displayed (1..8)
   */
  void setScanLimit(int addr, int limit);

  // intensity: the brightness of the display. (0..15)
  void setIntensity(int addr, int intensity);

  // Switch all Leds on the display off
  void clearDisplay(int addr);

  /* Set the status of a single Led. row (0..7) col (0..7) */
  void setLed(int addr, int row, int col, boolean state);
  void setRow(int addr, int row, byte value);     // row (0..7)
  void setColumn(int addr, int col, byte value);  // col (0..7)
};


#endif  //LedMatrix.h


