/*

  u8g_com_api.c

  Universal 8bit Graphics Library
  
  Copyright (c) 2011, olikraus@gmail.com
  All rights reserved.

  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this list 
    of conditions and the following disclaimer.
    
  * Redistributions in binary form must reproduce the above copyright notice, this 
    list of conditions and the following disclaimer in the documentation and/or other 
    materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
  CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR 
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF 
  ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.  
  
  
*/

#include "u8g.h"

#define I2C_SLA         (0x3c*2)
//#define I2C_CMD_MODE  0x080
#define I2C_CMD_MODE    0x000
#define I2C_DATA_MODE   0x040

uint8_t u8g_com_arduino_ssd_start_sequence(u8g_t *u8g) {
  /* are we requested to set the a0 state? */
  if ( u8g->pin_list[U8G_PI_SET_A0] == 0 )        return 1;

  /* setup bus, might be a repeated start */
  if ( u8g_i2c_start(I2C_SLA) == 0 )              return 0;
  if ( u8g->pin_list[U8G_PI_A0_STATE] == 0 )   {
    if ( u8g_i2c_send_byte(I2C_CMD_MODE) == 0 )   return 0;
  }
  else {
    if ( u8g_i2c_send_byte(I2C_DATA_MODE) == 0 )  return 0;
  }

  u8g->pin_list[U8G_PI_SET_A0] = 0;
  return 1;
}

uint8_t u8g_com_arduino_ssd_i2c_fn(u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr) {
  return 1;
}

uint8_t u8g_InitCom(u8g_t *u8g, u8g_dev_t *dev, uint8_t clk_cycle_time) {
  u8g_i2c_init(u8g->pin_list[U8G_PI_I2C_OPTION]);
  return 1;
}

void u8g_StopCom(u8g_t *u8g, u8g_dev_t *dev) {
}

/* cs contains the chip number, which should be enabled */
void u8g_SetChipSelect(u8g_t *u8g, u8g_dev_t *dev, uint8_t cs) {
  u8g->pin_list[U8G_PI_A0_STATE] = 0;
  u8g->pin_list[U8G_PI_SET_A0] = 1;    /* force a0 to set again, also forces start condition */
  if ( cs == 0 ) {
    /* disable chip, send stop condition */
    u8g_i2c_stop();
  }
  else {
    /* enable, do nothing: any byte writing will trigger the i2c start */
  }
}

void u8g_SetResetLow(u8g_t *u8g, u8g_dev_t *dev) {
}

void u8g_SetResetHigh(u8g_t *u8g, u8g_dev_t *dev) {
}

void u8g_SetAddress(u8g_t *u8g, u8g_dev_t *dev, uint8_t address) {
  /* define cmd (arg_val = 0) or data mode (arg_val = 1) */
  u8g->pin_list[U8G_PI_A0_STATE] = address;
  u8g->pin_list[U8G_PI_SET_A0] = 1;    /* force a0 to set again */
}

uint8_t u8g_WriteByte(u8g_t *u8g, u8g_dev_t *dev, uint8_t val) {
  if ( u8g_com_arduino_ssd_start_sequence(u8g) == 0 ) return u8g_i2c_stop(), 0;
  if ( u8g_i2c_send_byte(val) == 0 )                  return u8g_i2c_stop(), 0;
  return 1;
}

uint8_t u8g_WriteSequence(u8g_t *u8g, u8g_dev_t *dev, uint8_t cnt, uint8_t *seq) {
  if ( u8g_com_arduino_ssd_start_sequence(u8g) == 0 ) return u8g_i2c_stop(), 0;

  register uint8_t *ptr = seq;
  while( cnt > 0 ) {
    if ( u8g_i2c_send_byte(*ptr++) == 0 )             return u8g_i2c_stop(), 0;
    cnt--;
  }
  return 1;
}

uint8_t u8g_WriteSequenceP(u8g_t *u8g, u8g_dev_t *dev, uint8_t cnt, const uint8_t *seq) {
  if ( u8g_com_arduino_ssd_start_sequence(u8g) == 0 ) return u8g_i2c_stop(), 0;

  register uint8_t *ptr = seq;
  while( cnt > 0 ) {
    if ( u8g_i2c_send_byte(u8g_pgm_read(ptr)) == 0 )  return 0;
    ptr++;
    cnt--;
  }
  return 1;
}

/*
  sequence := { direct_value | escape_sequence }
  direct_value := 0..254
  escape_sequence := value_255 | sequence_end | delay | adr | cs | not_used 
  value_255 := 255 255
  sequence_end = 255 254
  delay := 255 0..127
  adr := 255 0x0e0 .. 0x0ef 
  cs := 255 0x0d0 .. 0x0df 
  not_used := 255 101..254

#define U8G_ESC_DLY(x) 255, ((x) & 0x7f)
#define U8G_ESC_CS(x) 255, (0xd0 | ((x)&0x0f))
#define U8G_ESC_ADR(x) 255, (0xe0 | ((x)&0x0f))
#define U8G_ESC_VCC(x) 255, (0xbe | ((x)&0x01))
#define U8G_ESC_END 255, 254
#define U8G_ESC_255 255, 255
#define U8G_ESC_RST(x) 255, (0xc0 | ((x)&0x0f))

*/
uint8_t u8g_WriteEscSeqP(u8g_t *u8g, u8g_dev_t *dev, const uint8_t *esc_seq) {
  uint8_t is_escape = 0;
  uint8_t value;
  for(;;) {
    value = u8g_pgm_read(esc_seq);
    if ( is_escape == 0 ) {
      if ( value != 255 ) {
        if ( u8g_WriteByte(u8g, dev, value) == 0 )
          return 0;
      }
      else {
        is_escape = 1;
      }
    }
    else {
      if ( value == 255 ) {
        if ( u8g_WriteByte(u8g, dev, value) == 0 )
          return 0;
      } else
      if ( value == 254 ) {
        break;
      } else
      if ( value >= 0xf0 ) {
        /* not yet used, do nothing */
      } else
      if ( value >= 0xe0 ) {
        u8g_SetAddress(u8g, dev, value & 0x0f);
      } else
      if ( value >= 0xd0 ) {
        u8g_SetChipSelect(u8g, dev, value & 0x0f);
      } else
      if ( value >= 0xc0 ) {
        u8g_SetResetLow(u8g, dev);
        value &= 0x0f;
        value <<= 4;
        value+=2;
        u8g_Delay(value);
        u8g_SetResetHigh(u8g, dev);
        u8g_Delay(value);
      } else
      if ( value >= 0xbe ) {
        /* not yet implemented */
        /* u8g_SetVCC(u8g, dev, value & 0x01); */
      } else
      if ( value <= 127 ) {
        u8g_Delay(value);
      }
      is_escape = 0;
    }
    esc_seq++;
  }
  return 1;
}

