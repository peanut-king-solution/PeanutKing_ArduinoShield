/*

  U8glib.cpp

  C++ Interface

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
#include "oledScreen.h"

#define WIDTH       128
#define HEIGHT      64
#define PAGE_HEIGHT 8


#if defined(OLED_WITH_PINLIST)
uint8_t OLEDLIB::initSPI(oled_dev_t *dev, uint8_t sck, uint8_t mosi, uint8_t cs, uint8_t a0, uint8_t reset) {
  prepare();
  return oled_InitSPI(&oled, dev, sck, mosi, cs, a0, reset);
}

uint8_t OLEDLIB::initHWSPI(oled_dev_t *dev, uint8_t cs, uint8_t a0, uint8_t reset) {
  prepare();
  return oled_InitHWSPI(&oled, dev, cs, a0, reset);
}

uint8_t OLEDLIB::initI2C(oled_dev_t *dev, uint8_t options) {
  prepare();
  return oled_InitI2C(&oled, dev, options);
}

uint8_t OLEDLIB::init8Bit(oled_dev_t *dev, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7, uint8_t en, uint8_t cs1, uint8_t cs2, uint8_t di, uint8_t rw, uint8_t reset) {
  prepare();
  return oled_Init8Bit(&oled, dev, d0, d1, d2, d3, d4, d5, d6, d7, en, cs1, cs2, di, rw, reset); 
}

uint8_t OLEDLIB::init8BitFixedPort(oled_dev_t *dev, uint8_t en, uint8_t cs, uint8_t di, uint8_t rw, uint8_t reset) {
  prepare();
  return oled_Init8BitFixedPort(&oled, dev, en, cs, di, rw, reset);
}

uint8_t OLEDLIB::initRW8Bit(oled_dev_t *dev, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7, uint8_t cs, uint8_t a0, uint8_t wr, uint8_t rd, uint8_t reset) {
  prepare();
  return oled_InitRW8Bit(&oled, dev, d0, d1, d2, d3, d4, d5, d6, d7, cs, a0, wr, rd, reset); 
}
#endif



/*================================== ssd1306_128x64 ====================================*/
/*======================================================================================*/

/*
  oled_dev_ssd1306_128x64.c
  Universal 8bit Graphics Library
*/


/* init sequence adafruit 128x64 OLED (NOT TESTED) */
static const uint8_t oled_dev_ssd1306_128x64_adafruit1_init_seq[] PROGMEM = {
  OLED_ESC_CS(0),             /* disable chip */
  OLED_ESC_ADR(0),           /* instruction mode */
  OLED_ESC_RST(1),           /* do reset low pulse with (1*16)+2 milliseconds */
  OLED_ESC_CS(1),             /* enable chip */

  0x0ae,        /* display off, sleep mode */
  0x0d5, 0x080,   /* clock divide ratio (0x00=1) and oscillator frequency (0x8) */
  0x0a8, 0x03f,   /* */

  0x0d3, 0x000,   /*  */

  0x040,        /* start line */
  
  0x08d, 0x010,   /* [1] charge pump setting (p62): 0x014 enable, 0x010 disable */

  0x020, 0x000,   /* */
  0x0a1,        /* segment remap a0/a1*/
  0x0c8,        /* c0: scan dir normal, c8: reverse */
  0x0da, 0x012,   /* com pin HW config, sequential com pin config (bit 4), disable left/right remap (bit 5) */
  0x081, 0x09f,   /* [1] set contrast control */
  0x0d9, 0x022,   /* [1] pre-charge period 0x022/f1*/
  0x0db, 0x040,   /* vcomh deselect level */
  
  0x02e,        /* 2012-05-27: Deactivate scroll */ 
  0x0a4,        /* output ram to display */
  0x0a6,        /* none inverted normal display mode */
  0x0af,        /* display on */

  OLED_ESC_CS(0),      /* disable chip */
  OLED_ESC_END         /* end of sequence */
};

/* init sequence adafruit 128x64 OLED (NOT TESTED) */
static const uint8_t oled_dev_ssd1306_128x64_adafruit2_init_seq[] PROGMEM = {
  OLED_ESC_CS(0),       /* disable chip */
  OLED_ESC_ADR(0),      /* instruction mode */
  OLED_ESC_RST(1),      /* do reset low pulse with (1*16)+2 milliseconds */
  OLED_ESC_CS(1),       /* enable chip */

  0x0ae,        /* display off, sleep mode */
  0x0d5, 0x080,   /* clock divide ratio (0x00=1) and oscillator frequency (0x8) */
  0x0a8, 0x03f,   /* */

  0x0d3, 0x000,   /*  */

  0x040,        /* start line */
  
  0x08d, 0x014,   /* [2] charge pump setting (p62): 0x014 enable, 0x010 disable */

  0x020, 0x000,   /* */
  0x0a1,        /* segment remap a0/a1*/
  0x0c8,        /* c0: scan dir normal, c8: reverse */
  0x0da, 0x012,   /* com pin HW config, sequential com pin config (bit 4), disable left/right remap (bit 5) */
  0x081, 0x0cf,   /* [2] set contrast control */
  0x0d9, 0x0f1,   /* [2] pre-charge period 0x022/f1*/
  0x0db, 0x040,   /* vcomh deselect level */
  
  0x02e,        /* 2012-05-27: Deactivate scroll */ 
  0x0a4,        /* output ram to display */
  0x0a6,        /* none inverted normal display mode */
  0x0af,        /* display on */

  OLED_ESC_CS(0),        /* disable chip */
  OLED_ESC_END           /* end of sequence */
};

/* init sequence adafruit 128x64 OLED (NOT TESTED), like adafruit3, but with page addressing mode */
static const uint8_t oled_dev_ssd1306_128x64_adafruit3_init_seq[] PROGMEM = {
  OLED_ESC_CS(0),             /* disable chip */
  OLED_ESC_ADR(0),           /* instruction mode */
  OLED_ESC_RST(1),           /* do reset low pulse with (1*16)+2 milliseconds */
  OLED_ESC_CS(1),             /* enable chip */

  0x0ae,        /* display off, sleep mode */
  0x0d5, 0x080,    /* clock divide ratio (0x00=1) and oscillator frequency (0x8) */
  0x0a8, 0x03f,    /* */

  0x0d3, 0x000,    /*  */

  0x040,        /* start line */
  
  0x08d, 0x014,    /* [2] charge pump setting (p62): 0x014 enable, 0x010 disable */

  0x020, 0x002,    /* 2012-05-27: page addressing mode */
  0x0a1,        /* segment remap a0/a1*/
  0x0c8,        /* c0: scan dir normal, c8: reverse */
  0x0da, 0x012,    /* com pin HW config, sequential com pin config (bit 4), disable left/right remap (bit 5) */
  0x081, 0x0cf,    /* [2] set contrast control */
  0x0d9, 0x0f1,    /* [2] pre-charge period 0x022/f1*/
  0x0db, 0x040,    /* vcomh deselect level */
  
  0x02e,        /* 2012-05-27: Deactivate scroll */ 
  0x0a4,        /* output ram to display */
  0x0a6,        /* none inverted normal display mode */
  0x0af,        /* display on */

  OLED_ESC_CS(0),             /* disable chip */
  OLED_ESC_END                /* end of sequence */
};

