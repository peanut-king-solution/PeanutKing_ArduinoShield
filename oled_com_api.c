/*

  oled_com_api.c

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

#include "oled.h"

uint8_t oled_InitCom(oled_t *oled, oled_dev_t *dev, uint8_t clk_cycle_time)
{
  return dev->com_fn(oled, OLED_COM_MSG_INIT, clk_cycle_time, NULL);
}

void oled_StopCom(oled_t *oled, oled_dev_t *dev)
{
  dev->com_fn(oled, OLED_COM_MSG_STOP, 0, NULL);
}

/* cs contains the chip number, which should be enabled */
void oled_SetChipSelect(oled_t *oled, oled_dev_t *dev, uint8_t cs)
{
  dev->com_fn(oled, OLED_COM_MSG_CHIP_SELECT, cs, NULL);
}

void oled_SetResetLow(oled_t *oled, oled_dev_t *dev)
{
  dev->com_fn(oled, OLED_COM_MSG_RESET, 0, NULL);
}

void oled_SetResetHigh(oled_t *oled, oled_dev_t *dev)
{
  dev->com_fn(oled, OLED_COM_MSG_RESET, 1, NULL);
}


void oled_SetAddress(oled_t *oled, oled_dev_t *dev, uint8_t address)
{
  dev->com_fn(oled, OLED_COM_MSG_ADDRESS, address, NULL);
}

uint8_t oled_WriteByte(oled_t *oled, oled_dev_t *dev, uint8_t val)
{
  return dev->com_fn(oled, OLED_COM_MSG_WRITE_BYTE, val, NULL);
}

uint8_t oled_WriteSequence(oled_t *oled, oled_dev_t *dev, uint8_t cnt, uint8_t *seq)
{
  return dev->com_fn(oled, OLED_COM_MSG_WRITE_SEQ, cnt, seq);
}

uint8_t oled_WriteSequenceP(oled_t *oled, oled_dev_t *dev, uint8_t cnt, const uint8_t *seq)
{
  return dev->com_fn(oled, OLED_COM_MSG_WRITE_SEQ_P, cnt, (void *)seq);
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

#define OLED_ESC_DLY(x) 255, ((x) & 0x7f)
#define OLED_ESC_CS(x) 255, (0xd0 | ((x)&0x0f))
#define OLED_ESC_ADR(x) 255, (0xe0 | ((x)&0x0f))
#define OLED_ESC_VCC(x) 255, (0xbe | ((x)&0x01))
#define OLED_ESC_END 255, 254
#define OLED_ESC_255 255, 255
#define OLED_ESC_RST(x) 255, (0xc0 | ((x)&0x0f))

*/
uint8_t oled_WriteEscSeqP(oled_t *oled, oled_dev_t *dev, const uint8_t *esc_seq)
{
  uint8_t is_escape = 0;
  uint8_t value;
  for(;;)
  {
    value = oled_pgm_read(esc_seq);
    if ( is_escape == 0 )
    {
      if ( value != 255 )
      {
        if ( oled_WriteByte(oled, dev, value) == 0 )
          return 0;
      }
      else
      {
        is_escape = 1;
      }
    }
    else
    {
      if ( value == 255 )
      {
        if ( oled_WriteByte(oled, dev, value) == 0 )
          return 0;
      }
      else if ( value == 254 )
      {
        break;
      }
      else if ( value >= 0x0f0 )
      {
        /* not yet used, do nothing */
      }
      else if ( value >= 0xe0  )
      {
        oled_SetAddress(oled, dev, value & 0x0f);
      }
      else if ( value >= 0xd0 )
      {
        oled_SetChipSelect(oled, dev, value & 0x0f);
      }
      else if ( value >= 0xc0 )
      {
        oled_SetResetLow(oled, dev);
        value &= 0x0f;
        value <<= 4;
        value+=2;
        oled_Delay(value);
        oled_SetResetHigh(oled, dev);
        oled_Delay(value);
      }
      else if ( value >= 0xbe )
      {
	/* not yet implemented */
        /* oled_SetVCC(oled, dev, value & 0x01); */
      }
      else if ( value <= 127 )
      {
        oled_Delay(value);
      }
      is_escape = 0;
    }
    esc_seq++;
  }
  return 1;
}

