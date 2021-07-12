/*
  oled.h
  Universal 8bit Graphics Library
  Copyright (c)
  All rights reserved.
*/

#ifndef _OLED_H
#define _OLED_H

/* comment the following line to generate more compact but interrupt unsafe code */
#define OLED_INTERRUPT_SAFE 1

#include <stddef.h>
#include <Arduino.h>

#ifdef __18CXX
typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef signed short int16_t;
#else
#include <stdint.h>
#endif

#if defined(__AVR__)
#include <avr/pgmspace.h>
#endif 

/* 
  use the com interface directly on any systems which are not AVR or ARDUINO 
*/
#if defined(__AVR__) || defined(ARDUINO) || defined(__MSP430__)
#define OLED_WITH_PINLIST
#endif


#ifdef __cplusplus
extern "C" {
#endif

  
/*===============================================================*/
#ifdef __GNUC__
#  define OLED_NOINLINE __attribute__((noinline))
#  define OLED_PURE  __attribute__ ((pure))
#  define OLED_NOCOMMON __attribute__ ((nocommon))
#  define OLED_SECTION(name) __attribute__ ((section (name)))
#  if defined(__MSPGCC__)
/* mspgcc does not have .progmem sections. Use -fdata-sections. */
#    define OLED_FONT_SECTION(name)
# endif
#  if defined(__AVR__)
#    define OLED_FONT_SECTION(name) OLED_SECTION(".progmem." name)
#  endif
#  if defined(__XTENSA__)
//#    define OLED_FONT_SECTION(name) OLED_SECTION(".irom.text." name)
#    define OLED_FONT_SECTION(name) OLED_SECTION(".irom0.text" )
#  endif
#else
#  define OLED_NOINLINE
#  define OLED_PURE
#  define OLED_NOCOMMON
#  define OLED_SECTION(name)
#  define OLED_FONT_SECTION(name)
#endif

#ifdef __MSP430__
/*
  Specifying a section will cause the MSP-GCC to put even const data to RAM
  at least for the fonts. But as the fonts are consts we don't need to specify
  it manually - the MSP-GCC seems to be smart enough to put it into the
  flash memory.
*/
# undef OLED_SECTION
# define OLED_SECTION(name)
#endif

/*===============================================================*/

#ifndef OLED_FONT_SECTION
#  define OLED_FONT_SECTION(name)
#endif


/*===============================================================*/
/* flash memory access */

#if defined(__AVR__)
/* OLED_PROGMEM is used by the XBM example */
#define OLED_PROGMEM OLED_SECTION(".progmem.data")
typedef uint8_t PROGMEM oled_pgm_uint8_t;
typedef uint8_t oled_fntpgm_uint8_t;
#define oled_pgm_read(adr) pgm_read_byte_near(adr)
#define OLED_PSTR(s) ((oled_pgm_uint8_t *)PSTR(s))
#endif

#if defined(__XTENSA__)
#  ifndef PROGMEM
#    define PROGMEM __attribute__ ((section (".irom0.text")))
#  endif
#  define OLED_PROGMEM PROGMEM
typedef uint8_t oled_pgm_uint8_t;
typedef uint8_t oled_fntpgm_uint8_t;
#  define oled_pgm_read(adr) (*(const oled_pgm_uint8_t *)(adr)) 
#  define OLED_PSTR(s) ((oled_pgm_uint8_t *)(s))
#endif

#ifndef OLED_PROGMEM
#  define OLED_PROGMEM
#  define PROGMEM
typedef uint8_t oled_pgm_uint8_t;
typedef uint8_t oled_fntpgm_uint8_t;
#  define oled_pgm_read(adr) (*(const oled_pgm_uint8_t *)(adr)) 
#  define OLED_PSTR(s) ((oled_pgm_uint8_t *)(s))
#endif
  
/*===============================================================*/
/* interrupt safe code */
#if defined(OLED_INTERRUPT_SAFE)
#  if defined(__AVR__)
extern uint8_t global_SREG_backup;	/* oled_state.c */
#    define OLED_ATOMIC_START()		do { global_SREG_backup = SREG; cli(); } while(0)
#    define OLED_ATOMIC_END()			SREG = global_SREG_backup
#    define OLED_ATOMIC_OR(ptr, val) 	do { uint8_t tmpSREG = SREG; cli(); (*(ptr) |= (val)); SREG = tmpSREG; } while(0)
#    define OLED_ATOMIC_AND(ptr, val) 	do { uint8_t tmpSREG = SREG; cli(); (*(ptr) &= (val)); SREG = tmpSREG; } while(0)
#  else
#    define OLED_ATOMIC_OR(ptr, val) (*(ptr) |= (val))
#    define OLED_ATOMIC_AND(ptr, val) (*(ptr) &= (val))
#    define OLED_ATOMIC_START()
#    define OLED_ATOMIC_END()
#  endif /* __AVR__ */
#else
#  define OLED_ATOMIC_OR(ptr, val) (*(ptr) |= (val))
#  define OLED_ATOMIC_AND(ptr, val) (*(ptr) &= (val))
#  define OLED_ATOMIC_START()
#  define OLED_ATOMIC_END()
#endif /* OLED_INTERRUPT_SAFE */
  
  
/*===============================================================*/
/* forward */
typedef struct _oled_t oled_t;
typedef struct _oled_dev_t oled_dev_t;

typedef struct _oled_dev_arg_pixel_t oled_dev_arg_pixel_t;
typedef struct _oled_dev_arg_bbx_t oled_dev_arg_bbx_t;
typedef struct _oled_box_t oled_box_t;
typedef struct _oled_dev_arg_irgb_t oled_dev_arg_irgb_t;


/*===============================================================*/
/* generic */
#ifdef OBSOLETE
struct _oled_box_t {
  uint8_t x0, y0, x1, y1;  
};
typedef struct _oled_box_t oled_box_t;
#endif /* OBSOLETE */


/*===============================================================*/
/* device structure */

#ifdef __XC8
/* device prototype */
typedef uint8_t (*oled_dev_fnptr)(void *oled, void *dev, uint8_t msg, void *arg);

/* com prototype */
typedef uint8_t (*oled_com_fnptr)(void *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);
#else
/* device prototype */
typedef uint8_t (*oled_dev_fnptr)(oled_t *oled, oled_dev_t *dev, uint8_t msg, void *arg);

/* com prototype */
typedef uint8_t (*oled_com_fnptr)(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);
#endif





struct _oled_dev_t {
  oled_dev_fnptr dev_fn;         /* device procedure */
  void *dev_mem;                /* device memory */
  oled_com_fnptr com_fn;         /* communication procedure */
};


/*===============================================================*/
/* device list */


/* OLED 128x64 Display with SSD1306 Controller */
extern oled_dev_t oled_dev_ssd1306_128x64_sw_spi;
extern oled_dev_t oled_dev_ssd1306_128x64_hw_spi;
extern oled_dev_t oled_dev_ssd1306_128x64_i2c;

extern oled_dev_t oled_dev_ssd1306_adafruit_128x64_sw_spi;
extern oled_dev_t oled_dev_ssd1306_adafruit_128x64_hw_spi;
extern oled_dev_t oled_dev_ssd1306_adafruit_128x64_i2c;

extern oled_dev_t oled_dev_ssd1306_128x64_2x_sw_spi;
extern oled_dev_t oled_dev_ssd1306_128x64_2x_hw_spi;
extern oled_dev_t oled_dev_ssd1306_128x64_2x_i2c;



/*===============================================================*/
/* device messages */

struct _oled_dev_arg_pixel_t {
  uint8_t x, y;              /* will be modified */
  uint8_t pixel;                  /* will be modified, pixel sequence or transparency value */
  uint8_t dir;
  uint8_t color;			/* color or index value, red value for true color mode */
  uint8_t hi_color;		/* high byte for 64K color mode, low byte is in "color", green value for true color mode */
  uint8_t blue;			/* blue value in true color mode */
};
/* typedef struct _oled_dev_arg_pixel_t oled_dev_arg_pixel_t; */ /* forward decl */

/* range for r,g,b: 0..255 */
#define OLED_GET_HICOLOR_BY_RGB(r,g,b) (((uint16_t)((r)&0x0f8))<<8)|(((uint16_t)((g)&0x0fc))<<3)|(((uint16_t)((b)>>3)))

struct _oled_dev_arg_bbx_t {
  uint8_t x, y, w, h;
};
/* typedef struct _oled_dev_arg_bbx_t oled_dev_arg_bbx_t; */ /* forward decl */

struct _oled_box_t {
  uint8_t x0, y0, x1, y1;
};
/* typedef struct _oled_box_t oled_box_t; */ /* forward decl */

struct _oled_dev_arg_irgb_t {
  uint8_t idx, r, g, b;		/* index with rgb value */
};
/* typedef struct _oled_dev_arg_irgb_t oled_dev_arg_irgb_t; */ /* forward decl */


#define OLED_DEV_MSG_INIT                10
#define OLED_DEV_MSG_STOP                11

/* arg: pointer to uint8_t, contranst value between 0 and 255 */
#define OLED_DEV_MSG_CONTRAST            15

#define OLED_DEV_MSG_SLEEP_ON            16
#define OLED_DEV_MSG_SLEEP_OFF           17

#define OLED_DEV_MSG_PAGE_FIRST          20
#define OLED_DEV_MSG_PAGE_NEXT           21

/* arg: oled_dev_arg_bbx_t * */
/* new algorithm with OLED_DEV_MSG_GET_PAGE_BOX makes this msg obsolete */
/* #define OLED_DEV_MSG_IS_BBX_INTERSECTION 22 */

/* arg: oled_box_t *, fill structure with current page properties */
#define OLED_DEV_MSG_GET_PAGE_BOX 23

/*
#define OLED_DEV_MSG_PRIMITIVE_START     30
#define OLED_DEV_MSG_PRIMITIVE_END       31
*/

/* arg: oled_dev_arg_pixel_t * */
#define OLED_DEV_MSG_SET_TPIXEL				  44
#define OLED_DEV_MSG_SET_4TPIXEL			    45

#define OLED_DEV_MSG_SET_PIXEL           50
#define OLED_DEV_MSG_SET_8PIXEL          59

#define OLED_DEV_MSG_SET_COLOR_ENTRY     60

#define OLED_DEV_MSG_SET_XY_CB           61

#define OLED_DEV_MSG_GET_WIDTH           70
#define OLED_DEV_MSG_GET_HEIGHT          71
#define OLED_DEV_MSG_GET_MODE            72

/*===============================================================*/
/* device modes */
#define OLED_MODE(is_index_mode, is_color, bits_per_pixel) (((is_index_mode)<<6) | ((is_color)<<5)|(bits_per_pixel))

#define OLED_MODE_UNKNOWN    0
#define OLED_MODE_BW         OLED_MODE(0, 0, 1)
#define OLED_MODE_GRAY2BIT   OLED_MODE(0, 0, 2)
#define OLED_MODE_R3G3B2     OLED_MODE(0, 1, 8)
#define OLED_MODE_INDEX      OLED_MODE(1, 1, 8)
/* hicolor is R5G6B5 */
#define OLED_MODE_HICOLOR    OLED_MODE(0, 1, 16)
/* truecolor  */
#define OLED_MODE_TRUECOLOR  OLED_MODE(0, 1, 24)


#define OLED_MODE_GET_BITS_PER_PIXEL(mode) ((mode)&31)
#define OLED_MODE_IS_COLOR(mode)           (((mode)&32)==0?0:1)
#define OLED_MODE_IS_INDEX_MODE(mode)      (((mode)&64)==0?0:1)


/*===============================================================*/
/* com options */

/* uncomment the following line for Atmega HW SPI double speed, issue 89 */
/* #define OLED_HW_SPI_2X 1 */

/* com messages */

#define OLED_COM_MSG_STOP        0
#define OLED_COM_MSG_INIT        1

#define OLED_COM_MSG_ADDRESS     2

/* CHIP_SELECT argument: number of the chip which needs to be activated, so this is more like high active */
#define OLED_COM_MSG_CHIP_SELECT 3

#define OLED_COM_MSG_RESET       4

#define OLED_COM_MSG_WRITE_BYTE  5
#define OLED_COM_MSG_WRITE_SEQ   6
#define OLED_COM_MSG_WRITE_SEQ_P 7


/* com driver */

uint8_t oled_com_null_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);               /* oled_com_null.c */

uint8_t oled_com_std_sw_spi_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);	        /* requires OLED_WITH_PINLIST */


uint8_t oled_com_arduino_std_sw_spi_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);     /* oled_com_arduino_std_sw_spi.c */
uint8_t oled_com_arduino_hw_usart_spi_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);   /* oled_com_atmega_hw_usart_spi.c */
uint8_t oled_com_arduino_sw_spi_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);         /* oled_com_arduino_sw_spi.c */
uint8_t oled_com_arduino_hw_spi_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);         /* oled_com_arduino_hw_spi.c */
uint8_t oled_com_arduino_ATtiny85_std_hw_spi_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);          /* oled_arduino_ATTiny85_std_hw_spi.c */
uint8_t oled_com_arduino_st7920_spi_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);     /* oled_com_arduino_st7920_spi.c */
uint8_t oled_com_arduino_st7920_custom_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);  /* oled_com_arduino_st7920_custom.c */
uint8_t oled_com_arduino_st7920_hw_spi_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);  /* oled_com_arduino_st7920_hw_spi.c */
uint8_t oled_com_arduino_parallel_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);           /* oled_com_arduino_parallel.c */
uint8_t oled_com_arduino_fast_parallel_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);      /* oled_com_arduino_fast_parallel.c */
uint8_t oled_com_arduino_port_d_wr_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);       /* oled_com_arduino_port_d_wr.c */
uint8_t oled_com_arduino_no_en_parallel_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);	/* oled_com_arduino_no_en_parallel.c */		
uint8_t oled_com_arduino_ssd_i2c_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);		/* oled_com_arduino_ssd_i2c.c */
uint8_t oled_com_arduino_uc_i2c_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);
uint8_t oled_com_arduino_t6963_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);			/* oled_com_arduino_t6963.c */