/* init sequence Univision datasheet (NOT TESTED) */
static const uint8_t oled_dev_ssd1306_128x64_univision_init_seq[] PROGMEM = {
  OLED_ESC_CS(0),             /* disable chip */
  OLED_ESC_ADR(0),           /* instruction mode */
  OLED_ESC_RST(1),           /* do reset low pulse with (1*16)+2 milliseconds */
  OLED_ESC_CS(1),             /* enable chip */

  0x0ae,        /* display off, sleep mode */
  0x0d5, 0x080,    /* clock divide ratio (0x00=1) and oscillator frequency (0x8) */
  0x0a8, 0x03f,    /* multiplex ratio */
  0x0d3, 0x000,    /* display offset */
  0x040,        /* start line */
  0x08d, 0x010,    /* charge pump setting (p62): 0x014 enable, 0x010 disable */
  0x0a1,        /* segment remap a0/a1*/
  0x0c8,        /* c0: scan dir normal, c8: reverse */
  0x0da, 0x012,    /* com pin HW config, sequential com pin config (bit 4), disable left/right remap (bit 5) */
  0x081, 0x09f,    /* set contrast control */
  0x0d9, 0x022,    /* pre-charge period */
  0x0db, 0x040,    /* vcomh deselect level */
  0x022, 0x000,    /* page addressing mode WRONG: 3 byte cmd! */
  0x0a4,        /* output ram to display */
  0x0a6,        /* none inverted normal display mode */
  0x0af,        /* display on */
  OLED_ESC_CS(0),             /* disable chip */
  OLED_ESC_END                /* end of sequence */
};

/* select one init sequence here */
//#define oled_dev_ssd1306_128x64_init_seq oled_dev_ssd1306_128x64_univision_init_seq
//#define oled_dev_ssd1306_128x64_init_seq oled_dev_ssd1306_128x64_adafruit1_init_seq
// 26. Apr 2014: in this thead: http://forum.arduino.cc/index.php?topic=234930.msg1696754;topicseen#msg1696754
// it is mentiond, that adafruit2_init_seq works better --> this will be used by the ssd1306_adafruit device
//#define oled_dev_ssd1306_128x64_init_seq oled_dev_ssd1306_128x64_adafruit2_init_seq

#define oled_dev_ssd1306_128x64_init_seq oled_dev_ssd1306_128x64_adafruit3_init_seq


static const uint8_t oled_dev_ssd1306_128x64_data_start[] PROGMEM = {
  OLED_ESC_ADR(0),           /* instruction mode */
  OLED_ESC_CS(1),             /* enable chip */
  0x010,    /* set upper 4 bit of the col adr to 0 */
  0x000,    /* set lower 4 bit of the col adr to 0  */
  OLED_ESC_END                /* end of sequence */
};

/* the sh1106 is compatible to the ssd1306, but is 132x64. display seems to be centered */
static const uint8_t oled_dev_sh1106_128x64_data_start[] PROGMEM = {
  OLED_ESC_ADR(0),           /* instruction mode */
  OLED_ESC_CS(1),             /* enable chip */
  0x010,    /* set upper 4 bit of the col adr to 0 */
  0x002,    /* set lower 4 bit of the col adr to 2 (centered display with sh1106)  */
  OLED_ESC_END                /* end of sequence */
};

static const uint8_t oled_dev_ssd13xx_sleep_on[] PROGMEM = {
  OLED_ESC_ADR(0),           /* instruction mode */
  OLED_ESC_CS(1),             /* enable chip */
  0x0ae,    /* display off */      
  OLED_ESC_CS(0),             /* disable chip, bugfix 12 nov 2014 */
  OLED_ESC_END                /* end of sequence */
};

static const uint8_t oled_dev_ssd13xx_sleep_off[] PROGMEM = {
  OLED_ESC_ADR(0),           /* instruction mode */
  OLED_ESC_CS(1),             /* enable chip */
  0x0af,    /* display on */      
  OLED_ESC_DLY(50),       /* delay 50 ms */
  OLED_ESC_CS(0),             /* disable chip, bugfix 12 nov 2014 */
  OLED_ESC_END                /* end of sequence */
};

uint8_t oled_dev_ssd1306_128x64_fn(oled_t *oled, oled_dev_t *dev, uint8_t msg, void *arg) {
  switch(msg) {
    case OLED_DEV_MSG_INIT:
      oled_InitCom(oled, dev, OLED_SPI_CLK_CYCLE_300NS);
      oled_WriteEscSeqP(oled, dev, oled_dev_ssd1306_128x64_adafruit2_init_seq);
      break;
    case OLED_DEV_MSG_STOP:
      break;
    case OLED_DEV_MSG_PAGE_NEXT:
      {
        oled_pb_t *pb = (oled_pb_t *)(dev->dev_mem);
        oled_WriteEscSeqP(oled, dev, oled_dev_ssd1306_128x64_data_start);    
        oled_WriteByte(oled, dev, 0x0b0 | pb->p.page); /* select current page (SSD1306) */
        oled_SetAddress(oled, dev, 1);           /* data mode */
        if ( oled_pb_WriteBuffer(pb, oled, dev) == 0 )
          return 0;
        oled_SetChipSelect(oled, dev, 0);
      }
      break;
    case OLED_DEV_MSG_SLEEP_ON:
      oled_WriteEscSeqP(oled, dev, oled_dev_ssd13xx_sleep_on);    
      return 1;
    case OLED_DEV_MSG_SLEEP_OFF:
      oled_WriteEscSeqP(oled, dev, oled_dev_ssd13xx_sleep_off);    
      return 1;
    case OLED_DEV_MSG_CONTRAST:
      {
        oled_SetChipSelect(oled, dev, 1);
        oled_SetAddress(oled, dev, 0); /* instruction mode */
        oled_WriteByte(oled, dev, 0x81);
        oled_WriteByte(oled, dev, *(uint8_t *) arg);
        oled_SetChipSelect(oled, dev, 0);
        return 1;
      }
  }
  return oled_dev_pb8v1_base_fn(oled, dev, msg, arg);
}

