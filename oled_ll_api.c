/*

  oled_ll_api.c
  
  low level api

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

#include <stddef.h>
#include "oled.h"

uint8_t oled_call_dev_fn(oled_t *oled, oled_dev_t *dev, uint8_t msg, void *arg)
{
  return dev->dev_fn(oled, dev, msg, arg); 
}

/*====================================================================*/

uint8_t oled_InitLL(oled_t *oled, oled_dev_t *dev)
{
  uint8_t r;
  oled->state_cb(OLED_STATE_MSG_BACKUP_ENV);
  r =  oled_call_dev_fn(oled, dev, OLED_DEV_MSG_INIT, NULL);
  oled->state_cb(OLED_STATE_MSG_BACKUP_OLED);
  oled->state_cb(OLED_STATE_MSG_RESTORE_ENV);
  return r;
}

void oled_FirstPageLL(oled_t *oled, oled_dev_t *dev)
{  
  oled->state_cb(OLED_STATE_MSG_BACKUP_ENV);
  oled->state_cb(OLED_STATE_MSG_RESTORE_OLED);
  oled_call_dev_fn(oled, dev, OLED_DEV_MSG_PAGE_FIRST, NULL);
  oled_call_dev_fn(oled, dev, OLED_DEV_MSG_GET_PAGE_BOX, &(oled->current_page));
  oled->state_cb(OLED_STATE_MSG_RESTORE_ENV);
}

uint8_t oled_NextPageLL(oled_t *oled, oled_dev_t *dev)
{  
  uint8_t r;
  oled->state_cb(OLED_STATE_MSG_BACKUP_ENV);
  oled->state_cb(OLED_STATE_MSG_RESTORE_OLED);
  r = oled_call_dev_fn(oled, dev, OLED_DEV_MSG_PAGE_NEXT, NULL);
  if ( r != 0 )
  {
    oled_call_dev_fn(oled, dev, OLED_DEV_MSG_GET_PAGE_BOX, &(oled->current_page));
  }
  oled->state_cb(OLED_STATE_MSG_RESTORE_ENV);
  return r;
}

uint8_t oled_SetContrastLL(oled_t *oled, oled_dev_t *dev, uint8_t contrast)
{  
  return oled_call_dev_fn(oled, dev, OLED_DEV_MSG_CONTRAST, &contrast);
}

void oled_DrawPixelLL(oled_t *oled, oled_dev_t *dev, uint8_t x, uint8_t y)
{
  oled_dev_arg_pixel_t *arg = &(oled->arg_pixel);
  arg->x = x;
  arg->y = y;
  oled_call_dev_fn(oled, dev, OLED_DEV_MSG_SET_PIXEL, arg);
}

void oled_Draw8PixelLL(oled_t *oled, oled_dev_t *dev, uint8_t x, uint8_t y, uint8_t dir, uint8_t pixel)
{
  oled_dev_arg_pixel_t *arg = &(oled->arg_pixel);
  arg->x = x;
  arg->y = y;
  arg->dir = dir;
  arg->pixel = pixel;
  oled_call_dev_fn(oled, dev, OLED_DEV_MSG_SET_8PIXEL, arg);
}

void oled_Draw4TPixelLL(oled_t *oled, oled_dev_t *dev, uint8_t x, uint8_t y, uint8_t dir, uint8_t pixel)
{
  oled_dev_arg_pixel_t *arg = &(oled->arg_pixel);
  arg->x = x;
  arg->y = y;
  arg->dir = dir;
  arg->pixel = pixel;
  oled_call_dev_fn(oled, dev, OLED_DEV_MSG_SET_4TPIXEL, arg);
}