uint8_t oled_com_atmega_hw_spi_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);      /* oled_com_atmega_hw_spi.c */
uint8_t oled_com_atmega_sw_spi_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);      /* oled_com_atmega_sw_spi.c */
uint8_t oled_com_atmega_st7920_sw_spi_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);	/* oled_com_atmega_st7920_spi.c */
uint8_t oled_com_atmega_st7920_hw_spi_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);
uint8_t oled_com_atmega_parallel_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);    /* oled_com_atmega_parallel.c */

uint8_t oled_com_atxmega_hw_spi_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);      /* oled_com_atxmega_hw_spi.c */
uint8_t oled_com_atxmega_st7920_hw_spi_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr); /* oled_com_atxmega_st7920_spi.c */

uint8_t oled_com_msp430_hw_spi_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);      /* oled_com_msp430_hw_spi.c */

uint8_t oled_com_raspberrypi_hw_spi_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);                /* oled_com_rasperrypi_hw_spi.c */
uint8_t oled_com_raspberrypi_ssd_i2c_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);		/* oled_com_raspberrypi_ssd_i2c.c */

uint8_t oled_com_linux_ssd_i2c_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);             /* oled_com_linux_ssd_i2c.c */

uint8_t oled_com_psoc5_ssd_hw_spi_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);   /* oled_com_psoc5_ssd_hw_spi.c */
uint8_t oled_com_psoc5_ssd_hw_parallel_fn(oled_t *oled, uint8_t msg, uint8_t arg_val, void *arg_ptr);   /* oled_com_psoc5_ssd_hw_parallel.c */

/* 
  Translation of system specific com drives to generic com names
  At the moment, the following generic com drives are available
  OLED_COM_HW_SPI
  OLED_COM_SW_SPI
  OLED_COM_PARALLEL
  OLED_COM_T6963
  OLED_COM_FAST_PARALLEL
  OLED_COM_SSD_I2C
  OLED_COM_UC_I2C
  
defined(__18CXX) || defined(__PIC32MX)  

*/

/* ==== HW SPI, Arduino ====*/
#if defined(ARDUINO)
#if defined(__AVR__)

#if defined(__AVR_ATtiny85__)
#define OLED_COM_HW_SPI oled_com_arduino_ATtiny85_std_hw_spi_fn
#define OLED_COM_ST7920_HW_SPI oled_com_null_fn
#else

#define OLED_COM_HW_SPI oled_com_arduino_hw_spi_fn
#if defined(__AVR_ATmega32U4__)
#define OLED_COM_HW_USART_SPI oled_com_arduino_hw_usart_spi_fn
#endif /* __AVR_ATmega32U4__ */
#define OLED_COM_ST7920_HW_SPI oled_com_arduino_st7920_hw_spi_fn
#endif /* __AVR_ATtiny85__ */

#elif defined(__18CXX) || defined(__PIC32MX)
#define OLED_COM_HW_SPI oled_com_null_fn
#define OLED_COM_ST7920_HW_SPI oled_com_null_fn
#elif defined(__SAM3X8E__)   /* Arduino Due */
#define OLED_COM_HW_SPI oled_com_arduino_hw_spi_fn
#define OLED_COM_ST7920_HW_SPI oled_com_null_fn
#endif
#endif


/* ==== SW SPI, Arduino ====*/
#if defined(ARDUINO)
#if defined(__AVR__)
#define OLED_COM_SW_SPI oled_com_arduino_sw_spi_fn
#define OLED_COM_ST7920_SW_SPI oled_com_arduino_st7920_spi_fn
#elif defined(__18CXX) || defined(__PIC32MX)
#define OLED_COM_SW_SPI oled_com_arduino_sw_spi_fn
#define OLED_COM_ST7920_SW_SPI oled_com_arduino_st7920_spi_fn
#elif defined(__SAM3X8E__)   /* Arduino Due */
//#define OLED_COM_SW_SPI oled_com_arduino_std_sw_spi_fn
#define OLED_COM_SW_SPI oled_com_arduino_sw_spi_fn
#define OLED_COM_ST7920_SW_SPI oled_com_arduino_st7920_spi_fn
#elif defined(__arm__)   /* Teensy */
#define OLED_COM_SW_SPI oled_com_arduino_std_sw_spi_fn
#define OLED_COM_ST7920_SW_SPI oled_com_arduino_st7920_spi_fn
#endif
#endif

/* ==== Parallel interface, Arduino ====*/
#if defined(ARDUINO)
#if defined(__AVR__)
#define OLED_COM_PARALLEL oled_com_arduino_parallel_fn
#define OLED_COM_FAST_PARALLEL oled_com_arduino_fast_parallel_fn
#define OLED_COM_T6963  oled_com_arduino_t6963_fn
#else /* Arduino Due, Chipkit PIC32 */
#define OLED_COM_PARALLEL oled_com_arduino_parallel_fn
#define OLED_COM_FAST_PARALLEL oled_com_arduino_parallel_fn
#define OLED_COM_T6963  oled_com_null_fn
#endif
#endif
#ifndef OLED_COM_PARALLEL
#if defined(__AVR__)
#define OLED_COM_PARALLEL oled_com_atmega_parallel_fn
#define OLED_COM_FAST_PARALLEL oled_com_atmega_parallel_fn
#define OLED_COM_T6963  oled_com_null_fn
#endif
#endif
#ifndef OLED_COM_PARALLEL
#define OLED_COM_PARALLEL oled_com_null_fn
#define OLED_COM_FAST_PARALLEL oled_com_null_fn
#define OLED_COM_T6963  oled_com_null_fn
#endif

#if defined(ARDUINO)
#if defined(__AVR__)
#define OLED_COM_SSD_I2C oled_com_arduino_ssd_i2c_fn
#define OLED_COM_UC_I2C oled_com_arduino_uc_i2c_fn
#endif
#endif

#ifndef OLED_COM_SSD_I2C
#if defined(__AVR__) || defined(__SAM3X8E__)
/* AVR variant and also DUE can use the arduino version at the moment */
#define OLED_COM_SSD_I2C oled_com_arduino_ssd_i2c_fn
#endif
#endif

#ifndef OLED_COM_SSD_I2C
#if defined(OLED_RASPBERRY_PI)
#define OLED_COM_SSD_I2C oled_com_raspberrypi_ssd_i2c_fn
#endif
#endif
#ifndef OLED_COM_SSD_I2C
#if defined(OLED_LINUX)
#define OLED_COM_SSD_I2C oled_com_linux_ssd_i2c_fn
#endif
#endif
#if defined(OLED_CYPRESS_PSOC5)
#define OLED_COM_HW_SPI oled_com_psoc5_ssd_hw_spi_fn
#define OLED_COM_FAST_PARALLEL oled_com_psoc5_ssd_hw_parallel_fn
#endif

#ifndef OLED_COM_SSD_I2C
#define OLED_COM_SSD_I2C oled_com_null_fn
#endif

#ifndef OLED_COM_UC_I2C
#if defined(__AVR__)
/* AVR variant can use the arduino version at the moment */
#define OLED_COM_UC_I2C oled_com_arduino_uc_i2c_fn
#endif
#endif
#ifndef OLED_COM_UC_I2C
#define OLED_COM_UC_I2C oled_com_null_fn
#endif


/*===============================================================*/
/* com api */

#define OLED_SPI_CLK_CYCLE_50NS 1
#define OLED_SPI_CLK_CYCLE_300NS 2
#define OLED_SPI_CLK_CYCLE_400NS 3
#define OLED_SPI_CLK_CYCLE_NONE 255

uint8_t oled_InitCom(oled_t *oled, oled_dev_t *dev, uint8_t clk_cycle_time);
void oled_StopCom(oled_t *oled, oled_dev_t *dev);
void oled_EnableCom(oled_t *oled, oled_dev_t *dev);         /* obsolete */
void oled_DisableCom(oled_t *oled, oled_dev_t *dev);        /* obsolete */
void oled_SetChipSelect(oled_t *oled, oled_dev_t *dev, uint8_t cs);
void oled_SetResetLow(oled_t *oled, oled_dev_t *dev);
void oled_SetResetHigh(oled_t *oled, oled_dev_t *dev);
void oled_SetAddress(oled_t *oled, oled_dev_t *dev, uint8_t address);
uint8_t oled_WriteByte(oled_t *oled, oled_dev_t *dev, uint8_t val);
uint8_t oled_WriteSequence(oled_t *oled, oled_dev_t *dev, uint8_t cnt, uint8_t *seq);
uint8_t oled_WriteSequenceP(oled_t *oled, oled_dev_t *dev, uint8_t cnt, const uint8_t *seq);



#define OLED_ESC_DLY(x) 255, ((x) & 0x7f)
#define OLED_ESC_CS(x) 255, (0xd0 | ((x)&0x0f))
#define OLED_ESC_ADR(x) 255, (0xe0 | ((x)&0x0f))
#define OLED_ESC_RST(x) 255, (0xc0 | ((x)&0x0f))
#define OLED_ESC_VCC(x) 255, (0xbe | ((x)&0x01))
#define OLED_ESC_END 255, 254
#define OLED_ESC_255 255, 255
//uint8_t oled_WriteEscSeqP(oled_t *oled, oled_dev_t *dev, oled_pgm_uint8_t *esc_seq);
uint8_t oled_WriteEscSeqP(oled_t *oled, oled_dev_t *dev, const uint8_t *esc_seq);


/* oled_com_api_16gr.c */
uint8_t oled_WriteByteBWTo16GrDevice(oled_t *oled, oled_dev_t *dev, uint8_t b);
uint8_t oled_WriteSequenceBWTo16GrDevice(oled_t *oled, oled_dev_t *dev, uint8_t cnt, uint8_t *ptr);
uint8_t oled_WriteByte4LTo16GrDevice(oled_t *oled, oled_dev_t *dev, uint8_t b);
uint8_t oled_WriteSequence4LTo16GrDevice(oled_t *oled, oled_dev_t *dev, uint8_t cnt, uint8_t *ptr);


/*===============================================================*/
/* oled_arduino_common.c */
void oled_com_arduino_digital_write(oled_t *oled, uint8_t pin_index, uint8_t value);
void oled_com_arduino_assign_pin_output_high(oled_t *oled);

/*===============================================================*/
/* oled_com_io.c */

/* create internal number from port and pin */
uint8_t oled_Pin(uint8_t port, uint8_t bitpos);
#define PN(port,bitpos) oled_Pin(port,bitpos)

/* low level procedures */
void oled_SetPinOutput(uint8_t internal_pin_number);
void oled_SetPinLevel(uint8_t internal_pin_number, uint8_t level);
void oled_SetPinInput(uint8_t internal_pin_number);
uint8_t oled_GetPinLevel(uint8_t internal_pin_number);

/* oled level procedures, expect OLED_PI_xxx macro */
void oled_SetPIOutput(oled_t *oled, uint8_t pi);
void oled_SetPILevel(oled_t *oled, uint8_t pi, uint8_t level);


/*===============================================================*/
/* page */
struct _oled_page_t
{
  uint8_t page_height;
  uint8_t total_height;
  uint8_t page_y0;
  uint8_t page_y1;
  uint8_t page;
};
typedef struct _oled_page_t oled_page_t;

void oled_page_First(oled_page_t *p) OLED_NOINLINE;                                                                                        /* oled_page.c */
void oled_page_Init(oled_page_t *p, uint8_t page_height, uint8_t total_height ) OLED_NOINLINE;            /* oled_page.c */
uint8_t oled_page_Next(oled_page_t *p) OLED_NOINLINE;                                                                                   /* oled_page.c */

/*===============================================================*/
/* page buffer (pb) */

struct _oled_pb_t
{
  oled_page_t p;
  uint8_t width;		/* pixel width */
  void *buf;
};
typedef struct _oled_pb_t oled_pb_t;


/* oled_pb.c */
void oled_pb_Clear(oled_pb_t *b);
uint8_t oled_pb_IsYIntersection(oled_pb_t *pb, uint8_t v0, uint8_t v1);
uint8_t oled_pb_IsXIntersection(oled_pb_t *b, uint8_t v0, uint8_t v1);
uint8_t oled_pb_IsIntersection(oled_pb_t *pb, oled_dev_arg_bbx_t *bbx);
void oled_pb_GetPageBox(oled_pb_t *pb, oled_box_t *box);
uint8_t oled_pb_Is8PixelVisible(oled_pb_t *b, oled_dev_arg_pixel_t *arg_pixel);
uint8_t oled_pb_WriteBuffer(oled_pb_t *b, oled_t *oled, oled_dev_t *dev);