uint8_t oled_dev_ssd1306_adafruit_128x64_fn(oled_t *oled, oled_dev_t *dev, uint8_t msg, void *arg) {
  switch(msg) {
    case OLED_DEV_MSG_INIT:
      oled_InitCom(oled, dev, OLED_SPI_CLK_CYCLE_300NS);
      oled_WriteEscSeqP(oled, dev, oled_dev_ssd1306_128x64_init_seq);
      break;
    case OLED_DEV_MSG_STOP:
      break;
    case OLED_DEV_MSG_PAGE_NEXT: 
      {
        oled_pb_t *pb = (oled_pb_t *)(dev->dev_mem);
        oled_WriteEscSeqP(oled, dev, oled_dev_ssd1306_128x64_data_start);    
        oled_WriteByte(oled, dev, 0x0b0 | pb->p.page); /* select current page (SSD1306) */
        oled_SetAddress(oled, dev, 1);           /* data mode */
        if ( oled_pb_WriteBuffer(pb, oled, dev) == 0 )
          return 0;
        oled_SetChipSelect(oled, dev, 0);
      }
      break;
    case OLED_DEV_MSG_SLEEP_ON:
      oled_WriteEscSeqP(oled, dev, oled_dev_ssd13xx_sleep_on);    
      return 1;
    case OLED_DEV_MSG_SLEEP_OFF:
      oled_WriteEscSeqP(oled, dev, oled_dev_ssd13xx_sleep_off);    
      return 1;
    case OLED_DEV_MSG_CONTRAST:
      {
        oled_SetChipSelect(oled, dev, 1);
        oled_SetAddress(oled, dev, 0); /* instruction mode */
        oled_WriteByte(oled, dev, 0x81);
        oled_WriteByte(oled, dev, *(uint8_t *) arg);
        oled_SetChipSelect(oled, dev, 0);
        return 1;
      }
  }
  return oled_dev_pb8v1_base_fn(oled, dev, msg, arg);
}

uint8_t oled_dev_sh1106_128x64_fn(oled_t *oled, oled_dev_t *dev, uint8_t msg, void *arg) {
  switch(msg) {
    case OLED_DEV_MSG_INIT:
      oled_InitCom(oled, dev, OLED_SPI_CLK_CYCLE_300NS);
      oled_WriteEscSeqP(oled, dev, oled_dev_ssd1306_128x64_init_seq);
      break;
    case OLED_DEV_MSG_STOP:
      break;
    case OLED_DEV_MSG_PAGE_NEXT:
      {
        oled_pb_t *pb = (oled_pb_t *)(dev->dev_mem);
        oled_WriteEscSeqP(oled, dev, oled_dev_sh1106_128x64_data_start);    
        oled_WriteByte(oled, dev, 0x0b0 | pb->p.page); /* select current page (SSD1306) */
        oled_SetAddress(oled, dev, 1);           /* data mode */
        if ( oled_pb_WriteBuffer(pb, oled, dev) == 0 )
          return 0;
        oled_SetChipSelect(oled, dev, 0);
      }
      break;
    case OLED_DEV_MSG_SLEEP_ON:
      oled_WriteEscSeqP(oled, dev, oled_dev_ssd13xx_sleep_on);    
      return 1;
    case OLED_DEV_MSG_SLEEP_OFF:
      oled_WriteEscSeqP(oled, dev, oled_dev_ssd13xx_sleep_off);    
      return 1;
    case OLED_DEV_MSG_CONTRAST:
      {
        oled_SetChipSelect(oled, dev, 1);
        oled_SetAddress(oled, dev, 0); /* instruction mode */
        oled_WriteByte(oled, dev, 0x81);
        oled_WriteByte(oled, dev, *(uint8_t *) arg);
        oled_SetChipSelect(oled, dev, 0);
        return 1;
      }
  }
  return oled_dev_pb8v1_base_fn(oled, dev, msg, arg);
}


uint8_t oled_dev_ssd1306_128x64_2x_fn(oled_t *oled, oled_dev_t *dev, uint8_t msg, void *arg) {
  switch(msg) {
    case OLED_DEV_MSG_INIT:
      oled_InitCom(oled, dev, OLED_SPI_CLK_CYCLE_300NS);
      oled_WriteEscSeqP(oled, dev, oled_dev_ssd1306_128x64_init_seq);
      break;
    case OLED_DEV_MSG_STOP:
      break;
    case OLED_DEV_MSG_PAGE_NEXT:
      {
        oled_pb_t *pb = (oled_pb_t *)(dev->dev_mem);
  
        oled_WriteEscSeqP(oled, dev, oled_dev_ssd1306_128x64_data_start);    
        oled_WriteByte(oled, dev, 0x0b0 | (pb->p.page*2)); /* select current page (SSD1306) */
        oled_SetAddress(oled, dev, 1);           /* data mode */
        oled_WriteSequence(oled, dev, pb->width, pb->buf); 
        oled_SetChipSelect(oled, dev, 0);
  
        oled_WriteEscSeqP(oled, dev, oled_dev_ssd1306_128x64_data_start);    
        oled_WriteByte(oled, dev, 0x0b0 | (pb->p.page*2+1)); /* select current page (SSD1306) */
        oled_SetAddress(oled, dev, 1);           /* data mode */
        oled_WriteSequence(oled, dev, pb->width, (uint8_t *)(pb->buf)+pb->width); 
        oled_SetChipSelect(oled, dev, 0);
      }
      break;
    case OLED_DEV_MSG_SLEEP_ON:
      oled_WriteEscSeqP(oled, dev, oled_dev_ssd13xx_sleep_on);    
      return 1;
    case OLED_DEV_MSG_SLEEP_OFF:
      oled_WriteEscSeqP(oled, dev, oled_dev_ssd13xx_sleep_off);    
      return 1;
    case OLED_DEV_MSG_CONTRAST:
      {
        oled_SetChipSelect(oled, dev, 1);
        oled_SetAddress(oled, dev, 0); /* instruction mode */
        oled_WriteByte(oled, dev, 0x81);
        oled_WriteByte(oled, dev, *(uint8_t *) arg);
        oled_SetChipSelect(oled, dev, 0);
        return 1;
      }
  }
  return oled_dev_pb16v1_base_fn(oled, dev, msg, arg);
}

