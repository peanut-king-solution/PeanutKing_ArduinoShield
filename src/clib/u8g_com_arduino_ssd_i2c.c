/*
*/

#include "u8g.h"



#define I2C_SLA         (0x3c*2)
//#define I2C_CMD_MODE  0x080
#define I2C_CMD_MODE    0x000
#define I2C_DATA_MODE   0x040

uint8_t u8g_com_arduino_ssd_start_sequence(u8g_t *u8g) {
  /* are we requested to set the a0 state? */
  if ( u8g->pin_list[U8G_PI_SET_A0] == 0 )
    return 1;

  /* setup bus, might be a repeated start */
  if ( u8g_i2c_start(I2C_SLA) == 0 )
    return 0;
  if ( u8g->pin_list[U8G_PI_A0_STATE] == 0 )   {
    if ( u8g_i2c_send_byte(I2C_CMD_MODE) == 0 )
      return 0;
  }
  else {
    if ( u8g_i2c_send_byte(I2C_DATA_MODE) == 0 )
      return 0;
  }

  u8g->pin_list[U8G_PI_SET_A0] = 0;
  return 1;
}

uint8_t u8g_com_arduino_ssd_i2c_fn(u8g_t *u8g, uint8_t msg, uint8_t arg_val, void *arg_ptr) {
  switch(msg) {
    case U8G_COM_MSG_INIT:
      u8g_i2c_init(u8g->pin_list[U8G_PI_I2C_OPTION]);
      break;
    
    case U8G_COM_MSG_STOP:
      break;

    case U8G_COM_MSG_RESET:
      /* Currently disabled, but it could be enable. Previous restrictions have been removed */
      /* u8g_com_arduino_digital_write(u8g, U8G_PI_RESET, arg_val); */
      break;
      
    case U8G_COM_MSG_CHIP_SELECT:
      u8g->pin_list[U8G_PI_A0_STATE] = 0;
      u8g->pin_list[U8G_PI_SET_A0] = 1;		/* force a0 to set again, also forces start condition */
      if ( arg_val == 0 ) {
        /* disable chip, send stop condition */
      	u8g_i2c_stop();
      }
      else {
        /* enable, do nothing: any byte writing will trigger the i2c start */
      }
      break;

    case U8G_COM_MSG_WRITE_BYTE:
      //u8g->pin_list[U8G_PI_SET_A0] = 1;
      if ( u8g_com_arduino_ssd_start_sequence(u8g) == 0 )
	      return u8g_i2c_stop(), 0;
      if ( u8g_i2c_send_byte(arg_val) == 0 )
	      return u8g_i2c_stop(), 0;
      break;
    
    case U8G_COM_MSG_WRITE_SEQ:
      //u8g->pin_list[U8G_PI_SET_A0] = 1;
      if ( u8g_com_arduino_ssd_start_sequence(u8g) == 0 )
	      return u8g_i2c_stop(), 0;
      {
        register uint8_t *ptr = arg_ptr;
        while( arg_val > 0 ) {
          if ( u8g_i2c_send_byte(*ptr++) == 0 )
            return u8g_i2c_stop(), 0;
          arg_val--;
        }
      }
      break;

    case U8G_COM_MSG_WRITE_SEQ_P:
      if ( u8g_com_arduino_ssd_start_sequence(u8g) == 0 )
	      return u8g_i2c_stop(), 0;
      {
        register uint8_t *ptr = arg_ptr;
        while( arg_val > 0 ) {
	        if ( u8g_i2c_send_byte(u8g_pgm_read(ptr)) == 0 )
	          return 0;
          ptr++;
          arg_val--;
        }
      }
      break;
      
    case U8G_COM_MSG_ADDRESS:                     /* define cmd (arg_val = 0) or data mode (arg_val = 1) */
      u8g->pin_list[U8G_PI_A0_STATE] = arg_val;
      u8g->pin_list[U8G_PI_SET_A0] = 1;		/* force a0 to set again */
      break;
  }
  return 1;
}