/*
  note on __attribute__ ((nocommon))
    AVR scripts often use  --gc-sections on the linker to remove unused section.
    This works fine for initialed data and text sections. In principle .bss is also
    handled, but the name##_pb definition is not removed. Reason is, that
    array definitions are placed in the COMMON section, by default
    The attribute "nocommon" removes this automatic assignment to the
    COMMON section and directly puts it into .bss. As a result, if more
    than one buffer is defined in one file, then it will be removed with --gc-sections

    .. not sure if Arduino IDE uses -fno-common... if yes, then the attribute is
    redundant.
*/
#define OLED_PB_DEV(name, width, height, page_height, dev_fn, com_fn) \
uint8_t name##_buf[width] OLED_NOCOMMON ; \
oled_pb_t name##_pb = { {page_height, height, 0, 0, 0},  width, name##_buf}; \
oled_dev_t name = { dev_fn, &name##_pb, com_fn }


void oled_pb8v1_Init(oled_pb_t *b, void *buf, uint8_t width)   OLED_NOINLINE;
void oled_pb8v1_Clear(oled_pb_t *b) OLED_NOINLINE;

uint8_t oled_pb8v1_IsYIntersection(oled_pb_t *b, uint8_t v0, uint8_t v1);
uint8_t oled_pb8v1_IsXIntersection(oled_pb_t *b, uint8_t v0, uint8_t v1);
uint8_t oled_pb8v1_WriteBuffer(oled_pb_t *b, oled_t *oled, oled_dev_t *dev);

uint8_t oled_dev_pb8v1_base_fn(oled_t *oled, oled_dev_t *dev, uint8_t msg, void *arg);

/* oled_pb16v1.c */
uint8_t oled_dev_pb16v1_base_fn(oled_t *oled, oled_dev_t *dev, uint8_t msg, void *arg);

/* oled_pb14v1.c */
uint8_t oled_dev_pb14v1_base_fn(oled_t *oled, oled_dev_t *dev, uint8_t msg, void *arg);

/* oled_pb8v2.c */
uint8_t oled_dev_pb8v2_base_fn(oled_t *oled, oled_dev_t *dev, uint8_t msg, void *arg);

/* oled_pb16v2.c (double memory of pb8v2) */
uint8_t oled_dev_pb16v2_base_fn(oled_t *oled, oled_dev_t *dev, uint8_t msg, void *arg);


/* oled_pb8h1.c */
uint8_t oled_dev_pb8h1_base_fn(oled_t *oled, oled_dev_t *dev, uint8_t msg, void *arg);

/* oled_pb16h1.c */
uint8_t oled_dev_pb16h1_base_fn(oled_t *oled, oled_dev_t *dev, uint8_t msg, void *arg);

/* oled_pb32h1.c */
uint8_t oled_dev_pb32h1_base_fn(oled_t *oled, oled_dev_t *dev, uint8_t msg, void *arg);


/* oled_pb8h2.c 8 pixel rows, byte has horzontal orientation */
uint8_t oled_dev_pb8h2_base_fn(oled_t *oled, oled_dev_t *dev, uint8_t msg, void *arg);

/* oled_pb16h2.c */
uint8_t oled_dev_pb16h2_base_fn(oled_t *oled, oled_dev_t *dev, uint8_t msg, void *arg);



/* oled_pb8h1f.c */
uint8_t oled_dev_pb8h1f_base_fn(oled_t *oled, oled_dev_t *dev, uint8_t msg, void *arg);

/* oled_pb8h8.c */
uint8_t oled_dev_pb8h8_base_fn(oled_t *oled, oled_dev_t *dev, uint8_t msg, void *arg);

/* oled_pbxh16.c */
uint8_t oled_dev_pbxh16_base_fn(oled_t *oled, oled_dev_t *dev, uint8_t msg, void *arg);

/* oled_pbxh24.c */
uint8_t oled_dev_pbxh24_base_fn(oled_t *oled, oled_dev_t *dev, uint8_t msg, void *arg);


/*===============================================================*/
/* oled_ll_api.c */

/* cursor draw callback */
typedef void (*oled_draw_cursor_fn)(oled_t *oled);

/* vertical reference point calculation callback */
typedef uint8_t (*oled_font_calc_vref_fnptr)(oled_t *oled);

/* state backup and restore procedure */
typedef void (*oled_state_cb)(uint8_t msg);


/* PI = Pin Index */

/* reset pin, usually optional */
#define OLED_PI_RESET 0

/* address / data or instruction */
#define OLED_PI_A0 1
#define OLED_PI_DI 1

/* chip select line */
#define OLED_PI_CS 2
#define OLED_PI_CS1 2
#define OLED_PI_CS2 3
/* Feb 2013: A0 state moved from 7 to 3 for t6963 controller*/
#define OLED_PI_A0_STATE 3

/* enable / clock signal */
#define OLED_PI_EN 4
#define OLED_PI_CS_STATE 4
#define OLED_PI_SCK 4
#define OLED_PI_SCL 4
#define OLED_PI_RD 4


/* data pins, shared with SPI and I2C pins */
#define OLED_PI_D0 5
#define OLED_PI_MOSI 5
#define OLED_PI_SDA 5
#define OLED_PI_D1 6
#define OLED_PI_MISO 6
#define OLED_PI_D2 7
#define OLED_PI_D3 8
#define OLED_PI_SET_A0 8
#define OLED_PI_D4 9
#define OLED_PI_D5 10
#define OLED_PI_I2C_OPTION 11
#define OLED_PI_D6 11
#define OLED_PI_D7 12

/* read/write pin, must be the last pin in the list, this means OLED_PIN_LIST_LEN =  OLED_PI_RW + 1*/
#define OLED_PI_WR 13
#define OLED_PI_RW 13 

#define OLED_PIN_LIST_LEN 14


#define OLED_PIN_DUMMY 254
#define OLED_PIN_NONE 255

#define OLED_FONT_HEIGHT_MODE_TEXT 0
#define OLED_FONT_HEIGHT_MODE_XTEXT 1
#define OLED_FONT_HEIGHT_MODE_ALL 2

struct _oled_t {
  uint8_t width;
  uint8_t height;
  
  oled_dev_t *dev;               /* first device in the device chain */
  const oled_pgm_uint8_t *font;             /* regular font for all text procedures */
  const oled_pgm_uint8_t *cursor_font;  /* special font for cursor procedures */
  uint8_t cursor_fg_color, cursor_bg_color;
  uint8_t cursor_encoding;
  uint8_t mode;                         /* display mode, one of OLED_MODE_xxx */
  uint8_t cursor_x;
  uint8_t cursor_y;
  oled_draw_cursor_fn cursor_fn;
  
  int8_t glyph_dx;
  int8_t glyph_x;
  int8_t glyph_y;
  uint8_t glyph_width;
  uint8_t glyph_height;
  
  oled_font_calc_vref_fnptr font_calc_vref;
  uint8_t font_height_mode;
  int8_t font_ref_ascent;
  int8_t font_ref_descent;
  uint8_t font_line_spacing_factor;     /* line_spacing = factor * (ascent - descent) / 64 */
  uint8_t line_spacing;
  
  oled_dev_arg_pixel_t arg_pixel;
  /* uint8_t color_index; */

#ifdef OLED_WITH_PINLIST
  uint8_t pin_list[OLED_PIN_LIST_LEN];
#endif
  
  oled_state_cb state_cb;
  
  oled_box_t current_page;		/* current box of the visible page */
};

#define oled_GetFontAscent(oled) ((oled)->font_ref_ascent)
#define oled_GetFontDescent(oled) ((oled)->font_ref_descent)
#define oled_GetFontLineSpacing(oled) ((oled)->line_spacing)

uint8_t oled_call_dev_fn(oled_t *oled, oled_dev_t *dev, uint8_t msg, void *arg);

uint8_t oled_InitLL(oled_t *oled, oled_dev_t *dev);
void oled_FirstPageLL(oled_t *oled, oled_dev_t *dev);
uint8_t oled_NextPageLL(oled_t *oled, oled_dev_t *dev);
uint8_t oled_SetContrastLL(oled_t *oled, oled_dev_t *dev, uint8_t contrast);
void oled_DrawPixelLL(oled_t *oled, oled_dev_t *dev, uint8_t x, uint8_t y);
void oled_Draw8PixelLL(oled_t *oled, oled_dev_t *dev, uint8_t x, uint8_t y, uint8_t dir, uint8_t pixel);
void oled_Draw4TPixelLL(oled_t *oled, oled_dev_t *dev, uint8_t x, uint8_t y, uint8_t dir, uint8_t pixel);
uint8_t oled_IsBBXIntersectionLL(oled_t *oled, oled_dev_t *dev, uint8_t x, uint8_t y, uint8_t w, uint8_t h);	/* obsolete */
uint8_t oled_GetWidthLL(oled_t *oled, oled_dev_t *dev);
uint8_t oled_GetHeightLL(oled_t *oled, oled_dev_t *dev);

void oled_UpdateDimension(oled_t *oled);
uint8_t oled_Begin(oled_t *oled);				/* reset device, put it into default state and call oled_UpdateDimension() */
uint8_t oled_Init(oled_t *oled, oled_dev_t *dev);   /* only usefull if the device only as hardcoded ports */
uint8_t oled_InitComFn(oled_t *oled, oled_dev_t *dev, oled_com_fnptr com_fn);	/* Init procedure for anything which is not Arduino or AVR (e.g. ARM, but not Due, which is Arduino) */

#if defined(OLED_WITH_PINLIST)
uint8_t oled_InitSPI(oled_t *oled, oled_dev_t *dev, uint8_t sck, uint8_t mosi, uint8_t cs, uint8_t a0, uint8_t reset);
uint8_t oled_InitHWSPI(oled_t *oled, oled_dev_t *dev, uint8_t cs, uint8_t a0, uint8_t reset);
uint8_t oled_InitI2C(oled_t *oled, oled_dev_t *dev, uint8_t options);	/* use OLED_I2C_OPT_NONE as options */
uint8_t oled_Init8BitFixedPort(oled_t *oled, oled_dev_t *dev, uint8_t en, uint8_t cs, uint8_t di, uint8_t rw, uint8_t reset);
uint8_t oled_Init8Bit(oled_t *oled, oled_dev_t *dev, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7, 
  uint8_t en, uint8_t cs1, uint8_t cs2, uint8_t di, uint8_t rw, uint8_t reset);
uint8_t oled_InitRW8Bit(oled_t *oled, oled_dev_t *dev, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7, 
  uint8_t cs, uint8_t a0, uint8_t wr, uint8_t rd, uint8_t reset);
#endif

void oled_FirstPage(oled_t *oled);
uint8_t oled_NextPage(oled_t *oled);
uint8_t oled_SetContrast(oled_t *oled, uint8_t contrast);
void oled_SleepOn(oled_t *oled);
void oled_SleepOff(oled_t *oled);
void oled_DrawPixel(oled_t *oled, uint8_t x, uint8_t y);
void oled_Draw8Pixel(oled_t *oled, uint8_t x, uint8_t y, uint8_t dir, uint8_t pixel);
void oled_Draw4TPixel(oled_t *oled, uint8_t x, uint8_t y, uint8_t dir, uint8_t pixel);
void oled_Draw8ColorPixel(oled_t *oled, uint8_t x, uint8_t y, uint8_t colpixel);

uint8_t oled_Stop(oled_t *oled);
void oled_SetColorEntry(oled_t *oled, uint8_t idx, uint8_t r, uint8_t g, uint8_t b);
void oled_SetColorIndex(oled_t *oled, uint8_t idx);
void oled_SetHiColor(oled_t *oled, uint16_t rgb);
void oled_SetHiColorByRGB(oled_t *oled, uint8_t r, uint8_t g, uint8_t b);
void oled_SetRGB(oled_t *oled, uint8_t r, uint8_t g, uint8_t b);
uint8_t oled_GetColorIndex(oled_t *oled);

uint8_t oled_GetDefaultForegroundColor(oled_t *oled);
void oled_SetDefaultForegroundColor(oled_t *oled);

uint8_t oled_GetDefaultBackgroundColor(oled_t *oled);
void oled_SetDefaultBackgroundColor(oled_t *oled);

uint8_t oled_GetDefaultMidColor(oled_t *oled);
void oled_SetDefaultMidColor(oled_t *oled);

#define oled_GetWidth(oled) ((oled)->width)
#define oled_GetHeight(oled) ((oled)->height)
#define oled_GetMode(oled) ((oled)->mode)
/*
  OLED_MODE_GET_BITS_PER_PIXEL(oled_GetMode(oled))
  OLED_MODE_IS_COLOR(oled_GetMode(oled)) 
*/

/* oled_state.c */
#define OLED_STATE_ENV_IDX 0
#define OLED_STATE_OLED_IDX 1
#define OLED_STATE_RESTORE 0
#define OLED_STATE_BACKUP 1
#define OLED_STATE_MSG_COMPOSE(cmd,idx) (((cmd)<<1) | (idx))

#define OLED_STATE_MSG_RESTORE_ENV OLED_STATE_MSG_COMPOSE(OLED_STATE_RESTORE,OLED_STATE_ENV_IDX)
#define OLED_STATE_MSG_BACKUP_ENV OLED_STATE_MSG_COMPOSE(OLED_STATE_BACKUP,OLED_STATE_ENV_IDX)
#define OLED_STATE_MSG_RESTORE_OLED OLED_STATE_MSG_COMPOSE(OLED_STATE_RESTORE,OLED_STATE_OLED_IDX)
#define OLED_STATE_MSG_BACKUP_OLED OLED_STATE_MSG_COMPOSE(OLED_STATE_BACKUP,OLED_STATE_OLED_IDX)

#define OLED_STATE_MSG_GET_IDX(msg) ((msg)&1)
#define OLED_STATE_MSG_IS_BACKUP(msg) ((msg)&2)



void oled_state_dummy_cb(uint8_t msg);
void oled_backup_spi(uint8_t msg);		/* backup SPI state controller */
/* backward compatible definition */
#define oled_backup_avr_spi oled_backup_spi

void oled_SetHardwareBackup(oled_t *oled, oled_state_cb backup_cb);

/* oled_clip.c */

uint8_t oled_IsBBXIntersection(oled_t *oled, uint8_t x, uint8_t y, uint8_t w, uint8_t h);


/* oled_rot.c */

void oled_UndoRotation(oled_t *oled);
void oled_SetRot90(oled_t *oled);
void oled_SetRot180(oled_t *oled);
void oled_SetRot270(oled_t *oled);

/* oled_scale.c */

void oled_UndoScale(oled_t *oled);
void oled_SetScale2x2(oled_t *oled);


/* oled_font.c */

size_t oled_font_GetSize(const void *font);
uint8_t oled_font_GetFontStartEncoding(const void *font) OLED_NOINLINE;
uint8_t oled_font_GetFontEndEncoding(const void *font) OLED_NOINLINE;

void oled_SetFont(oled_t *oled, const oled_fntpgm_uint8_t *font);

uint8_t oled_GetFontBBXWidth(oled_t *oled);
uint8_t oled_GetFontBBXHeight(oled_t *oled);
int8_t oled_GetFontBBXOffX(oled_t *oled);
int8_t oled_GetFontBBXOffY(oled_t *oled);
uint8_t oled_GetFontCapitalAHeight(oled_t *oled);

uint8_t oled_IsGlyph(oled_t *oled, uint8_t requested_encoding);
int8_t oled_GetGlyphDeltaX(oled_t *oled, uint8_t requested_encoding);

int8_t oled_draw_glyph(oled_t *oled, uint8_t x, uint8_t y, uint8_t encoding); /* used by oled_cursor.c */

int8_t oled_DrawGlyphDir(oled_t *oled, uint8_t x, uint8_t y, uint8_t dir, uint8_t encoding);
int8_t oled_DrawGlyph(oled_t *oled, uint8_t x, uint8_t y, uint8_t encoding);
int8_t oled_DrawGlyph90(oled_t *oled, uint8_t x, uint8_t y, uint8_t encoding);
int8_t oled_DrawGlyph180(oled_t *oled, uint8_t x, uint8_t y, uint8_t encoding);
int8_t oled_DrawGlyph270(oled_t *oled, uint8_t x, uint8_t y, uint8_t encoding);
int8_t oled_DrawGlyphFontBBX(oled_t *oled, uint8_t x, uint8_t y, uint8_t dir, uint8_t encoding);

uint8_t oled_DrawStr(oled_t *oled, uint8_t x, uint8_t y, const char *s);
uint8_t oled_DrawStr90(oled_t *oled, uint8_t x, uint8_t y, const char *s);
uint8_t oled_DrawStr180(oled_t *oled, uint8_t x, uint8_t y, const char *s);
uint8_t oled_DrawStr270(oled_t *oled, uint8_t x, uint8_t y, const char *s);

uint8_t oled_DrawStrDir(oled_t *oled, uint8_t x, uint8_t y, uint8_t dir, const char *s);


uint8_t oled_DrawStrP(oled_t *oled, uint8_t x, uint8_t y, const oled_pgm_uint8_t *s);
uint8_t oled_DrawStr90P(oled_t *oled, uint8_t x, uint8_t y, const oled_pgm_uint8_t *s);
uint8_t oled_DrawStr180P(oled_t *oled, uint8_t x, uint8_t y, const oled_pgm_uint8_t *s);
uint8_t oled_DrawStr270P(oled_t *oled, uint8_t x, uint8_t y, const oled_pgm_uint8_t *s);


void oled_SetFontRefHeightText(oled_t *oled);
void oled_SetFontRefHeightExtendedText(oled_t *oled);
void oled_SetFontRefHeightAll(oled_t *oled);
void oled_SetFontLineSpacingFactor(oled_t *oled, uint8_t factor);

uint8_t oled_font_calc_vref_font(oled_t *oled);
uint8_t oled_font_calc_vref_bottom(oled_t *oled);
uint8_t oled_font_calc_vref_top(oled_t *oled);
uint8_t oled_font_calc_vref_center(oled_t *oled);

void oled_SetFontPosBaseline(oled_t *oled);
void oled_SetFontPosBottom(oled_t *oled);
void oled_SetFontPosCenter(oled_t *oled);
void oled_SetFontPosTop(oled_t *oled);


uint8_t oled_GetStrPixelWidth(oled_t *oled, const char *s);
uint8_t oled_GetStrPixelWidthP(oled_t *oled, const oled_pgm_uint8_t *s);
int8_t oled_GetStrX(oled_t *oled, const char *s);
int8_t oled_GetStrXP(oled_t *oled, const oled_pgm_uint8_t *s);
uint8_t oled_GetStrWidth(oled_t *oled, const char *s) OLED_NOINLINE;
uint8_t oled_GetStrWidthP(oled_t *oled, const oled_pgm_uint8_t *s);

uint8_t oled_DrawStrFontBBX(oled_t *oled, uint8_t x, uint8_t y, uint8_t dir, const char *s);

void oled_GetStrMinBox(oled_t *oled, const char *s, uint8_t *x, uint8_t *y, uint8_t *width, uint8_t *height);
void oled_GetStrAMinBox(oled_t *oled, const char *s, uint8_t *x, uint8_t *y, uint8_t *width, uint8_t *height);


uint8_t oled_DrawAAStr(oled_t *oled, uint8_t x, uint8_t y, const char *s);

/* oled_rect.c */

void oled_draw_box(oled_t *oled, uint8_t x, uint8_t y, uint8_t w, uint8_t h) OLED_NOINLINE; 

void oled_DrawHLine(oled_t *oled, uint8_t x, uint8_t y, uint8_t w) OLED_NOINLINE;
void oled_DrawVLine(oled_t *oled, uint8_t x, uint8_t y, uint8_t w) OLED_NOINLINE;
void oled_DrawFrame(oled_t *oled, uint8_t x, uint8_t y, uint8_t w, uint8_t h) OLED_NOINLINE;
void oled_DrawBox(oled_t *oled, uint8_t x, uint8_t y, uint8_t w, uint8_t h) OLED_NOINLINE;

void oled_DrawRFrame(oled_t *oled, uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r) OLED_NOINLINE;
void oled_DrawRBox(oled_t *oled, uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r) OLED_NOINLINE;

/* oled_bitmap.c */

void oled_DrawHBitmap(oled_t *oled, uint8_t x, uint8_t y, uint8_t cnt, const uint8_t *bitmap);
void oled_DrawHBitmapP(oled_t *oled, uint8_t x, uint8_t y, uint8_t cnt, const oled_pgm_uint8_t *bitmap);
void oled_DrawBitmap(oled_t *oled, uint8_t x, uint8_t y, uint8_t cnt, uint8_t h, const uint8_t *bitmap);
void oled_DrawBitmapP(oled_t *oled, uint8_t x, uint8_t y, uint8_t cnt, uint8_t h, const oled_pgm_uint8_t *bitmap);

void oled_DrawXBM(oled_t *oled, uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bitmap);
void oled_DrawXBMP(oled_t *oled, uint8_t x, uint8_t y, uint8_t w, uint8_t h, const oled_pgm_uint8_t *bitmap);


/* oled_line.c */
void oled_DrawLine(oled_t *oled, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);


/* oled_circle.c */

/* the following, commented code has been rewritten or is not yet finished
#define OLED_CIRC_UPPER_RIGHT 0x01
#define OLED_CIRC_UPPER_LEFT  0x02
#define OLED_CIRC_LOWER_LEFT 0x04
#define OLED_CIRC_LOWER_RIGHT  0x08
#define OLED_CIRC_ALL (OLED_CIRC_UPPER_RIGHT|OLED_CIRC_UPPER_LEFT|OLED_CIRC_LOWER_RIGHT|OLED_CIRC_LOWER_LEFT)
void oled_DrawEmpCirc(oled_t *oled, uint8_t x0, uint8_t y0, uint8_t rad, uint8_t option);
void oled_DrawFillCirc(oled_t *oled, uint8_t x0, uint8_t y0, uint8_t rad, uint8_t option);
void oled_DrawEllipseRect(oled_t *oled, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
*/

#define OLED_DRAW_UPPER_RIGHT 0x01
#define OLED_DRAW_UPPER_LEFT  0x02
#define OLED_DRAW_LOWER_LEFT 0x04
#define OLED_DRAW_LOWER_RIGHT  0x08
#define OLED_DRAW_ALL (OLED_DRAW_UPPER_RIGHT|OLED_DRAW_UPPER_LEFT|OLED_DRAW_LOWER_RIGHT|OLED_DRAW_LOWER_LEFT)

void oled_draw_circle(oled_t *oled, uint8_t x0, uint8_t y0, uint8_t rad, uint8_t option) OLED_NOINLINE;
void oled_draw_disc(oled_t *oled, uint8_t x0, uint8_t y0, uint8_t rad, uint8_t option) OLED_NOINLINE;

void oled_DrawCircle(oled_t *oled, uint8_t x0, uint8_t y0, uint8_t rad, uint8_t option);
void oled_DrawDisc(oled_t *oled, uint8_t x0, uint8_t y0, uint8_t rad, uint8_t option);

/* oled_ellipse.c */
void oled_DrawEllipse(oled_t *oled, uint8_t x0, uint8_t y0, uint8_t rx, uint8_t ry, uint8_t option);
void oled_DrawFilledEllipse(oled_t *oled, uint8_t x0, uint8_t y0, uint8_t rx, uint8_t ry, uint8_t option);

/* oled_clip.c */
uint8_t oled_is_box_bbx_intersection(oled_box_t *box, oled_dev_arg_bbx_t *bbx);


/* oled_cursor.c */
void oled_SetCursorFont(oled_t *oled, const oled_pgm_uint8_t *cursor_font);
void oled_SetCursorStyle(oled_t *oled, uint8_t encoding);
void oled_SetCursorPos(oled_t *oled, uint8_t cursor_x, uint8_t cursor_y);
void oled_SetCursorColor(oled_t *oled, uint8_t fg, uint8_t bg);
void oled_EnableCursor(oled_t *oled);
void oled_DisableCursor(oled_t *oled);
void oled_DrawCursor(oled_t *oled);

/* oled_polygon.c */

typedef int16_t pg_word_t;

#define PG_NOINLINE OLED_NOINLINE

struct pg_point_struct
{
  pg_word_t x;
  pg_word_t y;
};

typedef struct _pg_struct pg_struct;	/* forward declaration */

struct pg_edge_struct
{
  pg_word_t x_direction;	/* 1, if x2 is greater than x1, -1 otherwise */
  pg_word_t height;
  pg_word_t current_x_offset;
  pg_word_t error_offset;
  
  /* --- line loop --- */
  pg_word_t current_y;
  pg_word_t max_y;
  pg_word_t current_x;
  pg_word_t error;

  /* --- outer loop --- */
  uint8_t (*next_idx_fn)(pg_struct *pg, uint8_t i);
  uint8_t curr_idx;
};

/* maximum number of points in the polygon */
/* can be redefined, but highest possible value is 254 */
#define PG_MAX_POINTS 6

/* index numbers for the pge structures below */
#define PG_LEFT 0
#define PG_RIGHT 1


struct _pg_struct
{
  struct pg_point_struct list[PG_MAX_POINTS];
  uint8_t cnt;
  uint8_t is_min_y_not_flat;
  pg_word_t total_scan_line_cnt;
  struct pg_edge_struct pge[2];	/* left and right line draw structures */
};

void pg_ClearPolygonXY(pg_struct *pg);
void pg_AddPolygonXY(pg_struct *pg, oled_t *oled, int16_t x, int16_t y);
void pg_DrawPolygon(pg_struct *pg, oled_t *oled);
void oled_ClearPolygonXY(void);
void oled_AddPolygonXY(oled_t *oled, int16_t x, int16_t y);
void oled_DrawPolygon(oled_t *oled);
void oled_DrawTriangle(oled_t *oled, int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2);


/*===============================================================*/
/* oled_virtual_screen.c */
void oled_SetVirtualScreenDimension(oled_t *vs_oled, uint8_t width, uint8_t height);
uint8_t oled_AddToVirtualScreen(oled_t *vs_oled, uint8_t x, uint8_t y, oled_t *child_oled);

/*===============================================================*/
void st_Draw(uint8_t fps);
void st_Step(uint8_t player_pos, uint8_t is_auto_fire, uint8_t is_fire);

/*===============================================================*/
/* oled_com_i2c.c */

/* options for oled_i2c_init() */
#define OLED_I2C_OPT_NONE 0
#define OLED_I2C_OPT_NO_ACK 2
#define OLED_I2C_OPT_DEV_0 0
#define OLED_I2C_OPT_DEV_1 4
#define OLED_I2C_OPT_FAST 16

/* retrun values from oled_twi_get_error() */
#define OLED_I2C_ERR_NONE 0x00
/* the following values are bit masks */
#define OLED_I2C_ERR_TIMEOUT 0x01
#define OLED_I2C_ERR_BUS 0x02

void oled_i2c_clear_error(void) OLED_NOINLINE;
uint8_t  oled_i2c_get_error(void) OLED_NOINLINE;
uint8_t oled_i2c_get_err_pos(void) OLED_NOINLINE;
void oled_i2c_init(uint8_t options) OLED_NOINLINE;		/* use OLED_I2C_OPT_NONE as options */
uint8_t oled_i2c_wait(uint8_t mask, uint8_t pos) OLED_NOINLINE;
uint8_t oled_i2c_start(uint8_t sla) OLED_NOINLINE;
uint8_t oled_i2c_send_byte(uint8_t data) OLED_NOINLINE;
uint8_t oled_i2c_send_mode(uint8_t mode) OLED_NOINLINE;
void oled_i2c_stop(void) OLED_NOINLINE;


/*===============================================================*/
/* oled_u8toa.c */
/* v = value, d = number of digits */
const char *oled_u8toa(uint8_t v, uint8_t d);

/* oled_u8toa.c */
/* v = value, d = number of digits */
const char *oled_u16toa(uint16_t v, uint8_t d);

/*===============================================================*/
/* oled_delay.c */

/* delay by the specified number of milliseconds */
void oled_Delay(uint16_t val);

/* delay by one microsecond */
void oled_MicroDelay(void);

/* delay by 10 microseconds */
void oled_10MicroDelay(void);

/*===============================================================*/
/* chessengine.c */
#define CHESS_KEY_NONE 0
#define CHESS_KEY_NEXT 1
#define CHESS_KEY_PREV 2
#define CHESS_KEY_SELECT 3
#define CHESS_KEY_BACK 4

void chess_Init(oled_t *oled, uint8_t empty_body_color);
void chess_Draw(void);
void chess_Step(uint8_t keycode);

/*===============================================================*/
/* font definitions */
extern const oled_fntpgm_uint8_t oled_font_m2icon_5[] OLED_FONT_SECTION("oled_font_m2icon_5");
extern const oled_fntpgm_uint8_t oled_font_m2icon_7[] OLED_FONT_SECTION("oled_font_m2icon_7");
extern const oled_fntpgm_uint8_t oled_font_m2icon_9[] OLED_FONT_SECTION("oled_font_m2icon_9");

extern const oled_fntpgm_uint8_t oled_font_oledlib_4[] OLED_FONT_SECTION("oled_font_oledlib_4");
extern const oled_fntpgm_uint8_t oled_font_oledlib_4r[] OLED_FONT_SECTION("oled_font_oledlib_4r");


extern const oled_fntpgm_uint8_t oled_font_6x12_75r[] OLED_FONT_SECTION("oled_font_6x12_75r");
extern const oled_fntpgm_uint8_t oled_font_6x13_75r[] OLED_FONT_SECTION("oled_font_6x13_75r");
extern const oled_fntpgm_uint8_t oled_font_7x13_75r[] OLED_FONT_SECTION("oled_font_7x13_75r");
extern const oled_fntpgm_uint8_t oled_font_8x13_75r[] OLED_FONT_SECTION("oled_font_8x13_75r");
extern const oled_fntpgm_uint8_t oled_font_9x15_75r[] OLED_FONT_SECTION("oled_font_9x15_75r");
extern const oled_fntpgm_uint8_t oled_font_9x18_75r[] OLED_FONT_SECTION("oled_font_9x18_75r");
extern const oled_fntpgm_uint8_t oled_font_cu12_75r[] OLED_FONT_SECTION("oled_font_cu12_75r");
extern const oled_fntpgm_uint8_t oled_font_unifont_75r[] OLED_FONT_SECTION("oled_font_unifont_75r");
extern const oled_fntpgm_uint8_t oled_font_10x20_75r[] OLED_FONT_SECTION("oled_font_10x20_75r");

extern const oled_fntpgm_uint8_t oled_font_10x20_67_75[] OLED_FONT_SECTION("oled_font_10x20_67_75");
extern const oled_fntpgm_uint8_t oled_font_10x20_78_79[] OLED_FONT_SECTION("oled_font_10x20_78_79");
extern const oled_fntpgm_uint8_t oled_font_10x20[] OLED_FONT_SECTION("oled_font_10x20");
extern const oled_fntpgm_uint8_t oled_font_10x20r[] OLED_FONT_SECTION("oled_font_10x20r");
extern const oled_fntpgm_uint8_t oled_font_4x6[] OLED_FONT_SECTION("oled_font_4x6");
extern const oled_fntpgm_uint8_t oled_font_4x6r[] OLED_FONT_SECTION("oled_font_4x6r");
//extern const oled_fntpgm_uint8_t oled_font_4x6n[] OLED_FONT_SECTION("oled_font_4x6n");
extern const oled_fntpgm_uint8_t oled_font_5x7[] OLED_FONT_SECTION("oled_font_5x7");
extern const oled_fntpgm_uint8_t oled_font_5x7r[] OLED_FONT_SECTION("oled_font_5x7r");
extern const oled_fntpgm_uint8_t oled_font_5x8[] OLED_FONT_SECTION("oled_font_5x8");
extern const oled_fntpgm_uint8_t oled_font_5x8r[] OLED_FONT_SECTION("oled_font_5x8r");
extern const oled_fntpgm_uint8_t oled_font_6x10[] OLED_FONT_SECTION("oled_font_6x10");
extern const oled_fntpgm_uint8_t oled_font_6x10r[] OLED_FONT_SECTION("oled_font_6x10r");
extern const oled_fntpgm_uint8_t oled_font_6x12_67_75[] OLED_FONT_SECTION("oled_font_6x12_67_75");
extern const oled_fntpgm_uint8_t oled_font_6x12_78_79[] OLED_FONT_SECTION("oled_font_6x12_78_79");
extern const oled_fntpgm_uint8_t oled_font_6x12[] OLED_FONT_SECTION("oled_font_6x12");
extern const oled_fntpgm_uint8_t oled_font_6x12r[] OLED_FONT_SECTION("oled_font_6x12r");
extern const oled_fntpgm_uint8_t oled_font_6x13_67_75[] OLED_FONT_SECTION("oled_font_6x13_67_75");
extern const oled_fntpgm_uint8_t oled_font_6x13_78_79[] OLED_FONT_SECTION("oled_font_6x13_78_79");
extern const oled_fntpgm_uint8_t oled_font_6x13B[] OLED_FONT_SECTION("oled_font_6x13B");
extern const oled_fntpgm_uint8_t oled_font_6x13Br[] OLED_FONT_SECTION("oled_font_6x13Br");
extern const oled_fntpgm_uint8_t oled_font_6x13[] OLED_FONT_SECTION("oled_font_6x13");
extern const oled_fntpgm_uint8_t oled_font_6x13r[] OLED_FONT_SECTION("oled_font_6x13r");
extern const oled_fntpgm_uint8_t oled_font_6x13O[] OLED_FONT_SECTION("oled_font_6x13O");
extern const oled_fntpgm_uint8_t oled_font_6x13Or[] OLED_FONT_SECTION("oled_font_6x13Or");
extern const oled_fntpgm_uint8_t oled_font_7x13_67_75[] OLED_FONT_SECTION("oled_font_7x13_67_75");
extern const oled_fntpgm_uint8_t oled_font_7x13_78_79[] OLED_FONT_SECTION("oled_font_7x13_78_79");
extern const oled_fntpgm_uint8_t oled_font_7x13B[] OLED_FONT_SECTION("oled_font_7x13B");
extern const oled_fntpgm_uint8_t oled_font_7x13Br[] OLED_FONT_SECTION("oled_font_7x13Br");
extern const oled_fntpgm_uint8_t oled_font_7x13[] OLED_FONT_SECTION("oled_font_7x13");
extern const oled_fntpgm_uint8_t oled_font_7x13r[] OLED_FONT_SECTION("oled_font_7x13r");
extern const oled_fntpgm_uint8_t oled_font_7x13O[] OLED_FONT_SECTION("oled_font_7x13O");
extern const oled_fntpgm_uint8_t oled_font_7x13Or[] OLED_FONT_SECTION("oled_font_7x13Or");
extern const oled_fntpgm_uint8_t oled_font_7x14B[] OLED_FONT_SECTION("oled_font_7x14B");
extern const oled_fntpgm_uint8_t oled_font_7x14Br[] OLED_FONT_SECTION("oled_font_7x14Br");
extern const oled_fntpgm_uint8_t oled_font_7x14[] OLED_FONT_SECTION("oled_font_7x14");
extern const oled_fntpgm_uint8_t oled_font_7x14r[] OLED_FONT_SECTION("oled_font_7x14r");
extern const oled_fntpgm_uint8_t oled_font_8x13_67_75[] OLED_FONT_SECTION("oled_font_8x13_67_75");
extern const oled_fntpgm_uint8_t oled_font_8x13B[] OLED_FONT_SECTION("oled_font_8x13B");
extern const oled_fntpgm_uint8_t oled_font_8x13Br[] OLED_FONT_SECTION("oled_font_8x13Br");
extern const oled_fntpgm_uint8_t oled_font_8x13[] OLED_FONT_SECTION("oled_font_8x13");
extern const oled_fntpgm_uint8_t oled_font_8x13r[] OLED_FONT_SECTION("oled_font_8x13r");
extern const oled_fntpgm_uint8_t oled_font_8x13O[] OLED_FONT_SECTION("oled_font_8x13O");
extern const oled_fntpgm_uint8_t oled_font_8x13Or[] OLED_FONT_SECTION("oled_font_8x13Or");

extern const oled_fntpgm_uint8_t oled_font_9x15_67_75[] OLED_FONT_SECTION("oled_font_9x15_67_75");
extern const oled_fntpgm_uint8_t oled_font_9x15_78_79[] OLED_FONT_SECTION("oled_font_9x15_78_79");
extern const oled_fntpgm_uint8_t oled_font_9x15B[] OLED_FONT_SECTION("oled_font_9x15B");
extern const oled_fntpgm_uint8_t oled_font_9x15Br[] OLED_FONT_SECTION("oled_font_9x15Br");
extern const oled_fntpgm_uint8_t oled_font_9x15[] OLED_FONT_SECTION("oled_font_9x15");
extern const oled_fntpgm_uint8_t oled_font_9x15r[] OLED_FONT_SECTION("oled_font_9x15r");

extern const oled_fntpgm_uint8_t oled_font_9x18_67_75[] OLED_FONT_SECTION("oled_font_9x18_67_75");
extern const oled_fntpgm_uint8_t oled_font_9x18_78_79[] OLED_FONT_SECTION("oled_font_9x18_78_79");
extern const oled_fntpgm_uint8_t oled_font_9x18B[] OLED_FONT_SECTION("oled_font_9x18B");
extern const oled_fntpgm_uint8_t oled_font_9x18[] OLED_FONT_SECTION("oled_font_9x18");
extern const oled_fntpgm_uint8_t oled_font_9x18Br[] OLED_FONT_SECTION("oled_font_9x18Br");
extern const oled_fntpgm_uint8_t oled_font_9x18r[] OLED_FONT_SECTION("oled_font_9x18r");

extern const oled_fntpgm_uint8_t oled_font_cursor[] OLED_FONT_SECTION("oled_font_cursor");
extern const oled_fntpgm_uint8_t oled_font_cursorr[] OLED_FONT_SECTION("oled_font_cursorr");
extern const oled_fntpgm_uint8_t oled_font_micro[] OLED_FONT_SECTION("oled_font_micro");

extern const oled_fntpgm_uint8_t oled_font_cu12_67_75[] OLED_FONT_SECTION("oled_font_cu12_67_75");
extern const oled_fntpgm_uint8_t oled_font_cu12_78_79[] OLED_FONT_SECTION("oled_font_cu12_78_79");
extern const oled_fntpgm_uint8_t oled_font_cu12[] OLED_FONT_SECTION("oled_font_cu12");

/* 
  Free-Universal Bold 
  r: Reduced char set (codes 32 - 128)
  n: Numbers (codes 42 - 57)
  no char: Full set (codes 32 - 255)
*/

extern const oled_fntpgm_uint8_t oled_font_fub11[] OLED_FONT_SECTION("oled_font_fub11");
extern const oled_fntpgm_uint8_t oled_font_fub11r[] OLED_FONT_SECTION("oled_font_fub11r");
extern const oled_fntpgm_uint8_t oled_font_fub11n[] OLED_FONT_SECTION("oled_font_fub11n");
extern const oled_fntpgm_uint8_t oled_font_fub14[] OLED_FONT_SECTION("oled_font_fub14");
extern const oled_fntpgm_uint8_t oled_font_fub14r[] OLED_FONT_SECTION("oled_font_fub14r");
extern const oled_fntpgm_uint8_t oled_font_fub14n[] OLED_FONT_SECTION("oled_font_fub14n");
extern const oled_fntpgm_uint8_t oled_font_fub17[] OLED_FONT_SECTION("oled_font_fub17");
extern const oled_fntpgm_uint8_t oled_font_fub17r[] OLED_FONT_SECTION("oled_font_fub17r");
extern const oled_fntpgm_uint8_t oled_font_fub17n[] OLED_FONT_SECTION("oled_font_fub17n");
extern const oled_fntpgm_uint8_t oled_font_fub20[] OLED_FONT_SECTION("oled_font_fub20");
extern const oled_fntpgm_uint8_t oled_font_fub20r[] OLED_FONT_SECTION("oled_font_fub20r");
extern const oled_fntpgm_uint8_t oled_font_fub20n[] OLED_FONT_SECTION("oled_font_fub20n");
extern const oled_fntpgm_uint8_t oled_font_fub25[] OLED_FONT_SECTION("oled_font_fub25");
extern const oled_fntpgm_uint8_t oled_font_fub25r[] OLED_FONT_SECTION("oled_font_fub25r");
extern const oled_fntpgm_uint8_t oled_font_fub25n[] OLED_FONT_SECTION("oled_font_fub25n");
extern const oled_fntpgm_uint8_t oled_font_fub30[] OLED_FONT_SECTION("oled_font_fub30");
extern const oled_fntpgm_uint8_t oled_font_fub30r[] OLED_FONT_SECTION("oled_font_fub30r");
extern const oled_fntpgm_uint8_t oled_font_fub30n[] OLED_FONT_SECTION("oled_font_fub30n");
extern const oled_fntpgm_uint8_t oled_font_fub35n[] OLED_FONT_SECTION("oled_font_fub35n");
extern const oled_fntpgm_uint8_t oled_font_fub42n[] OLED_FONT_SECTION("oled_font_fub42n");
extern const oled_fntpgm_uint8_t oled_font_fub49n[] OLED_FONT_SECTION("oled_font_fub49n");

/* 
  Free-Universal Regular
  r: Reduced char set (codes 32 - 128)
  n: Numbers (codes 42 - 57)
  no char: Full set (codes 32 - 255)
*/

extern const oled_fntpgm_uint8_t oled_font_fur11[] OLED_FONT_SECTION("oled_font_fur11");
extern const oled_fntpgm_uint8_t oled_font_fur11r[] OLED_FONT_SECTION("oled_font_fur11r");
extern const oled_fntpgm_uint8_t oled_font_fur11n[] OLED_FONT_SECTION("oled_font_fur11n");
extern const oled_fntpgm_uint8_t oled_font_fur14[] OLED_FONT_SECTION("oled_font_fur14");
extern const oled_fntpgm_uint8_t oled_font_fur14r[] OLED_FONT_SECTION("oled_font_fur14r");
extern const oled_fntpgm_uint8_t oled_font_fur14n[] OLED_FONT_SECTION("oled_font_fur14n");
extern const oled_fntpgm_uint8_t oled_font_fur17[] OLED_FONT_SECTION("oled_font_fur17");
extern const oled_fntpgm_uint8_t oled_font_fur17r[] OLED_FONT_SECTION("oled_font_fur17r");
extern const oled_fntpgm_uint8_t oled_font_fur17n[] OLED_FONT_SECTION("oled_font_fur17n");
extern const oled_fntpgm_uint8_t oled_font_fur20[] OLED_FONT_SECTION("oled_font_fur20");
extern const oled_fntpgm_uint8_t oled_font_fur20r[] OLED_FONT_SECTION("oled_font_fur20r");
extern const oled_fntpgm_uint8_t oled_font_fur20n[] OLED_FONT_SECTION("oled_font_fur20n");
extern const oled_fntpgm_uint8_t oled_font_fur25[] OLED_FONT_SECTION("oled_font_fur25");
extern const oled_fntpgm_uint8_t oled_font_fur25r[] OLED_FONT_SECTION("oled_font_fur25r");
extern const oled_fntpgm_uint8_t oled_font_fur25n[] OLED_FONT_SECTION("oled_font_fur25n");
extern const oled_fntpgm_uint8_t oled_font_fur30[] OLED_FONT_SECTION("oled_font_fur30");
extern const oled_fntpgm_uint8_t oled_font_fur30r[] OLED_FONT_SECTION("oled_font_fur30r");
extern const oled_fntpgm_uint8_t oled_font_fur30n[] OLED_FONT_SECTION("oled_font_fur30n");
extern const oled_fntpgm_uint8_t oled_font_fur35n[] OLED_FONT_SECTION("oled_font_fur35n");
extern const oled_fntpgm_uint8_t oled_font_fur42n[] OLED_FONT_SECTION("oled_font_fur42n");
extern const oled_fntpgm_uint8_t oled_font_fur49n[] OLED_FONT_SECTION("oled_font_fur49n");

/* 
  Gentium Bold
  r: Reduced char set (codes 32 - 128)
  n: Numbers (codes 42 - 57)
  no char: Full set (codes 32 - 255)
*/

extern const oled_fntpgm_uint8_t oled_font_gdb11[] OLED_FONT_SECTION("oled_font_gdb11");
extern const oled_fntpgm_uint8_t oled_font_gdb12[] OLED_FONT_SECTION("oled_font_gdb12");
extern const oled_fntpgm_uint8_t oled_font_gdb14[] OLED_FONT_SECTION("oled_font_gdb14");
extern const oled_fntpgm_uint8_t oled_font_gdb17[] OLED_FONT_SECTION("oled_font_gdb17");
extern const oled_fntpgm_uint8_t oled_font_gdb20[] OLED_FONT_SECTION("oled_font_gdb20");
extern const oled_fntpgm_uint8_t oled_font_gdb25[] OLED_FONT_SECTION("oled_font_gdb25");
extern const oled_fntpgm_uint8_t oled_font_gdb30[] OLED_FONT_SECTION("oled_font_gdb30");

extern const oled_fntpgm_uint8_t oled_font_gdb11r[] OLED_FONT_SECTION("oled_font_gdb11r");
extern const oled_fntpgm_uint8_t oled_font_gdb12r[] OLED_FONT_SECTION("oled_font_gdb12r");
extern const oled_fntpgm_uint8_t oled_font_gdb14r[] OLED_FONT_SECTION("oled_font_gdb14r");
extern const oled_fntpgm_uint8_t oled_font_gdb17r[] OLED_FONT_SECTION("oled_font_gdb17r");
extern const oled_fntpgm_uint8_t oled_font_gdb20r[] OLED_FONT_SECTION("oled_font_gdb20r");
extern const oled_fntpgm_uint8_t oled_font_gdb25r[] OLED_FONT_SECTION("oled_font_gdb25r");
extern const oled_fntpgm_uint8_t oled_font_gdb30r[] OLED_FONT_SECTION("oled_font_gdb30r");

extern const oled_fntpgm_uint8_t oled_font_gdb11n[] OLED_FONT_SECTION("oled_font_gdb11n");
extern const oled_fntpgm_uint8_t oled_font_gdb12n[] OLED_FONT_SECTION("oled_font_gdb12n");
extern const oled_fntpgm_uint8_t oled_font_gdb14n[] OLED_FONT_SECTION("oled_font_gdb14n");
extern const oled_fntpgm_uint8_t oled_font_gdb17n[] OLED_FONT_SECTION("oled_font_gdb17n");
extern const oled_fntpgm_uint8_t oled_font_gdb20n[] OLED_FONT_SECTION("oled_font_gdb20n");
extern const oled_fntpgm_uint8_t oled_font_gdb25n[] OLED_FONT_SECTION("oled_font_gdb25n");
extern const oled_fntpgm_uint8_t oled_font_gdb30n[] OLED_FONT_SECTION("oled_font_gdb30n");

/* 
  Gentium Regular
  r: Reduced char set (codes 32 - 128)
  n: Numbers (codes 42 - 57)
  no char: Full set (codes 32 - 255)
*/

extern const oled_fntpgm_uint8_t oled_font_gdr9[] OLED_FONT_SECTION("oled_font_gdr9");
extern const oled_fntpgm_uint8_t oled_font_gdr10[] OLED_FONT_SECTION("oled_font_gdr10");
extern const oled_fntpgm_uint8_t oled_font_gdr11[] OLED_FONT_SECTION("oled_font_gdr11");
extern const oled_fntpgm_uint8_t oled_font_gdr12[] OLED_FONT_SECTION("oled_font_gdr12");
extern const oled_fntpgm_uint8_t oled_font_gdr14[] OLED_FONT_SECTION("oled_font_gdr14");
extern const oled_fntpgm_uint8_t oled_font_gdr17[] OLED_FONT_SECTION("oled_font_gdr17");
extern const oled_fntpgm_uint8_t oled_font_gdr20[] OLED_FONT_SECTION("oled_font_gdr20");
extern const oled_fntpgm_uint8_t oled_font_gdr25[] OLED_FONT_SECTION("oled_font_gdr25");
extern const oled_fntpgm_uint8_t oled_font_gdr30[] OLED_FONT_SECTION("oled_font_gdr30");

extern const oled_fntpgm_uint8_t oled_font_gdr9r[] OLED_FONT_SECTION("oled_font_gdr9r");
extern const oled_fntpgm_uint8_t oled_font_gdr10r[] OLED_FONT_SECTION("oled_font_gdr10r");
extern const oled_fntpgm_uint8_t oled_font_gdr11r[] OLED_FONT_SECTION("oled_font_gdr11r");
extern const oled_fntpgm_uint8_t oled_font_gdr12r[] OLED_FONT_SECTION("oled_font_gdr12r");
extern const oled_fntpgm_uint8_t oled_font_gdr14r[] OLED_FONT_SECTION("oled_font_gdr14r");
extern const oled_fntpgm_uint8_t oled_font_gdr17r[] OLED_FONT_SECTION("oled_font_gdr17r");
extern const oled_fntpgm_uint8_t oled_font_gdr20r[] OLED_FONT_SECTION("oled_font_gdr20r");
extern const oled_fntpgm_uint8_t oled_font_gdr25r[] OLED_FONT_SECTION("oled_font_gdr25r");
extern const oled_fntpgm_uint8_t oled_font_gdr30r[] OLED_FONT_SECTION("oled_font_gdr30r");

extern const oled_fntpgm_uint8_t oled_font_gdr9n[] OLED_FONT_SECTION("oled_font_gdr9n");
extern const oled_fntpgm_uint8_t oled_font_gdr10n[] OLED_FONT_SECTION("oled_font_gdr10n");
extern const oled_fntpgm_uint8_t oled_font_gdr11n[] OLED_FONT_SECTION("oled_font_gdr11n");
extern const oled_fntpgm_uint8_t oled_font_gdr12n[] OLED_FONT_SECTION("oled_font_gdr12n");
extern const oled_fntpgm_uint8_t oled_font_gdr14n[] OLED_FONT_SECTION("oled_font_gdr14n");
extern const oled_fntpgm_uint8_t oled_font_gdr17n[] OLED_FONT_SECTION("oled_font_gdr17n");
extern const oled_fntpgm_uint8_t oled_font_gdr20n[] OLED_FONT_SECTION("oled_font_gdr20n");
extern const oled_fntpgm_uint8_t oled_font_gdr25n[] OLED_FONT_SECTION("oled_font_gdr25n");
extern const oled_fntpgm_uint8_t oled_font_gdr30n[] OLED_FONT_SECTION("oled_font_gdr30n");

/* 
  Old-Standard Bold
  r: Reduced char set (codes 32 - 128)
  n: Numbers (codes 42 - 57)
  no char: Full set (codes 32 - 255)
*/

extern const oled_fntpgm_uint8_t oled_font_osb18[] OLED_FONT_SECTION("oled_font_osb18");
extern const oled_fntpgm_uint8_t oled_font_osb21[] OLED_FONT_SECTION("oled_font_osb21");
extern const oled_fntpgm_uint8_t oled_font_osb26[] OLED_FONT_SECTION("oled_font_osb26");
extern const oled_fntpgm_uint8_t oled_font_osb29[] OLED_FONT_SECTION("oled_font_osb29");
extern const oled_fntpgm_uint8_t oled_font_osb35[] OLED_FONT_SECTION("oled_font_osb35");

extern const oled_fntpgm_uint8_t oled_font_osb18r[] OLED_FONT_SECTION("oled_font_osb18r");
extern const oled_fntpgm_uint8_t oled_font_osb21r[] OLED_FONT_SECTION("oled_font_osb21r");
extern const oled_fntpgm_uint8_t oled_font_osb26r[] OLED_FONT_SECTION("oled_font_osb26r");
extern const oled_fntpgm_uint8_t oled_font_osb29r[] OLED_FONT_SECTION("oled_font_osb29r");
extern const oled_fntpgm_uint8_t oled_font_osb35r[] OLED_FONT_SECTION("oled_font_osb35r");

extern const oled_fntpgm_uint8_t oled_font_osb18n[] OLED_FONT_SECTION("oled_font_osb18n");
extern const oled_fntpgm_uint8_t oled_font_osb21n[] OLED_FONT_SECTION("oled_font_osb21n");
extern const oled_fntpgm_uint8_t oled_font_osb26n[] OLED_FONT_SECTION("oled_font_osb26n");
extern const oled_fntpgm_uint8_t oled_font_osb29n[] OLED_FONT_SECTION("oled_font_osb29n");
extern const oled_fntpgm_uint8_t oled_font_osb35n[] OLED_FONT_SECTION("oled_font_osb35n");

/* 
  Old-Standard Regular
  r: Reduced char set (codes 32 - 128)
  n: Numbers (codes 42 - 57)
  no char: Full set (codes 32 - 255)
*/

extern const oled_fntpgm_uint8_t oled_font_osr18[] OLED_FONT_SECTION("oled_font_osr18");
extern const oled_fntpgm_uint8_t oled_font_osr21[] OLED_FONT_SECTION("oled_font_osr21");
extern const oled_fntpgm_uint8_t oled_font_osr26[] OLED_FONT_SECTION("oled_font_osr26");
extern const oled_fntpgm_uint8_t oled_font_osr29[] OLED_FONT_SECTION("oled_font_osr29");
extern const oled_fntpgm_uint8_t oled_font_osr35[] OLED_FONT_SECTION("oled_font_osr35");

extern const oled_fntpgm_uint8_t oled_font_osr18r[] OLED_FONT_SECTION("oled_font_osr18r");
extern const oled_fntpgm_uint8_t oled_font_osr21r[] OLED_FONT_SECTION("oled_font_osr21r");
extern const oled_fntpgm_uint8_t oled_font_osr26r[] OLED_FONT_SECTION("oled_font_osr26r");
extern const oled_fntpgm_uint8_t oled_font_osr29r[] OLED_FONT_SECTION("oled_font_osr29r");
extern const oled_fntpgm_uint8_t oled_font_osr35r[] OLED_FONT_SECTION("oled_font_osr35r");

extern const oled_fntpgm_uint8_t oled_font_osr18n[] OLED_FONT_SECTION("oled_font_osr18n");
extern const oled_fntpgm_uint8_t oled_font_osr21n[] OLED_FONT_SECTION("oled_font_osr21n");
extern const oled_fntpgm_uint8_t oled_font_osr26n[] OLED_FONT_SECTION("oled_font_osr26n");
extern const oled_fntpgm_uint8_t oled_font_osr29n[] OLED_FONT_SECTION("oled_font_osr29n");
extern const oled_fntpgm_uint8_t oled_font_osr35n[] OLED_FONT_SECTION("oled_font_osr35n");

//extern const oled_fntpgm_uint8_t oled_font_osr41[] OLED_FONT_SECTION("oled_font_osr41");

/* GNU unifont */

extern const oled_fntpgm_uint8_t oled_font_unifont_18_19[] OLED_FONT_SECTION("oled_font_unifont_18_19");
extern const oled_fntpgm_uint8_t oled_font_unifont_72_73[] OLED_FONT_SECTION("oled_font_unifont_72_73");
extern const oled_fntpgm_uint8_t oled_font_unifont_67_75[] OLED_FONT_SECTION("oled_font_unifont_67_75");
extern const oled_fntpgm_uint8_t oled_font_unifont_76[] OLED_FONT_SECTION("oled_font_unifont_76");
extern const oled_fntpgm_uint8_t oled_font_unifont_77[] OLED_FONT_SECTION("oled_font_unifont_77");
extern const oled_fntpgm_uint8_t oled_font_unifont_78_79[] OLED_FONT_SECTION("oled_font_unifont_78_79");
extern const oled_fntpgm_uint8_t oled_font_unifont_86[] OLED_FONT_SECTION("oled_font_unifont_86");
extern const oled_fntpgm_uint8_t oled_font_unifont[] OLED_FONT_SECTION("oled_font_unifont");
extern const oled_fntpgm_uint8_t oled_font_unifontr[] OLED_FONT_SECTION("oled_font_unifontr");
extern const oled_fntpgm_uint8_t oled_font_unifont_0_8[] OLED_FONT_SECTION("oled_font_unifont_0_8");
extern const oled_fntpgm_uint8_t oled_font_unifont_2_3[] OLED_FONT_SECTION("oled_font_unifont_2_3");
extern const oled_fntpgm_uint8_t oled_font_unifont_4_5[] OLED_FONT_SECTION("oled_font_unifont_4_5");
extern const oled_fntpgm_uint8_t oled_font_unifont_8_9[] OLED_FONT_SECTION("oled_font_unifont_8_9");
extern const oled_fntpgm_uint8_t oled_font_unifont_12_13[] OLED_FONT_SECTION("oled_font_unifont_12_13");


/* 04b fonts */

extern const oled_fntpgm_uint8_t oled_font_04b_03b[] OLED_FONT_SECTION("oled_font_04b_03b"); 
extern const oled_fntpgm_uint8_t oled_font_04b_03bn[] OLED_FONT_SECTION("oled_font_04b_03bn");
extern const oled_fntpgm_uint8_t oled_font_04b_03br[] OLED_FONT_SECTION("oled_font_04b_03br");
extern const oled_fntpgm_uint8_t oled_font_04b_03[] OLED_FONT_SECTION("oled_font_04b_03");
extern const oled_fntpgm_uint8_t oled_font_04b_03n[] OLED_FONT_SECTION("oled_font_04b_03n");
extern const oled_fntpgm_uint8_t oled_font_04b_03r[] OLED_FONT_SECTION("oled_font_04b_03r");
extern const oled_fntpgm_uint8_t oled_font_04b_24[] OLED_FONT_SECTION("oled_font_04b_24");
extern const oled_fntpgm_uint8_t oled_font_04b_24n[] OLED_FONT_SECTION("oled_font_04b_24n");
extern const oled_fntpgm_uint8_t oled_font_04b_24r[] OLED_FONT_SECTION("oled_font_04b_24r");

/* orgdot fonts */

extern const oled_fntpgm_uint8_t oled_font_orgv01[] OLED_FONT_SECTION("oled_font_orgv01");
extern const oled_fntpgm_uint8_t oled_font_orgv01r[] OLED_FONT_SECTION("oled_font_orgv01r");
extern const oled_fntpgm_uint8_t oled_font_orgv01n[] OLED_FONT_SECTION("oled_font_orgv01n");

extern const oled_fntpgm_uint8_t oled_font_fixed_v0[] OLED_FONT_SECTION("oled_font_fixed_v0");
extern const oled_fntpgm_uint8_t oled_font_fixed_v0r[] OLED_FONT_SECTION("oled_font_fixed_v0r");
extern const oled_fntpgm_uint8_t oled_font_fixed_v0n[] OLED_FONT_SECTION("oled_font_fixed_v0n");

extern const oled_fntpgm_uint8_t oled_font_tpssb[] OLED_FONT_SECTION("oled_font_tpssb");
extern const oled_fntpgm_uint8_t oled_font_tpssbr[] OLED_FONT_SECTION("oled_font_tpssbr");
extern const oled_fntpgm_uint8_t oled_font_tpssbn[] OLED_FONT_SECTION("oled_font_tpssbn");

extern const oled_fntpgm_uint8_t oled_font_tpss[] OLED_FONT_SECTION("oled_font_tpss");
extern const oled_fntpgm_uint8_t oled_font_tpssr[] OLED_FONT_SECTION("oled_font_tpssr");
extern const oled_fntpgm_uint8_t oled_font_tpssn[] OLED_FONT_SECTION("oled_font_tpssn");

/* contributed */

extern const oled_fntpgm_uint8_t oled_font_freedoomr25n[] OLED_FONT_SECTION("oled_font_freedoomr25n");
extern const oled_fntpgm_uint8_t oled_font_freedoomr10r[] OLED_FONT_SECTION("oled_font_freedoomr10r");

/* adobe X11 */
extern const oled_fntpgm_uint8_t oled_font_courB08[] OLED_FONT_SECTION("oled_font_courB08");
extern const oled_fntpgm_uint8_t oled_font_courB08r[] OLED_FONT_SECTION("oled_font_courB08r");
extern const oled_fntpgm_uint8_t oled_font_courB10[] OLED_FONT_SECTION("oled_font_courB10");
extern const oled_fntpgm_uint8_t oled_font_courB10r[] OLED_FONT_SECTION("oled_font_courB10r");
extern const oled_fntpgm_uint8_t oled_font_courB12[] OLED_FONT_SECTION("oled_font_courB12");
extern const oled_fntpgm_uint8_t oled_font_courB12r[] OLED_FONT_SECTION("oled_font_courB12r");
extern const oled_fntpgm_uint8_t oled_font_courB14[] OLED_FONT_SECTION("oled_font_courB14");
extern const oled_fntpgm_uint8_t oled_font_courB14r[] OLED_FONT_SECTION("oled_font_courB14r");
extern const oled_fntpgm_uint8_t oled_font_courB18[] OLED_FONT_SECTION("oled_font_courB18");
extern const oled_fntpgm_uint8_t oled_font_courB18r[] OLED_FONT_SECTION("oled_font_courB18r");
extern const oled_fntpgm_uint8_t oled_font_courB24[] OLED_FONT_SECTION("oled_font_courB24");
extern const oled_fntpgm_uint8_t oled_font_courB24r[] OLED_FONT_SECTION("oled_font_courB24r");
extern const oled_fntpgm_uint8_t oled_font_courB24n[] OLED_FONT_SECTION("oled_font_courB24n");

extern const oled_fntpgm_uint8_t oled_font_courR08[] OLED_FONT_SECTION("oled_font_courR08");
extern const oled_fntpgm_uint8_t oled_font_courR08r[] OLED_FONT_SECTION("oled_font_courR08r");
extern const oled_fntpgm_uint8_t oled_font_courR10[] OLED_FONT_SECTION("oled_font_courR10");
extern const oled_fntpgm_uint8_t oled_font_courR10r[] OLED_FONT_SECTION("oled_font_courR10r");
extern const oled_fntpgm_uint8_t oled_font_courR12[] OLED_FONT_SECTION("oled_font_courR12");
extern const oled_fntpgm_uint8_t oled_font_courR12r[] OLED_FONT_SECTION("oled_font_courR12r");
extern const oled_fntpgm_uint8_t oled_font_courR14[] OLED_FONT_SECTION("oled_font_courR14");
extern const oled_fntpgm_uint8_t oled_font_courR14r[] OLED_FONT_SECTION("oled_font_courR14r");
extern const oled_fntpgm_uint8_t oled_font_courR18[] OLED_FONT_SECTION("oled_font_courR18");
extern const oled_fntpgm_uint8_t oled_font_courR18r[] OLED_FONT_SECTION("oled_font_courR18r");
extern const oled_fntpgm_uint8_t oled_font_courR24[] OLED_FONT_SECTION("oled_font_courR24");
extern const oled_fntpgm_uint8_t oled_font_courR24r[] OLED_FONT_SECTION("oled_font_courR24r");
extern const oled_fntpgm_uint8_t oled_font_courR24n[] OLED_FONT_SECTION("oled_font_courR24n");

extern const oled_fntpgm_uint8_t oled_font_helvB08[] OLED_FONT_SECTION("oled_font_helvB08");
extern const oled_fntpgm_uint8_t oled_font_helvB08r[] OLED_FONT_SECTION("oled_font_helvB08r");
extern const oled_fntpgm_uint8_t oled_font_helvB08n[] OLED_FONT_SECTION("oled_font_helvB08n");
extern const oled_fntpgm_uint8_t oled_font_helvB10[] OLED_FONT_SECTION("oled_font_helvB10");
extern const oled_fntpgm_uint8_t oled_font_helvB10r[] OLED_FONT_SECTION("oled_font_helvB10r");
extern const oled_fntpgm_uint8_t oled_font_helvB10n[] OLED_FONT_SECTION("oled_font_helvB10n");
extern const oled_fntpgm_uint8_t oled_font_helvB12[] OLED_FONT_SECTION("oled_font_helvB12");
extern const oled_fntpgm_uint8_t oled_font_helvB12r[] OLED_FONT_SECTION("oled_font_helvB12r");
extern const oled_fntpgm_uint8_t oled_font_helvB12n[] OLED_FONT_SECTION("oled_font_helvB12n");
extern const oled_fntpgm_uint8_t oled_font_helvB14[] OLED_FONT_SECTION("oled_font_helvB14");
extern const oled_fntpgm_uint8_t oled_font_helvB14r[] OLED_FONT_SECTION("oled_font_helvB14r");
extern const oled_fntpgm_uint8_t oled_font_helvB14n[] OLED_FONT_SECTION("oled_font_helvB14n");
extern const oled_fntpgm_uint8_t oled_font_helvB18[] OLED_FONT_SECTION("oled_font_helvB18");
extern const oled_fntpgm_uint8_t oled_font_helvB18r[] OLED_FONT_SECTION("oled_font_helvB18r");
extern const oled_fntpgm_uint8_t oled_font_helvB18n[] OLED_FONT_SECTION("oled_font_helvB18n");
extern const oled_fntpgm_uint8_t oled_font_helvB24[] OLED_FONT_SECTION("oled_font_helvB24");
extern const oled_fntpgm_uint8_t oled_font_helvB24r[] OLED_FONT_SECTION("oled_font_helvB24r");
extern const oled_fntpgm_uint8_t oled_font_helvB24n[] OLED_FONT_SECTION("oled_font_helvB24n");

extern const oled_fntpgm_uint8_t oled_font_helvR08[] OLED_FONT_SECTION("oled_font_helvR08");
extern const oled_fntpgm_uint8_t oled_font_helvR08r[] OLED_FONT_SECTION("oled_font_helvR08r");
extern const oled_fntpgm_uint8_t oled_font_helvR08n[] OLED_FONT_SECTION("oled_font_helvR08n");
extern const oled_fntpgm_uint8_t oled_font_helvR10[] OLED_FONT_SECTION("oled_font_helvR10");
extern const oled_fntpgm_uint8_t oled_font_helvR10r[] OLED_FONT_SECTION("oled_font_helvR10r");
extern const oled_fntpgm_uint8_t oled_font_helvR10n[] OLED_FONT_SECTION("oled_font_helvR10n");
extern const oled_fntpgm_uint8_t oled_font_helvR12[] OLED_FONT_SECTION("oled_font_helvR12");
extern const oled_fntpgm_uint8_t oled_font_helvR12r[] OLED_FONT_SECTION("oled_font_helvR12r");
extern const oled_fntpgm_uint8_t oled_font_helvR12n[] OLED_FONT_SECTION("oled_font_helvR12n");
extern const oled_fntpgm_uint8_t oled_font_helvR14[] OLED_FONT_SECTION("oled_font_helvR14");
extern const oled_fntpgm_uint8_t oled_font_helvR14r[] OLED_FONT_SECTION("oled_font_helvR14r");
extern const oled_fntpgm_uint8_t oled_font_helvR14n[] OLED_FONT_SECTION("oled_font_helvR14n");
extern const oled_fntpgm_uint8_t oled_font_helvR18[] OLED_FONT_SECTION("oled_font_helvR18");
extern const oled_fntpgm_uint8_t oled_font_helvR18r[] OLED_FONT_SECTION("oled_font_helvR18r");
extern const oled_fntpgm_uint8_t oled_font_helvR18n[] OLED_FONT_SECTION("oled_font_helvR18n");
extern const oled_fntpgm_uint8_t oled_font_helvR24[] OLED_FONT_SECTION("oled_font_helvR24");
extern const oled_fntpgm_uint8_t oled_font_helvR24r[] OLED_FONT_SECTION("oled_font_helvR24r");
extern const oled_fntpgm_uint8_t oled_font_helvR24n[] OLED_FONT_SECTION("oled_font_helvR24n");

extern const oled_fntpgm_uint8_t oled_font_ncenB08[] OLED_FONT_SECTION("oled_font_ncenB08");
extern const oled_fntpgm_uint8_t oled_font_ncenB08r[] OLED_FONT_SECTION("oled_font_ncenB08r");
extern const oled_fntpgm_uint8_t oled_font_ncenB10[] OLED_FONT_SECTION("oled_font_ncenB10");
extern const oled_fntpgm_uint8_t oled_font_ncenB10r[] OLED_FONT_SECTION("oled_font_ncenB10r");
extern const oled_fntpgm_uint8_t oled_font_ncenB12[] OLED_FONT_SECTION("oled_font_ncenB12");
extern const oled_fntpgm_uint8_t oled_font_ncenB12r[] OLED_FONT_SECTION("oled_font_ncenB12r");
extern const oled_fntpgm_uint8_t oled_font_ncenB14[] OLED_FONT_SECTION("oled_font_ncenB14");
extern const oled_fntpgm_uint8_t oled_font_ncenB14r[] OLED_FONT_SECTION("oled_font_ncenB14r");
extern const oled_fntpgm_uint8_t oled_font_ncenB18[] OLED_FONT_SECTION("oled_font_ncenB18");
extern const oled_fntpgm_uint8_t oled_font_ncenB18r[] OLED_FONT_SECTION("oled_font_ncenB18r");
extern const oled_fntpgm_uint8_t oled_font_ncenB24[] OLED_FONT_SECTION("oled_font_ncenB24");
extern const oled_fntpgm_uint8_t oled_font_ncenB24r[] OLED_FONT_SECTION("oled_font_ncenB24r");
extern const oled_fntpgm_uint8_t oled_font_ncenB24n[] OLED_FONT_SECTION("oled_font_ncenB24n");

extern const oled_fntpgm_uint8_t oled_font_ncenR08[] OLED_FONT_SECTION("oled_font_ncenR08");
extern const oled_fntpgm_uint8_t oled_font_ncenR08r[] OLED_FONT_SECTION("oled_font_ncenR08r");
extern const oled_fntpgm_uint8_t oled_font_ncenR10[] OLED_FONT_SECTION("oled_font_ncenR10");
extern const oled_fntpgm_uint8_t oled_font_ncenR10r[] OLED_FONT_SECTION("oled_font_ncenR10r");
extern const oled_fntpgm_uint8_t oled_font_ncenR12[] OLED_FONT_SECTION("oled_font_ncenR12");
extern const oled_fntpgm_uint8_t oled_font_ncenR12r[] OLED_FONT_SECTION("oled_font_ncenR12r");
extern const oled_fntpgm_uint8_t oled_font_ncenR14[] OLED_FONT_SECTION("oled_font_ncenR14");
extern const oled_fntpgm_uint8_t oled_font_ncenR14r[] OLED_FONT_SECTION("oled_font_ncenR14r");
extern const oled_fntpgm_uint8_t oled_font_ncenR18[] OLED_FONT_SECTION("oled_font_ncenR18");
extern const oled_fntpgm_uint8_t oled_font_ncenR18r[] OLED_FONT_SECTION("oled_font_ncenR18r");
extern const oled_fntpgm_uint8_t oled_font_ncenR24[] OLED_FONT_SECTION("oled_font_ncenR24");
extern const oled_fntpgm_uint8_t oled_font_ncenR24r[] OLED_FONT_SECTION("oled_font_ncenR24r");
extern const oled_fntpgm_uint8_t oled_font_ncenR24n[] OLED_FONT_SECTION("oled_font_ncenR24n");

extern const oled_fntpgm_uint8_t oled_font_symb08[] OLED_FONT_SECTION("oled_font_symb08");
extern const oled_fntpgm_uint8_t oled_font_symb08r[] OLED_FONT_SECTION("oled_font_symb08r");
extern const oled_fntpgm_uint8_t oled_font_symb10[] OLED_FONT_SECTION("oled_font_symb10");
extern const oled_fntpgm_uint8_t oled_font_symb10r[] OLED_FONT_SECTION("oled_font_symb10r");
extern const oled_fntpgm_uint8_t oled_font_symb12[] OLED_FONT_SECTION("oled_font_symb12");
extern const oled_fntpgm_uint8_t oled_font_symb12r[] OLED_FONT_SECTION("oled_font_symb12r");
extern const oled_fntpgm_uint8_t oled_font_symb14[] OLED_FONT_SECTION("oled_font_symb14");
extern const oled_fntpgm_uint8_t oled_font_symb14r[] OLED_FONT_SECTION("oled_font_symb14r");
extern const oled_fntpgm_uint8_t oled_font_symb18[] OLED_FONT_SECTION("oled_font_symb18");
extern const oled_fntpgm_uint8_t oled_font_symb18r[] OLED_FONT_SECTION("oled_font_symb18r");
extern const oled_fntpgm_uint8_t oled_font_symb24[] OLED_FONT_SECTION("oled_font_symb24");
extern const oled_fntpgm_uint8_t oled_font_symb24r[] OLED_FONT_SECTION("oled_font_symb24r");

extern const oled_fntpgm_uint8_t oled_font_timB08[] OLED_FONT_SECTION("oled_font_timB08");
extern const oled_fntpgm_uint8_t oled_font_timB08r[] OLED_FONT_SECTION("oled_font_timB08r");
extern const oled_fntpgm_uint8_t oled_font_timB10[] OLED_FONT_SECTION("oled_font_timB10");
extern const oled_fntpgm_uint8_t oled_font_timB10r[] OLED_FONT_SECTION("oled_font_timB10r");
extern const oled_fntpgm_uint8_t oled_font_timB12[] OLED_FONT_SECTION("oled_font_timB12");
extern const oled_fntpgm_uint8_t oled_font_timB12r[] OLED_FONT_SECTION("oled_font_timB12r");
extern const oled_fntpgm_uint8_t oled_font_timB14[] OLED_FONT_SECTION("oled_font_timB14");
extern const oled_fntpgm_uint8_t oled_font_timB14r[] OLED_FONT_SECTION("oled_font_timB14r");
extern const oled_fntpgm_uint8_t oled_font_timB18[] OLED_FONT_SECTION("oled_font_timB18");
extern const oled_fntpgm_uint8_t oled_font_timB18r[] OLED_FONT_SECTION("oled_font_timB18r");
extern const oled_fntpgm_uint8_t oled_font_timB24[] OLED_FONT_SECTION("oled_font_timB24");
extern const oled_fntpgm_uint8_t oled_font_timB24r[] OLED_FONT_SECTION("oled_font_timB24r");
extern const oled_fntpgm_uint8_t oled_font_timB24n[] OLED_FONT_SECTION("oled_font_timB24n");

extern const oled_fntpgm_uint8_t oled_font_timR08[] OLED_FONT_SECTION("oled_font_timR08");
extern const oled_fntpgm_uint8_t oled_font_timR08r[] OLED_FONT_SECTION("oled_font_timR08r");
extern const oled_fntpgm_uint8_t oled_font_timR10[] OLED_FONT_SECTION("oled_font_timR10");
extern const oled_fntpgm_uint8_t oled_font_timR10r[] OLED_FONT_SECTION("oled_font_timR10r");
extern const oled_fntpgm_uint8_t oled_font_timR12[] OLED_FONT_SECTION("oled_font_timR12");
extern const oled_fntpgm_uint8_t oled_font_timR12r[] OLED_FONT_SECTION("oled_font_timR12r");
extern const oled_fntpgm_uint8_t oled_font_timR14[] OLED_FONT_SECTION("oled_font_timR14");
extern const oled_fntpgm_uint8_t oled_font_timR14r[] OLED_FONT_SECTION("oled_font_timR14r");
extern const oled_fntpgm_uint8_t oled_font_timR18[] OLED_FONT_SECTION("oled_font_timR18");
extern const oled_fntpgm_uint8_t oled_font_timR18r[] OLED_FONT_SECTION("oled_font_timR18r");
extern const oled_fntpgm_uint8_t oled_font_timR24[] OLED_FONT_SECTION("oled_font_timR24");
extern const oled_fntpgm_uint8_t oled_font_timR24r[] OLED_FONT_SECTION("oled_font_timR24r");
extern const oled_fntpgm_uint8_t oled_font_timR24n[] OLED_FONT_SECTION("oled_font_timR24n");

/* fontstruct */

extern const oled_fntpgm_uint8_t oled_font_p01type[] OLED_FONT_SECTION("oled_font_p01type");
extern const oled_fntpgm_uint8_t oled_font_p01typer[] OLED_FONT_SECTION("oled_font_p01typer");
extern const oled_fntpgm_uint8_t oled_font_p01typen[] OLED_FONT_SECTION("oled_font_p01typen");

extern const oled_fntpgm_uint8_t oled_font_lucasfont_alternate[] OLED_FONT_SECTION("oled_font_lucasfont_alternate");
extern const oled_fntpgm_uint8_t oled_font_lucasfont_alternater[] OLED_FONT_SECTION("oled_font_lucasfont_alternater");
extern const oled_fntpgm_uint8_t oled_font_lucasfont_alternaten[] OLED_FONT_SECTION("oled_font_lucasfont_alternaten");

extern const oled_fntpgm_uint8_t oled_font_chikita[] OLED_FONT_SECTION("oled_font_chikita");
extern const oled_fntpgm_uint8_t oled_font_chikitar[] OLED_FONT_SECTION("oled_font_chikitar");
extern const oled_fntpgm_uint8_t oled_font_chikitan[] OLED_FONT_SECTION("oled_font_chikitan");

extern const oled_fntpgm_uint8_t oled_font_pixelle_micro[] OLED_FONT_SECTION("oled_font_pixelle_micro");
extern const oled_fntpgm_uint8_t oled_font_pixelle_micror[] OLED_FONT_SECTION("oled_font_pixelle_micror");
extern const oled_fntpgm_uint8_t oled_font_pixelle_micron[] OLED_FONT_SECTION("oled_font_pixelle_micron");

extern const oled_fntpgm_uint8_t oled_font_trixel_square[] OLED_FONT_SECTION("oled_font_trixel_square");
extern const oled_fntpgm_uint8_t oled_font_trixel_squarer[] OLED_FONT_SECTION("oled_font_trixel_squarer");
extern const oled_fntpgm_uint8_t oled_font_trixel_squaren[] OLED_FONT_SECTION("oled_font_trixel_squaren");

extern const oled_fntpgm_uint8_t oled_font_robot_de_niro[] OLED_FONT_SECTION("oled_font_robot_de_niro");
extern const oled_fntpgm_uint8_t oled_font_robot_de_niror[] OLED_FONT_SECTION("oled_font_robot_de_niror");
extern const oled_fntpgm_uint8_t oled_font_robot_de_niron[] OLED_FONT_SECTION("oled_font_robot_de_niron");

extern const oled_fntpgm_uint8_t oled_font_baby[] OLED_FONT_SECTION("oled_font_baby");
extern const oled_fntpgm_uint8_t oled_font_babyr[] OLED_FONT_SECTION("oled_font_babyr");
extern const oled_fntpgm_uint8_t oled_font_babyn[] OLED_FONT_SECTION("oled_font_babyn");

extern const oled_fntpgm_uint8_t oled_font_blipfest_07[] OLED_FONT_SECTION("oled_font_blipfest_07");
extern const oled_fntpgm_uint8_t oled_font_blipfest_07r[] OLED_FONT_SECTION("oled_font_blipfest_07r");
extern const oled_fntpgm_uint8_t oled_font_blipfest_07n[] OLED_FONT_SECTION("oled_font_blipfest_07n");

/* profont */

extern const oled_fntpgm_uint8_t oled_font_profont10[] OLED_FONT_SECTION("oled_font_profont10");
extern const oled_fntpgm_uint8_t oled_font_profont10r[] OLED_FONT_SECTION("oled_font_profont10r");
extern const oled_fntpgm_uint8_t oled_font_profont11[] OLED_FONT_SECTION("oled_font_profont11");
extern const oled_fntpgm_uint8_t oled_font_profont11r[] OLED_FONT_SECTION("oled_font_profont11r");
extern const oled_fntpgm_uint8_t oled_font_profont12[] OLED_FONT_SECTION("oled_font_profont12");
extern const oled_fntpgm_uint8_t oled_font_profont12r[] OLED_FONT_SECTION("oled_font_profont12r");
extern const oled_fntpgm_uint8_t oled_font_profont15[] OLED_FONT_SECTION("oled_font_profont15");
extern const oled_fntpgm_uint8_t oled_font_profont15r[] OLED_FONT_SECTION("oled_font_profont15r");
extern const oled_fntpgm_uint8_t oled_font_profont17[] OLED_FONT_SECTION("oled_font_profont17");
extern const oled_fntpgm_uint8_t oled_font_profont17r[] OLED_FONT_SECTION("oled_font_profont17r");
extern const oled_fntpgm_uint8_t oled_font_profont22[] OLED_FONT_SECTION("oled_font_profont22");
extern const oled_fntpgm_uint8_t oled_font_profont22r[] OLED_FONT_SECTION("oled_font_profont22r");
extern const oled_fntpgm_uint8_t oled_font_profont29[] OLED_FONT_SECTION("oled_font_profont29");
extern const oled_fntpgm_uint8_t oled_font_profont29r[] OLED_FONT_SECTION("oled_font_profont29r");


#ifdef __cplusplus
}
#endif

#endif /* _OLED_H */