uint8_t oled_dev_sh1106_128x64_2x_fn(oled_t *oled, oled_dev_t *dev, uint8_t msg, void *arg) {
  switch(msg) {
    case OLED_DEV_MSG_INIT:
      oled_InitCom(oled, dev, OLED_SPI_CLK_CYCLE_300NS);
      oled_WriteEscSeqP(oled, dev, oled_dev_ssd1306_128x64_init_seq);
      break;
    case OLED_DEV_MSG_STOP:
      break;
    case OLED_DEV_MSG_PAGE_NEXT:
      {
        oled_pb_t *pb = (oled_pb_t *)(dev->dev_mem);
  
        oled_WriteEscSeqP(oled, dev, oled_dev_sh1106_128x64_data_start);    
        oled_WriteByte(oled, dev, 0x0b0 | (pb->p.page*2)); /* select current page (SSD1306) */
        oled_SetAddress(oled, dev, 1);           /* data mode */
        oled_WriteSequence(oled, dev, pb->width, pb->buf); 
        oled_SetChipSelect(oled, dev, 0);
  
        oled_WriteEscSeqP(oled, dev, oled_dev_sh1106_128x64_data_start);    
        oled_WriteByte(oled, dev, 0x0b0 | (pb->p.page*2+1)); /* select current page (SSD1306) */
        oled_SetAddress(oled, dev, 1);           /* data mode */
        oled_WriteSequence(oled, dev, pb->width, (uint8_t *)(pb->buf)+pb->width); 
        oled_SetChipSelect(oled, dev, 0);
      }
      break;
    case OLED_DEV_MSG_SLEEP_ON:
      oled_WriteEscSeqP(oled, dev, oled_dev_ssd13xx_sleep_on);    
      return 1;
    case OLED_DEV_MSG_SLEEP_OFF:
      oled_WriteEscSeqP(oled, dev, oled_dev_ssd13xx_sleep_off);    
      return 1;
    case OLED_DEV_MSG_CONTRAST:
      {
        oled_SetChipSelect(oled, dev, 1);
        oled_SetAddress(oled, dev, 0); /* instruction mode */
        oled_WriteByte(oled, dev, 0x81);
        oled_WriteByte(oled, dev, *(uint8_t *) arg);
        oled_SetChipSelect(oled, dev, 0);
        return 1;
      }
  }
  return oled_dev_pb16v1_base_fn(oled, dev, msg, arg);
}




OLED_PB_DEV(oled_dev_ssd1306_128x64_sw_spi, WIDTH, HEIGHT, PAGE_HEIGHT, oled_dev_ssd1306_128x64_fn, OLED_COM_SW_SPI);
OLED_PB_DEV(oled_dev_ssd1306_128x64_hw_spi, WIDTH, HEIGHT, PAGE_HEIGHT, oled_dev_ssd1306_128x64_fn, OLED_COM_HW_SPI);
OLED_PB_DEV(oled_dev_ssd1306_128x64_i2c, WIDTH, HEIGHT, PAGE_HEIGHT, oled_dev_ssd1306_128x64_fn, OLED_COM_SSD_I2C);

OLED_PB_DEV(oled_dev_ssd1306_adafruit_128x64_sw_spi, WIDTH, HEIGHT, PAGE_HEIGHT, oled_dev_ssd1306_adafruit_128x64_fn, OLED_COM_SW_SPI);
OLED_PB_DEV(oled_dev_ssd1306_adafruit_128x64_hw_spi, WIDTH, HEIGHT, PAGE_HEIGHT, oled_dev_ssd1306_adafruit_128x64_fn, OLED_COM_HW_SPI);
OLED_PB_DEV(oled_dev_ssd1306_adafruit_128x64_i2c, WIDTH, HEIGHT, PAGE_HEIGHT, oled_dev_ssd1306_adafruit_128x64_fn, OLED_COM_SSD_I2C);


uint8_t oled_dev_ssd1306_128x64_2x_buf[WIDTH*2] OLED_NOCOMMON ; 
oled_pb_t oled_dev_ssd1306_128x64_2x_pb = { {16, HEIGHT, 0, 0, 0},  WIDTH, oled_dev_ssd1306_128x64_2x_buf}; 
oled_dev_t oled_dev_ssd1306_128x64_2x_sw_spi = { oled_dev_ssd1306_128x64_2x_fn, &oled_dev_ssd1306_128x64_2x_pb, OLED_COM_SW_SPI };
oled_dev_t oled_dev_ssd1306_128x64_2x_hw_spi = { oled_dev_ssd1306_128x64_2x_fn, &oled_dev_ssd1306_128x64_2x_pb, OLED_COM_HW_SPI };
oled_dev_t oled_dev_ssd1306_128x64_2x_i2c = { oled_dev_ssd1306_128x64_2x_fn, &oled_dev_ssd1306_128x64_2x_pb, OLED_COM_SSD_I2C };


OLED_PB_DEV(oled_dev_sh1106_128x64_sw_spi, WIDTH, HEIGHT, PAGE_HEIGHT, oled_dev_sh1106_128x64_fn, OLED_COM_SW_SPI);
OLED_PB_DEV(oled_dev_sh1106_128x64_hw_spi, WIDTH, HEIGHT, PAGE_HEIGHT, oled_dev_sh1106_128x64_fn, OLED_COM_HW_SPI);
OLED_PB_DEV(oled_dev_sh1106_128x64_i2c, WIDTH, HEIGHT, PAGE_HEIGHT, oled_dev_sh1106_128x64_fn, OLED_COM_SSD_I2C);

uint8_t oled_dev_sh1106_128x64_2x_buf[WIDTH*2] OLED_NOCOMMON ; 
oled_pb_t oled_dev_sh1106_128x64_2x_pb = { {16, HEIGHT, 0, 0, 0},  WIDTH, oled_dev_sh1106_128x64_2x_buf}; 
oled_dev_t oled_dev_sh1106_128x64_2x_sw_spi = { oled_dev_sh1106_128x64_2x_fn, &oled_dev_sh1106_128x64_2x_pb, OLED_COM_SW_SPI };
oled_dev_t oled_dev_sh1106_128x64_2x_hw_spi = { oled_dev_sh1106_128x64_2x_fn, &oled_dev_sh1106_128x64_2x_pb, OLED_COM_HW_SPI };
oled_dev_t oled_dev_sh1106_128x64_2x_i2c = { oled_dev_sh1106_128x64_2x_fn, &oled_dev_sh1106_128x64_2x_pb, OLED_COM_SSD_I2C };



/*======================================== DELAY =======================================*/
/*======================================================================================*/
void oled_Delay(uint16_t val) {
	delay(val);
}
void oled_MicroDelay(void) {
	delayMicroseconds(1);
}
void oled_10MicroDelay(void) {
	delayMicroseconds(10);
}


/*======================================== pb8v1 =======================================*/
/*======================================================================================*/
void u8g_pb8v1_Init(u8g_pb_t *b, void *buf, u8g_uint_t width) U8G_NOINLINE;
void u8g_pb8v1_set_pixel(u8g_pb_t *b, u8g_uint_t x, u8g_uint_t y, uint8_t color_index) U8G_NOINLINE;
void u8g_pb8v1_SetPixel(u8g_pb_t *b, const u8g_dev_arg_pixel_t * const arg_pixel) U8G_NOINLINE ;
void u8g_pb8v1_Set8PixelStd(u8g_pb_t *b, u8g_dev_arg_pixel_t *arg_pixel) U8G_NOINLINE;