#ifdef OLED_DEV_MSG_IS_BBX_INTERSECTION
uint8_t oled_IsBBXIntersectionLL(oled_t *oled, oled_dev_t *dev, uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{  
  return oled_call_dev_fn(oled, dev, OLED_DEV_MSG_IS_BBX_INTERSECTION, &arg);
}
#endif



uint8_t oled_GetWidthLL(oled_t *oled, oled_dev_t *dev)
{
  uint8_t r;
  oled_call_dev_fn(oled, dev, OLED_DEV_MSG_GET_WIDTH, &r);
  return r;
}

uint8_t oled_GetHeightLL(oled_t *oled, oled_dev_t *dev)
{       
  uint8_t r;
  oled_call_dev_fn(oled, dev, OLED_DEV_MSG_GET_HEIGHT, &r);
  return r;
}

uint8_t oled_GetModeLL(oled_t *oled, oled_dev_t *dev)
{       
  return oled_call_dev_fn(oled, dev, OLED_DEV_MSG_GET_MODE, NULL);
}



/*====================================================================*/

void oled_UpdateDimension(oled_t *oled)
{
  oled->width = oled_GetWidthLL(oled, oled->dev);
  oled->height = oled_GetHeightLL(oled, oled->dev);
  oled->mode = oled_GetModeLL(oled, oled->dev);
  /* 9 Dec 2012: oled_scale.c requires update of current page */
  oled_call_dev_fn(oled, oled->dev, OLED_DEV_MSG_GET_PAGE_BOX, &(oled->current_page));
}

static void oled_init_data(oled_t *oled)
{
  oled->font = NULL;
  oled->cursor_font = NULL;
  oled->cursor_bg_color = 0;
  oled->cursor_fg_color = 1;
  oled->cursor_encoding = 34;
  oled->cursor_fn = (oled_draw_cursor_fn)0;

#if defined(OLED_WITH_PINLIST)  
  {
    uint8_t i;
    for( i = 0; i < OLED_PIN_LIST_LEN; i++ )
      oled->pin_list[i] = OLED_PIN_NONE;
  }
#endif
  
  oled_SetColorIndex(oled, 1);

  oled_SetFontPosBaseline(oled);
  
  oled->font_height_mode = OLED_FONT_HEIGHT_MODE_XTEXT;
  oled->font_ref_ascent = 0;
  oled->font_ref_descent = 0;
  oled->font_line_spacing_factor = 64;           /* 64 = 1.0, 77 = 1.2 line spacing factor */
  oled->line_spacing = 0;
  
  oled->state_cb = oled_state_dummy_cb;

}

uint8_t oled_Begin(oled_t *oled)
{
  /* call and init low level driver and com device */
  if ( oled_InitLL(oled, oled->dev) == 0 )
    return 0;
  /* fetch width and height from the low level */
  oled_UpdateDimension(oled);
  return 1;
}

uint8_t oled_Init(oled_t *oled, oled_dev_t *dev)
{
  oled_init_data(oled);
  oled->dev = dev;
  
  /* On the Arduino Environment this will lead to two calls to oled_Begin(), the following line will be called first (by U8glib constructors) */
  /* if - in future releases - this is removed, then still call oled_UpdateDimension() */
  /* if Arduino call oled_UpdateDimension else oled_Begin */
  /* issue 146 */
  return oled_Begin(oled);
}

/* special init for pure ARM systems */
uint8_t oled_InitComFn(oled_t *oled, oled_dev_t *dev, oled_com_fnptr com_fn)
{
  oled_init_data(oled);
  
#if defined(OLED_WITH_PINLIST)  
  {
    uint8_t i;
    for( i = 0; i < OLED_PIN_LIST_LEN; i++ )
      oled->pin_list[i] = OLED_PIN_DUMMY;
  }
#endif
  
  oled->dev = dev;
  
  /* replace the device procedure with a custom communication procedure */
  oled->dev->com_fn = com_fn;
  
  /* On the Arduino Environment this will lead to two calls to oled_Begin(), the following line will be called first (by U8glib constructors) */
  /* if - in future releases - this is removed, then still call oled_UpdateDimension() */
  /* if Arduino call oled_UpdateDimension else oled_Begin */
  /* issue 146 */
  return oled_Begin(oled);
}


#if defined(OLED_WITH_PINLIST)  
uint8_t oled_InitSPI(oled_t *oled, oled_dev_t *dev, uint8_t sck, uint8_t mosi, uint8_t cs, uint8_t a0, uint8_t reset)
{
  
  /* fill data structure with some suitable values */
  oled_init_data(oled);
  oled->dev = dev;
    
  /* assign user pins */
  oled->pin_list[OLED_PI_SCK] = sck;
  oled->pin_list[OLED_PI_MOSI] = mosi;
  oled->pin_list[OLED_PI_CS] = cs;
  oled->pin_list[OLED_PI_A0] = a0;
  oled->pin_list[OLED_PI_RESET] = reset;
  
  /* On the Arduino Environment this will lead to two calls to oled_Begin(), the following line will be called first (by U8glib constructors) */
  /* if - in future releases - this is removed, then still call oled_UpdateDimension() */
  /* if Arduino call oled_UpdateDimension else oled_Begin */
  /* issue 146 */
  return oled_Begin(oled);
}

uint8_t oled_InitHWSPI(oled_t *oled, oled_dev_t *dev, uint8_t cs, uint8_t a0, uint8_t reset)
{
  /* fill data structure with some suitable values */
  oled_init_data(oled);
  oled->dev = dev;
  
  
  /* assign user pins */
  oled->pin_list[OLED_PI_CS] = cs;
  oled->pin_list[OLED_PI_A0] = a0;
  oled->pin_list[OLED_PI_RESET] = reset;
  
  return oled_Begin(oled);
}

uint8_t oled_InitI2C(oled_t *oled, oled_dev_t *dev, uint8_t options)
{
  /* fill data structure with some suitable values */
  oled_init_data(oled);
  oled->dev = dev;
    
  oled->pin_list[OLED_PI_I2C_OPTION] = options;
  
  return oled_Begin(oled);
}


uint8_t oled_Init8BitFixedPort(oled_t *oled, oled_dev_t *dev, uint8_t en, uint8_t cs, uint8_t di, uint8_t rw, uint8_t reset)
{
  
  /* fill data structure with some suitable values */
  oled_init_data(oled);
  oled->dev = dev;
    
  /* assign user pins */

  oled->pin_list[OLED_PI_EN] = en;
  oled->pin_list[OLED_PI_CS] = cs;
  oled->pin_list[OLED_PI_DI] = di;
  oled->pin_list[OLED_PI_RW] = rw;
  oled->pin_list[OLED_PI_RESET] = reset;

  return oled_Begin(oled);
}

uint8_t oled_Init8Bit(oled_t *oled, oled_dev_t *dev, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7, 
  uint8_t en, uint8_t cs1, uint8_t cs2, uint8_t di, uint8_t rw, uint8_t reset)
{
  
  /* fill data structure with some suitable values */
  oled_init_data(oled);
  oled->dev = dev;
    
  /* assign user pins */

  oled->pin_list[OLED_PI_D0] = d0;
  oled->pin_list[OLED_PI_D1] = d1;
  oled->pin_list[OLED_PI_D2] = d2;
  oled->pin_list[OLED_PI_D3] = d3;
  oled->pin_list[OLED_PI_D4] = d4;
  oled->pin_list[OLED_PI_D5] = d5;
  oled->pin_list[OLED_PI_D6] = d6;
  oled->pin_list[OLED_PI_D7] = d7;

  oled->pin_list[OLED_PI_EN] = en;
  oled->pin_list[OLED_PI_CS1] = cs1;
  oled->pin_list[OLED_PI_CS2] = cs2;
  oled->pin_list[OLED_PI_DI] = di;
  oled->pin_list[OLED_PI_RW] = rw;
  oled->pin_list[OLED_PI_RESET] = reset;
  
  return oled_Begin(oled);
}

/*

  PIN_D0 8
  PIN_D1 9
  PIN_D2 10
  PIN_D3 11
  PIN_D4 4
  PIN_D5 5
  PIN_D6 6
  PIN_D7 7

  PIN_CS 14
  PIN_A0 15
  PIN_RESET 16
  PIN_WR 17
  PIN_RD 18
  
  oled_InitRW8Bit(oled, dev, d0, d1, d2, d3, d4, d5, d6, d7, cs, a0, wr, rd, reset)
  oled_InitRW8Bit(oled, dev,  8,  9, 10, 11,  4,  5,  6,  7, 14, 15, 17, 18, 16)

*/

uint8_t oled_InitRW8Bit(oled_t *oled, oled_dev_t *dev, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7, 
  uint8_t cs, uint8_t a0, uint8_t wr, uint8_t rd, uint8_t reset)
{
  
  /* fill data structure with some suitable values */
  oled_init_data(oled);
  oled->dev = dev;
    
  /* assign user pins */

  oled->pin_list[OLED_PI_D0] = d0;
  oled->pin_list[OLED_PI_D1] = d1;
  oled->pin_list[OLED_PI_D2] = d2;
  oled->pin_list[OLED_PI_D3] = d3;
  oled->pin_list[OLED_PI_D4] = d4;
  oled->pin_list[OLED_PI_D5] = d5;
  oled->pin_list[OLED_PI_D6] = d6;
  oled->pin_list[OLED_PI_D7] = d7;

  oled->pin_list[OLED_PI_CS] = cs;
  oled->pin_list[OLED_PI_A0] = a0;
  oled->pin_list[OLED_PI_WR] = wr;
  oled->pin_list[OLED_PI_RD] = rd;
  oled->pin_list[OLED_PI_RESET] = reset;
  
  return oled_Begin(oled);
}
#endif /* defined(OLED_WITH_PINLIST)  */

void oled_FirstPage(oled_t *oled)
{
  oled_FirstPageLL(oled, oled->dev);
}

uint8_t oled_NextPage(oled_t *oled)
{
  if  ( oled->cursor_fn != (oled_draw_cursor_fn)0 )
  {
    oled->cursor_fn(oled);
  }
  return oled_NextPageLL(oled, oled->dev);
}

uint8_t oled_SetContrast(oled_t *oled, uint8_t contrast)
{
  return oled_SetContrastLL(oled, oled->dev, contrast);
}

void oled_SleepOn(oled_t *oled)
{
  oled_call_dev_fn(oled, oled->dev, OLED_DEV_MSG_SLEEP_ON, NULL);
}

void oled_SleepOff(oled_t *oled)
{
  oled_call_dev_fn(oled, oled->dev, OLED_DEV_MSG_SLEEP_OFF, NULL);
}


void oled_DrawPixel(oled_t *oled, uint8_t x, uint8_t y)
{
  oled_DrawPixelLL(oled, oled->dev, x, y);
}

void oled_Draw8Pixel(oled_t *oled, uint8_t x, uint8_t y, uint8_t dir, uint8_t pixel)
{
  oled_Draw8PixelLL(oled, oled->dev, x, y, dir, pixel);
}

void oled_Draw4TPixel(oled_t *oled, uint8_t x, uint8_t y, uint8_t dir, uint8_t pixel)
{
  oled_Draw4TPixelLL(oled, oled->dev, x, y, dir, pixel);
}

void oled_Draw8ColorPixel(oled_t *oled, uint8_t x, uint8_t y, uint8_t colpixel)
{
  oled_dev_arg_pixel_t *arg = &(oled->arg_pixel);
  arg->x = x;
  arg->y = y;
  arg->dir = 0;
  arg->pixel = 0x80;
  arg->color = colpixel;
  oled_call_dev_fn(oled, oled->dev, OLED_DEV_MSG_SET_8PIXEL, arg);
}

/* oled_IsBBXIntersection() has been moved to oled_clip.c */
#ifdef OBSOLETE_CODE
uint8_t oled_IsBBXIntersection(oled_t *oled, uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
  /* new code */
  oled_dev_arg_bbx_t arg;
  arg.x = x;
  arg.y = y;
  arg.w = w;
  arg.h = h;
  return oled_is_box_bbx_intersection(&(oled->current_page), &arg);

  /* old code */
  //return oled_IsBBXIntersectionLL(oled, oled->dev, x, y, w, h);
}
#endif

/*
  idx: index for the palette entry (0..255)
  r: value for red (0..255)
  g: value for green (0..255)
  b: value for blue (0..255)
*/
void oled_SetColorEntry(oled_t *oled, uint8_t idx, uint8_t r, uint8_t g, uint8_t b)
{
  oled_dev_arg_irgb_t irgb;
  irgb.idx = idx;
  irgb.r = r;
  irgb.g = g;
  irgb.b = b;  
  oled_call_dev_fn(oled, oled->dev, OLED_DEV_MSG_SET_COLOR_ENTRY, &irgb);
}

void oled_SetColorIndex(oled_t *oled, uint8_t idx)
{
  oled->arg_pixel.color = idx;
  /*oled->color_index = idx; */ /* must be removed */
}

void oled_SetHiColor(oled_t *oled, uint16_t rgb)
{
  oled->arg_pixel.color = rgb&255;
  oled->arg_pixel.hi_color = rgb>>8;
  /*oled->color_index = idx; */ /* must be removed */
}

void oled_SetHiColorByRGB(oled_t *oled, uint8_t r, uint8_t g, uint8_t b)
{
  
  r &= ~7;
  g >>= 2;
  b >>= 3;
  oled->arg_pixel.color = b;
  oled->arg_pixel.color |= (g & 7) << 5;
  oled->arg_pixel.hi_color = r;
  oled->arg_pixel.hi_color |= (g>>3) & 7;
  
  //oled_SetHiColor(oled, OLED_GET_HICOLOR_BY_RGB(r,g,b));
}

void oled_SetRGB(oled_t *oled, uint8_t r, uint8_t g, uint8_t b)
{
  if ( oled->mode == OLED_MODE_R3G3B2 ) 
  {
    r &= 0x0e0;
    g &= 0x0e0;
    g >>= 3;
    b >>= 6;
    oled->arg_pixel.color = r | g | b;
  }
  else if ( oled->mode == OLED_MODE_HICOLOR )
  {
    oled_SetHiColorByRGB(oled, r,g,b);
  }
  else
  {
    oled->arg_pixel.color = r;
    oled->arg_pixel.hi_color = g;
    oled->arg_pixel.blue = b;
  }
}


uint8_t oled_GetColorIndex(oled_t *oled)
{
  return oled->arg_pixel.color;
}

uint8_t oled_GetDefaultForegroundColor(oled_t *oled)
{
  uint8_t mode;
  mode = oled_GetMode(oled);
  if ( mode == OLED_MODE_R3G3B2 ) 
    return 255;     /* white */
  else if ( oled_GetMode(oled) == OLED_MODE_GRAY2BIT )
    return 3;         /* max intensity */
  /* if ( oled.getMode() == OLED_MODE_BW ) */
  return 1;         /* pixel on */
}

void oled_SetDefaultForegroundColor(oled_t *oled)
{
  if ( oled->mode == OLED_MODE_HICOLOR )
  {
    oled->arg_pixel.color = 0x0ff;
    oled->arg_pixel.hi_color = 0x0ff;
  }
  else
  {
    oled_SetColorIndex(oled, oled_GetDefaultForegroundColor(oled));
  }
}

uint8_t oled_GetDefaultBackgroundColor(oled_t *oled)
{
  return 0;
}

void oled_SetDefaultBackgroundColor(oled_t *oled)
{
  oled_SetColorIndex(oled, oled_GetDefaultBackgroundColor(oled));         /* pixel on / black */
}

uint8_t oled_GetDefaultMidColor(oled_t *oled)
{
  uint8_t mode;
  mode = oled_GetMode(oled);
  if ( mode == OLED_MODE_R3G3B2 ) 
    return 0x06d;     /* gray: 01101101 */
  else if ( mode == OLED_MODE_GRAY2BIT )
    return 1;         /* low mid intensity */
  /* if ( oled.getMode() == OLED_MODE_BW ) */
  return 1;         /* pixel on */
}

void oled_SetDefaultMidColor(oled_t *oled)
{
  oled_SetColorIndex(oled, oled_GetDefaultMidColor(oled));
}