/* Obsolete, usually set by the init of the structure */
void u8g_pb8v1_Init(u8g_pb_t *b, void *buf, u8g_uint_t width)
{
  b->buf = buf;
  b->width = width;
  u8g_pb_Clear(b);
}

void u8g_pb8v1_set_pixel(u8g_pb_t *b, u8g_uint_t x, u8g_uint_t y, uint8_t color_index)
{
  register uint8_t mask;
  uint8_t *ptr = b->buf;
  
  y -= b->p.page_y0;
  mask = 1;
  y &= 0x07;
  mask <<= y;
  ptr += x;
  if ( color_index )
  {
    *ptr |= mask;
  }
  else
  {
    mask ^=0xff;
    *ptr &= mask;
  }
}


void u8g_pb8v1_SetPixel(u8g_pb_t *b, const u8g_dev_arg_pixel_t * const arg_pixel)
{
  if ( arg_pixel->y < b->p.page_y0 )
    return;
  if ( arg_pixel->y > b->p.page_y1 )
    return;
  if ( arg_pixel->x >= b->width )
    return;
  u8g_pb8v1_set_pixel(b, arg_pixel->x, arg_pixel->y, arg_pixel->color);
}

void u8g_pb8v1_Set8PixelStd(u8g_pb_t *b, u8g_dev_arg_pixel_t *arg_pixel)
{
  register uint8_t pixel = arg_pixel->pixel;
  do
  {
    if ( pixel & 128 )
    {
      u8g_pb8v1_SetPixel(b, arg_pixel);
    }
    switch( arg_pixel->dir )
    {
      case 0: arg_pixel->x++; break;
      case 1: arg_pixel->y++; break;
      case 2: arg_pixel->x--; break;
      case 3: arg_pixel->y--; break;
    }
    pixel <<= 1;
  } while( pixel != 0  );
}


void u8g_pb8v1_Set8PixelOpt2(u8g_pb_t *b, u8g_dev_arg_pixel_t *arg_pixel)
{
  register uint8_t pixel = arg_pixel->pixel;
  u8g_uint_t dx = 0;
  u8g_uint_t dy = 0;
  
  switch( arg_pixel->dir )
  {
    case 0: dx++; break;
    case 1: dy++; break;
    case 2: dx--; break;
    case 3: dy--; break;
  }
  
  do
  {
    if ( pixel & 128 )
      u8g_pb8v1_SetPixel(b, arg_pixel);
    arg_pixel->x += dx;
    arg_pixel->y += dy;
    pixel <<= 1;
  } while( pixel != 0  );
  
}

uint8_t u8g_dev_pb8v1_base_fn(u8g_t *u8g, u8g_dev_t *dev, uint8_t msg, void *arg)
{
  u8g_pb_t *pb = (u8g_pb_t *)(dev->dev_mem);
  switch(msg)
  {
    case U8G_DEV_MSG_SET_8PIXEL:
      if ( u8g_pb_Is8PixelVisible(pb, (u8g_dev_arg_pixel_t *)arg) )
        u8g_pb8v1_Set8PixelOpt2(pb, (u8g_dev_arg_pixel_t *)arg);
      break;
    case U8G_DEV_MSG_SET_PIXEL:
        u8g_pb8v1_SetPixel(pb, (u8g_dev_arg_pixel_t *)arg);
      break;
    case U8G_DEV_MSG_INIT:
      break;
    case U8G_DEV_MSG_STOP:
      break;
    case U8G_DEV_MSG_PAGE_FIRST:
      u8g_pb_Clear(pb);
      u8g_page_First(&(pb->p));
      break;
    case U8G_DEV_MSG_PAGE_NEXT:
      if ( u8g_page_Next(&(pb->p)) == 0 )
        return 0;
      u8g_pb_Clear(pb);
      break;
#ifdef U8G_DEV_MSG_IS_BBX_INTERSECTION
    case U8G_DEV_MSG_IS_BBX_INTERSECTION:
      return u8g_pb_IsIntersection(pb, (u8g_dev_arg_bbx_t *)arg);
#endif
    case U8G_DEV_MSG_GET_PAGE_BOX:
      u8g_pb_GetPageBox(pb, (u8g_box_t *)arg);
      break;
    case U8G_DEV_MSG_GET_WIDTH:
      *((u8g_uint_t *)arg) = pb->width;
      break;
    case U8G_DEV_MSG_GET_HEIGHT:
      *((u8g_uint_t *)arg) = pb->p.total_height;
      break;
    case U8G_DEV_MSG_SET_COLOR_ENTRY:
      break;
    case U8G_DEV_MSG_SET_XY_CB:
      break;
    case U8G_DEV_MSG_GET_MODE:
      return U8G_MODE_BW;
  }
  return 1;
}
 

/*======================================== Circle ======================================*/
/*======================================================================================*/


static void oled_draw_circle_section(oled_t *oled, uint8_t x, uint8_t y, uint8_t x0, uint8_t y0, uint8_t option) OLED_NOINLINE;

static void oled_draw_circle_section(oled_t *oled, uint8_t x, uint8_t y, uint8_t x0, uint8_t y0, uint8_t option) {
  if ( option & OLED_DRAW_UPPER_RIGHT ) {  /* upper right */
    oled_DrawPixel(oled, x0 + x, y0 - y);
    oled_DrawPixel(oled, x0 + y, y0 - x);
  } else
  if ( option & OLED_DRAW_UPPER_LEFT ) {   /* upper left */
    oled_DrawPixel(oled, x0 - x, y0 - y);
    oled_DrawPixel(oled, x0 - y, y0 - x);
  } else
  if ( option & OLED_DRAW_LOWER_RIGHT ) {  /* lower right */
    oled_DrawPixel(oled, x0 + x, y0 + y);
    oled_DrawPixel(oled, x0 + y, y0 + x);
  } else
  if ( option & OLED_DRAW_LOWER_LEFT ) {   /* lower left */
    oled_DrawPixel(oled, x0 - x, y0 + y);
    oled_DrawPixel(oled, x0 - y, y0 + x);
  }
}

void oled_draw_circle(oled_t *oled, uint8_t x0, uint8_t y0, uint8_t rad, uint8_t option) {
  int8_t f     = 1 - rad;
  int8_t ddF_x = 1;
  int8_t ddF_y = - rad*2;
  uint8_t x    = 0;
  uint8_t y    = rad;

  oled_draw_circle_section(oled, x, y, x0, y0, option);
  
  while ( x < y ) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    oled_draw_circle_section(oled, x, y, x0, y0, option);    
  }
}

void oled_DrawCircle(oled_t *oled, uint8_t x0, uint8_t y0, uint8_t rad, uint8_t option) {
  /* check for bounding box */
  {
    uint8_t radp, radp2;
    
    radp = rad;
    radp++;
    radp2 = radp;
    radp2 *= 2;
    
    if ( oled_IsBBXIntersection(oled, x0-radp, y0-radp, radp2, radp2) == 0)
      return;    
  }
  /* draw circle */
  oled_draw_circle(oled, x0, y0, rad, option);
}

static void oled_draw_disc_section(oled_t *oled, uint8_t x, uint8_t y, uint8_t x0, uint8_t y0, uint8_t option) OLED_NOINLINE;

static void oled_draw_disc_section(oled_t *oled, uint8_t x, uint8_t y, uint8_t x0, uint8_t y0, uint8_t option) {
  if ( option & OLED_DRAW_UPPER_RIGHT ) {  /* upper right */
    oled_DrawVLine(oled, x0+x, y0-y, y+1);
    oled_DrawVLine(oled, x0+y, y0-x, x+1);
  } else
  if ( option & OLED_DRAW_UPPER_LEFT ) {   /* upper left */
    oled_DrawVLine(oled, x0-x, y0-y, y+1);
    oled_DrawVLine(oled, x0-y, y0-x, x+1);
  } else
  if ( option & OLED_DRAW_LOWER_RIGHT ) {  /* lower right */
    oled_DrawVLine(oled, x0+x, y0, y+1);
    oled_DrawVLine(oled, x0+y, y0, x+1);
  } else
  if ( option & OLED_DRAW_LOWER_LEFT ) {   /* lower left */
    oled_DrawVLine(oled, x0-x, y0, y+1);
    oled_DrawVLine(oled, x0-y, y0, x+1);
  }
}

void oled_draw_disc(oled_t *oled, uint8_t x0, uint8_t y0, uint8_t rad, uint8_t option) {
  int8_t f     = 1 - rad;
  int8_t ddF_x = 1;
  int8_t ddF_y = - rad*2;
  uint8_t x    = 0;
  uint8_t y    = rad;

  oled_draw_disc_section(oled, x, y, x0, y0, option);
  
  while ( x < y ) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    oled_draw_disc_section(oled, x, y, x0, y0, option);    
  }
}

void oled_DrawDisc(oled_t *oled, uint8_t x0, uint8_t y0, uint8_t rad, uint8_t option) {
  /* check for bounding box */
  {
    uint8_t radp, radp2;
    
    radp = rad;
    radp++;
    radp2 = radp;
    radp2 *= 2;
    
    if ( oled_IsBBXIntersection(oled, x0-radp, y0-radp, radp2, radp2) == 0)
      return;    
  }
  
  /* draw disc */
  oled_draw_disc(oled, x0, y0, rad, option);
}


/*======================================== Bitmap ======================================*/
/*======================================================================================*/
void oled_DrawHBitmap(oled_t *oled, uint8_t x, uint8_t y, uint8_t cnt, const uint8_t *bitmap) {
  while( cnt > 0 ) {
    oled_Draw8Pixel(oled, x, y, 0, *bitmap);
    bitmap++;
    cnt--;
    x+=8;
  }
}

void oled_DrawBitmap(oled_t *oled, uint8_t x, uint8_t y, uint8_t cnt, uint8_t h, const uint8_t *bitmap) {
  if ( oled_IsBBXIntersection(oled, x, y, cnt*8, h) == 0 )
    return;
  while( h > 0 ) {
    oled_DrawHBitmap(oled, x, y, cnt, bitmap);
    bitmap += cnt;
    y++;
    h--;
  }
}


void oled_DrawHBitmapP(oled_t *oled, uint8_t x, uint8_t y, uint8_t cnt, const oled_pgm_uint8_t *bitmap) {
  while( cnt > 0 ) {
    oled_Draw8Pixel(oled, x, y, 0, oled_pgm_read(bitmap));
    bitmap++;
    cnt--;
    x+=8;
  }
}

void oled_DrawBitmapP(oled_t *oled, uint8_t x, uint8_t y, uint8_t cnt, uint8_t h, const oled_pgm_uint8_t *bitmap) {
  if ( oled_IsBBXIntersection(oled, x, y, cnt*8, h) == 0 )
    return;
  while( h > 0 ) {
    oled_DrawHBitmapP(oled, x, y, cnt, bitmap);
    bitmap += cnt;
    y++;
    h--;
  }
}

/*=========================================================================*/

static void oled_DrawHXBM(oled_t *oled, uint8_t x, uint8_t y, uint8_t w, const uint8_t *bitmap) {
  uint8_t d;
  x+=7;
  while( w >= 8 ) {
    oled_Draw8Pixel(oled, x, y, 2, *bitmap);
    bitmap++;
    w-= 8;
    x+=8;
  }
  if ( w > 0 ) {
    d = *bitmap;
    x -= 7;
    do {
      if ( d & 1 )
        oled_DrawPixel(oled, x, y);
      x++;
      w--;
      d >>= 1;      
    } while ( w > 0 );
  }
}

void oled_DrawXBM(oled_t *oled, uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bitmap) {
  uint8_t b;
  b = w;
  b += 7;
  b >>= 3;
  
  if ( oled_IsBBXIntersection(oled, x, y, w, h) == 0 )
    return;
  
  while( h > 0 ) {
    oled_DrawHXBM(oled, x, y, w, bitmap);
    bitmap += b;
    y++;
    h--;
  }
}

static void oled_DrawHXBMP(oled_t *oled, uint8_t x, uint8_t y, uint8_t w, const oled_pgm_uint8_t *bitmap) {
  uint8_t d;
  x+=7;
  while( w >= 8 ) {
    oled_Draw8Pixel(oled, x, y, 2, oled_pgm_read(bitmap));
    bitmap++;
    w-= 8;
    x+=8;
  }
  if ( w > 0 ) {
    d = oled_pgm_read(bitmap);
    x -= 7;
    do {
      if ( d & 1 )
        oled_DrawPixel(oled, x, y);
      x++;
      w--;
      d >>= 1;      
    } while ( w > 0 );
  }
}

void oled_DrawXBMP(oled_t *oled, uint8_t x, uint8_t y, uint8_t w, uint8_t h, const oled_pgm_uint8_t *bitmap) {
  uint8_t b;
  b = w;
  b += 7;
  b >>= 3;
  
  if ( oled_IsBBXIntersection(oled, x, y, w, h) == 0 )
    return;
  while( h > 0 ) {
    oled_DrawHXBMP(oled, x, y, w, bitmap);
    bitmap += b;
    y++;
    h--;
  }
}



/*===================================== Rectangle ======================================*/
/*======================================================================================*/

void oled_draw_hline(oled_t *oled, uint8_t x, uint8_t y, uint8_t w) {
  uint8_t pixel = 0x0ff;
  while( w >= 8 ) {
    oled_Draw8Pixel(oled, x, y, 0, pixel);
    w-=8;
    x+=8;
  }
  if ( w != 0 ) {
    w ^=7;
    w++;
    pixel <<= w&7;
    oled_Draw8Pixel(oled, x, y, 0, pixel);
  }
}

void oled_draw_vline(oled_t *oled, uint8_t x, uint8_t y, uint8_t h) {
  uint8_t pixel = 0x0ff;
  while( h >= 8 ) {
    oled_Draw8Pixel(oled, x, y, 1, pixel);
    h-=8;
    y+=8;
  }
  if ( h != 0 ) {
    h ^=7;
    h++;
    pixel <<= h&7;
    oled_Draw8Pixel(oled, x, y, 1, pixel);
  }
}

void oled_DrawHLine(oled_t *oled, uint8_t x, uint8_t y, uint8_t w) {
  if ( oled_IsBBXIntersection(oled, x, y, w, 1) == 0 )
    return;
  oled_draw_hline(oled, x, y, w);
}

void oled_DrawVLine(oled_t *oled, uint8_t x, uint8_t y, uint8_t w) {
  if ( oled_IsBBXIntersection(oled, x, y, 1, w) == 0 )
    return;
  oled_draw_vline(oled, x, y, w);
}

/* restrictions: w > 0 && h > 0 */
void oled_DrawFrame(oled_t *oled, uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
  uint8_t xtmp = x;
  
  if ( oled_IsBBXIntersection(oled, x, y, w, h) == 0 )
    return;

  oled_draw_hline(oled, x, y, w);
  oled_draw_vline(oled, x, y, h);
  x+=w;
  x--;
  oled_draw_vline(oled, x, y, h);
  y+=h;
  y--;
  oled_draw_hline(oled, xtmp, y, w);
}

void oled_draw_box(oled_t *oled, uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
  do {
    oled_draw_hline(oled, x, y, w);
    y++;    
    h--;
  } while( h != 0 );
}

/* restrictions: h > 0 */
void oled_DrawBox(oled_t *oled, uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
  if ( oled_IsBBXIntersection(oled, x, y, w, h) == 0 )
    return;
  oled_draw_box(oled, x, y, w, h);
}


void oled_DrawRFrame(oled_t *oled, uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r) {
  uint8_t xl, yu;

  if ( oled_IsBBXIntersection(oled, x, y, w, h) == 0 )
    return;

  xl = x;
  xl += r;
  yu = y;
  yu += r;
 
  {
    uint8_t yl, xr;
      
    xr = x;
    xr += w;
    xr -= r;
    xr -= 1;
    
    yl = y;
    yl += h;
    yl -= r; 
    yl -= 1;

    oled_draw_circle(oled, xl, yu, r, OLED_DRAW_UPPER_LEFT);
    oled_draw_circle(oled, xr, yu, r, OLED_DRAW_UPPER_RIGHT);
    oled_draw_circle(oled, xl, yl, r, OLED_DRAW_LOWER_LEFT);
    oled_draw_circle(oled, xr, yl, r, OLED_DRAW_LOWER_RIGHT);
  }

  {
    uint8_t ww, hh;

    ww = w;
    ww -= r;
    ww -= r;
    ww -= 2;
    hh = h;
    hh -= r;
    hh -= r;
    hh -= 2;
    
    xl++;
    yu++;
    h--;
    w--;
    oled_draw_hline(oled, xl, y, ww);
    oled_draw_hline(oled, xl, y+h, ww);
    oled_draw_vline(oled, x,         yu, hh);
    oled_draw_vline(oled, x+w, yu, hh);
  }
}

void oled_DrawRBox(oled_t *oled, uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r) {
  uint8_t xl, yu;
  uint8_t yl, xr;

  if ( oled_IsBBXIntersection(oled, x, y, w, h) == 0 )
    return;

  xl = x;
  xl += r;
  yu = y;
  yu += r;
 
  xr = x;
  xr += w;
  xr -= r;
  xr -= 1;
  
  yl = y;
  yl += h;
  yl -= r; 
  yl -= 1;

  oled_draw_disc(oled, xl, yu, r, OLED_DRAW_UPPER_LEFT);
  oled_draw_disc(oled, xr, yu, r, OLED_DRAW_UPPER_RIGHT);
  oled_draw_disc(oled, xl, yl, r, OLED_DRAW_LOWER_LEFT);
  oled_draw_disc(oled, xr, yl, r, OLED_DRAW_LOWER_RIGHT);

  {
    uint8_t ww, hh;

    ww = w;
    ww -= r;
    ww -= r;
    ww -= 2;
    hh = h;
    hh -= r;
    hh -= r;
    hh -= 2;
    
    xl++;
    yu++;
    h--;
    oled_draw_box(oled, xl, y, ww, r+1);
    oled_draw_box(oled, xl, yl, ww, r+1);
    //oled_draw_hline(oled, xl, y+h, ww);
    oled_draw_box(oled, x, yu, w, hh);
    //oled_draw_vline(oled, x+w, yu, hh);
  }
}



/*======================================= Polygon ======================================*/
/*======================================================================================*/
/* procedures, which should not be inlined (save as much flash ROM as possible */

static uint8_t pge_Next(struct pg_edge_struct *pge) PG_NOINLINE;
static uint8_t pg_inc(pg_struct *pg, uint8_t i) PG_NOINLINE;
static uint8_t pg_dec(pg_struct *pg, uint8_t i) PG_NOINLINE;
static void pg_expand_min_y(pg_struct *pg, pg_word_t min_y, uint8_t pge_idx) PG_NOINLINE;
static void pg_line_init(pg_struct * const pg, uint8_t pge_index) PG_NOINLINE;

/*===========================================*/
/* line draw algorithm */

static uint8_t pge_Next(struct pg_edge_struct *pge) {
  if ( pge->current_y >= pge->max_y )
    return 0;
  
  pge->current_x += pge->current_x_offset;
  pge->error += pge->error_offset;
  if ( pge->error > 0 ) {
    pge->current_x += pge->x_direction;
    pge->error -= pge->height;
  }  
  
  pge->current_y++;
  return 1;
}

/* assumes y2 > y1 */
static void pge_Init(struct pg_edge_struct *pge, pg_word_t x1, pg_word_t y1, pg_word_t x2, pg_word_t y2) {
  pg_word_t dx = x2 - x1;
  pg_word_t width;

  pge->height = y2 - y1;
  pge->max_y = y2;
  pge->current_y = y1;
  pge->current_x = x1;

  if ( dx >= 0 ) {
    pge->x_direction = 1;
    width = dx;
    pge->error = 0;
  }
  else {
    pge->x_direction = -1;
    width = -dx;
    pge->error = 1 - pge->height;
  }
  
  pge->current_x_offset = dx / pge->height;
  pge->error_offset = width % pge->height;
}

/*===========================================*/
/* convex polygon algorithm */

static uint8_t pg_inc(pg_struct *pg, uint8_t i) {
  i++;
  if ( i >= pg->cnt )
    i = 0;
  return i;
}

static uint8_t pg_dec(pg_struct *pg, uint8_t i) {
  i--;
  if ( i >= pg->cnt )
    i = pg->cnt-1;
  return i;
}

static void pg_expand_min_y(pg_struct *pg, pg_word_t min_y, uint8_t pge_idx) {
  uint8_t i = pg->pge[pge_idx].curr_idx;
  for(;;) {
    i = pg->pge[pge_idx].next_idx_fn(pg, i);
    if ( pg->list[i].y != min_y )
      break;	
    pg->pge[pge_idx].curr_idx = i;
  }
}

static uint8_t pg_prepare(pg_struct *pg) {
  pg_word_t max_y;
  pg_word_t min_y;
  uint8_t i;

  /* setup the next index procedures */
  pg->pge[PG_RIGHT].next_idx_fn = pg_inc;
  pg->pge[PG_LEFT].next_idx_fn = pg_dec;
  
  /* search for highest and lowest point */
  max_y = pg->list[0].y;
  min_y = pg->list[0].y;
  pg->pge[PG_LEFT].curr_idx = 0;
  for( i = 1; i < pg->cnt; i++ ) {
    if ( max_y < pg->list[i].y ) {
      max_y = pg->list[i].y;
    }
    if ( min_y > pg->list[i].y ) {
      pg->pge[PG_LEFT].curr_idx = i;
      min_y = pg->list[i].y;
    }
  }

  /* calculate total number of scan lines */
  pg->total_scan_line_cnt = max_y;
  pg->total_scan_line_cnt -= min_y;
  
  /* exit if polygon height is zero */
  if ( pg->total_scan_line_cnt == 0 )
    return 0;
  
  /* if the minimum y side is flat, try to find the lowest and highest x points */
  pg->pge[PG_RIGHT].curr_idx = pg->pge[PG_LEFT].curr_idx;  
  pg_expand_min_y(pg, min_y, PG_RIGHT);
  pg_expand_min_y(pg, min_y, PG_LEFT);
  
  /* check if the min side is really flat (depends on the x values) */
  pg->is_min_y_not_flat = 1;
  if ( pg->list[pg->pge[PG_LEFT].curr_idx].x != pg->list[pg->pge[PG_RIGHT].curr_idx].x ) {
    pg->is_min_y_not_flat = 0;
  }
  else {
    pg->total_scan_line_cnt--;
    if ( pg->total_scan_line_cnt == 0 )
      return 0;
  }

  return 1;
}

static void pg_hline(pg_struct *pg, oled_t *oled) {
  pg_word_t x1, x2, y;
  x1 = pg->pge[PG_LEFT].current_x;
  x2 = pg->pge[PG_RIGHT].current_x;
  y = pg->pge[PG_RIGHT].current_y;
  
  if ( y < 0 )
    return;
  if ( y >= oled_GetHeight(oled) )
    return;
  if ( x1 < x2 ) {
    if ( x2 < 0 )
      return;
    if ( x1 >= oled_GetWidth(oled) )
      return;
    if ( x1 < 0 )
      x1 = 0;
    if ( x2 >= oled_GetWidth(oled) )
      x2 = oled_GetWidth(oled);
    oled_DrawHLine(oled, x1, y, x2 - x1);
  }
  else {
    if ( x1 < 0 )
      return;
    if ( x2 >= oled_GetWidth(oled) )
      return;
    if ( x2 < 0 )
      x1 = 0;
    if ( x1 >= oled_GetWidth(oled) )
      x1 = oled_GetWidth(oled);
    oled_DrawHLine(oled, x2, y, x1 - x2);
  }
}

static void pg_line_init(pg_struct * pg, uint8_t pge_index) {
  struct pg_edge_struct  *pge = pg->pge+pge_index;
  uint8_t idx;  
  pg_word_t x1;
  pg_word_t y1;
  pg_word_t x2;
  pg_word_t y2;

  idx = pge->curr_idx;  
  y1 = pg->list[idx].y;
  x1 = pg->list[idx].x;
  idx = pge->next_idx_fn(pg, idx);
  y2 = pg->list[idx].y;
  x2 = pg->list[idx].x; 
  pge->curr_idx = idx;
  
  pge_Init(pge, x1, y1, x2, y2);
}

static void pg_exec(pg_struct *pg, oled_t *oled) {
  pg_word_t i = pg->total_scan_line_cnt;

  /* first line is skipped if the min y line is not flat */
  pg_line_init(pg, PG_LEFT);		
  pg_line_init(pg, PG_RIGHT);
  
  if ( pg->is_min_y_not_flat != 0 ) {
    pge_Next(&(pg->pge[PG_LEFT])); 
    pge_Next(&(pg->pge[PG_RIGHT]));
  }

  do {
    pg_hline(pg, oled);
    while ( pge_Next(&(pg->pge[PG_LEFT])) == 0 ) {
      pg_line_init(pg, PG_LEFT);
    }
    while ( pge_Next(&(pg->pge[PG_RIGHT])) == 0 ) {
      pg_line_init(pg, PG_RIGHT);
    }
    i--;
  } while( i > 0 );
}

/*===========================================*/
/* API procedures */

void pg_ClearPolygonXY(pg_struct *pg) {
  pg->cnt = 0;
}

void pg_AddPolygonXY(pg_struct *pg, oled_t *oled, int16_t x, int16_t y) {
  if ( pg->cnt < PG_MAX_POINTS ) {
    pg->list[pg->cnt].x = x;
    pg->list[pg->cnt].y = y;
    pg->cnt++;
  }
}

void pg_DrawPolygon(pg_struct *pg, oled_t *oled) {
  if ( pg_prepare(pg) == 0 )
    return;
  pg_exec(pg, oled);
}

pg_struct oled_pg;

void oled_ClearPolygonXY(void) {
  pg_ClearPolygonXY(&oled_pg);
}

void oled_AddPolygonXY(oled_t *oled, int16_t x, int16_t y) {
  pg_AddPolygonXY(&oled_pg, oled, x, y);
}

void oled_DrawPolygon(oled_t *oled) {
  pg_DrawPolygon(&oled_pg, oled);
}

void oled_DrawTriangle(oled_t *oled, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
  oled_ClearPolygonXY();
  oled_AddPolygonXY(oled, x0, y0);
  oled_AddPolygonXY(oled, x1, y1);
  oled_AddPolygonXY(oled, x2, y2);
  oled_DrawPolygon(oled);
}


/*======================================== Bitmap ======================================*/
/*======================================================================================*/



/*======================================== Bitmap ======================================*/
/*======================================================================================*/



/*======================================== Bitmap ======================================*/
/*======================================================================================*/