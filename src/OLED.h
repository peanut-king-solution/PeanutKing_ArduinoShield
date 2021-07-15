/*

  U8glib.h
  
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

#ifndef _OLED_H
#define _OLED_H

#include <Print.h>
#include "clib/u8g.h"

uint8_t u8g_dev_rot_dummy_fn(u8g_t *u8g, u8g_dev_t*dev, uint8_t msg, void *arg);
extern u8g_dev_t u8g_dev_rot;


class OLED : public Print {
  private:
    u8g_t u8g;
    u8g_uint_t tx, ty;          // current position for the Print base class procedures
    uint8_t is_begin;

    void prepare(void) { tx = 0; ty = 0; is_begin = 0; }
    void cbegin(void) { if ( is_begin == 0 ) { is_begin = 1; u8g_Begin(&u8g); } }
    uint8_t initI2C(u8g_dev_t *dev, uint8_t options);
  protected:
    uint8_t init8BitFixedPort(u8g_dev_t *dev, uint8_t en, uint8_t cs, uint8_t di, uint8_t rw, uint8_t reset);

  public:
    /* constructor */
    OLED(void)
      { }
    OLED(u8g_dev_t *dev)
      { prepare(); u8g_Init(&u8g, dev); }
    OLED(u8g_dev_t *dev, u8g_com_fnptr com_fn)
      { prepare(); u8g_InitComFn(&u8g, dev, com_fn); }
    OLED(u8g_dev_t *dev, uint8_t options) 
      { initI2C(dev, options); }

    uint8_t begin(void) { is_begin = 1; return u8g_Begin(&u8g); }
      
    void setPrintPos(u8g_uint_t x, u8g_uint_t y) { tx = x; ty = y; }
    u8g_t *getU8g(void) { return &u8g; }
    u8g_uint_t getPrintCol(void) { return tx; }	
    u8g_uint_t getPrintRow(void) { return ty; }    
    
    size_t write(uint8_t c) { 
      u8g_uint_t y = ty + u8g.font_calc_vref(&u8g);
      tx += u8g_draw_glyph(&u8g, tx, y, c);
      return 1;
    }

     /* screen rotation */
    void undoRotation(void) { 
      if ( u8g.dev != &u8g_dev_rot )
        return;
      u8g.dev = u8g_dev_rot.dev_mem;
      u8g_UpdateDimension(&u8g);
    }
    void setRot90(void) {
      if ( u8g.dev != &u8g_dev_rot ) {
        u8g_dev_rot.dev_mem = u8g.dev;
        u8g.dev = &u8g_dev_rot;
      }
      u8g_dev_rot.dev_fn = u8g_dev_rot90_fn;
      u8g_UpdateDimension(&u8g);
    }
    void setRot180(void) {
      if ( u8g.dev != &u8g_dev_rot ) {
        u8g_dev_rot.dev_mem = u8g.dev;
        u8g.dev = &u8g_dev_rot;
      }
      u8g_dev_rot.dev_fn = u8g_dev_rot180_fn;
      u8g_UpdateDimension(&u8g);
    }
    void setRot270(void) { 
      if ( u8g.dev != &u8g_dev_rot ) {
        u8g_dev_rot.dev_mem = u8g.dev;
        u8g.dev = &u8g_dev_rot;
      }
      u8g_dev_rot.dev_fn = u8g_dev_rot270_fn;
      u8g_UpdateDimension(&u8g);
    }

     /* screen scaling */
    void undoScale(void) { u8g_UndoScale(&u8g); }
    void setScale2x2(void) { u8g_SetScale2x2(&u8g); }
    
     /* picture loop */
    void firstPage(void) { cbegin(); u8g_FirstPage(&u8g); }
    uint8_t nextPage(void) { return u8g_NextPage(&u8g); }
    
    /* system commands */
    uint8_t setContrast(uint8_t contrast) { cbegin(); return u8g_SetContrast(&u8g, contrast); }
    void sleepOn(void) { u8g_SleepOn(&u8g); }
    void sleepOff(void) { u8g_SleepOff(&u8g); }
    
    /* graphic primitives */
    void setColorEntry(uint8_t color_index, uint8_t r, uint8_t g, uint8_t b) { u8g_SetColorEntry(&u8g, color_index, r, g, b); }
    void setHiColor(uint16_t  rgb) { u8g_SetHiColor(&u8g, rgb); }
    void setHiColorByRGB(uint8_t r, uint8_t g, uint8_t b) { u8g_SetHiColorByRGB(&u8g, r, g, b); }
    void setRGB(uint8_t r, uint8_t g, uint8_t b) { u8g_SetRGB(&u8g, r, g, b); }
    
    void setColorIndex(uint8_t color_index) { u8g_SetColorIndex(&u8g, color_index); }
    uint8_t getColorIndex(void) { return u8g_GetColorIndex(&u8g); }
    
    void setDefaultForegroundColor(void) { u8g_SetDefaultForegroundColor(&u8g);  }
    void setDefaultBackgroundColor(void) { u8g_SetDefaultBackgroundColor(&u8g);  }
    void setDefaultMidColor(void) { u8g_SetDefaultMidColor(&u8g);  }
    
    u8g_uint_t getWidth(void) { return u8g_GetWidth(&u8g); }
    u8g_uint_t getHeight(void) { return u8g_GetHeight(&u8g); }
    uint8_t getMode(void) { return u8g_GetMode(&u8g); }

    void drawPixel(u8g_uint_t x, u8g_uint_t y) { return u8g_DrawPixel(&u8g, x, y); }
    void drawHLine(u8g_uint_t x, u8g_uint_t y, u8g_uint_t w) { u8g_DrawHLine(&u8g, x, y, w); }  
    void drawVLine(u8g_uint_t x, u8g_uint_t y, u8g_uint_t h) { u8g_DrawVLine(&u8g, x, y, h); }
    void drawLine(u8g_uint_t x1, u8g_uint_t y1, u8g_uint_t x2, u8g_uint_t y2) { u8g_DrawLine(&u8g, x1, y1, x2, y2); }
    
    void drawFrame(u8g_uint_t x, u8g_uint_t y, u8g_uint_t w, u8g_uint_t h) { u8g_DrawFrame(&u8g, x, y, w, h); }
    void drawRFrame(u8g_uint_t x, u8g_uint_t y, u8g_uint_t w, u8g_uint_t h, u8g_uint_t r) { u8g_DrawRFrame(&u8g, x, y, w, h,r); }
    void drawBox(u8g_uint_t x, u8g_uint_t y, u8g_uint_t w, u8g_uint_t h) { u8g_DrawBox(&u8g, x, y, w, h); }
    void drawRBox(u8g_uint_t x, u8g_uint_t y, u8g_uint_t w, u8g_uint_t h, u8g_uint_t r) { u8g_DrawRBox(&u8g, x, y, w, h,r); }

    void drawCircle(u8g_uint_t x0, u8g_uint_t y0, u8g_uint_t rad, uint8_t opt = U8G_DRAW_ALL) { u8g_DrawCircle(&u8g, x0, y0, rad, opt); }
    void drawDisc(u8g_uint_t x0, u8g_uint_t y0, u8g_uint_t rad, uint8_t opt = U8G_DRAW_ALL) { u8g_DrawDisc(&u8g, x0, y0, rad, opt); }
     
    void drawEllipse(u8g_uint_t x0, u8g_uint_t y0, u8g_uint_t rx, u8g_uint_t ry, uint8_t opt = U8G_DRAW_ALL) { u8g_DrawEllipse(&u8g, x0, y0, rx, ry, opt); }
    void drawFilledEllipse(u8g_uint_t x0, u8g_uint_t y0, u8g_uint_t rx, u8g_uint_t ry, uint8_t opt = U8G_DRAW_ALL) { u8g_DrawFilledEllipse(&u8g, x0, y0, rx, ry, opt); }    

    void drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) 
      { u8g_DrawTriangle(&u8g, x0, y0, x1, y1, x2, y2); }
      

    
    /* bitmap handling */
    void drawBitmap(u8g_uint_t x, u8g_uint_t y, u8g_uint_t cnt, u8g_uint_t h, const uint8_t *bitmap) 
      { u8g_DrawBitmap(&u8g, x, y, cnt, h, bitmap); }
    void drawBitmapP(u8g_uint_t x, u8g_uint_t y, u8g_uint_t cnt, u8g_uint_t h, const u8g_pgm_uint8_t *bitmap)
      { u8g_DrawBitmapP(&u8g, x, y, cnt, h, bitmap); }

    void drawXBM(u8g_uint_t x, u8g_uint_t y, u8g_uint_t w, u8g_uint_t h, const uint8_t *bitmap)
      { u8g_DrawXBM(&u8g, x, y, w, h, bitmap); }
    void drawXBMP(u8g_uint_t x, u8g_uint_t y, u8g_uint_t w, u8g_uint_t h, const u8g_pgm_uint8_t *bitmap)
      { u8g_DrawXBMP(&u8g, x, y, w, h, bitmap); }
    
      
    /* font handling */
    void setFont(const u8g_fntpgm_uint8_t *font) {u8g_SetFont(&u8g, font); }
    int8_t getFontAscent(void) { return u8g_GetFontAscent(&u8g); }
    int8_t getFontDescent(void) { return u8g_GetFontDescent(&u8g); }
    int8_t getFontLineSpacing(void) { return u8g_GetFontLineSpacing(&u8g); }
    
    u8g_uint_t drawStr(u8g_uint_t x, u8g_uint_t y, const char *s) { return u8g_DrawStr(&u8g, x, y, s); }
    u8g_uint_t drawStr90(u8g_uint_t x, u8g_uint_t y, const char *s) { return u8g_DrawStr90(&u8g, x, y, s); }
    u8g_uint_t drawStr180(u8g_uint_t x, u8g_uint_t y, const char *s) { return u8g_DrawStr180(&u8g, x, y, s); }
    u8g_uint_t drawStr270(u8g_uint_t x, u8g_uint_t y, const char *s) { return u8g_DrawStr270(&u8g, x, y, s); }
    u8g_uint_t drawStrP(u8g_uint_t x, u8g_uint_t y, const u8g_pgm_uint8_t *s) { return u8g_DrawStrP(&u8g, x, y, s); }
    u8g_uint_t drawStr90P(u8g_uint_t x, u8g_uint_t y, const u8g_pgm_uint8_t *s) { return u8g_DrawStr90P(&u8g, x, y, s); }
    u8g_uint_t drawStr180P(u8g_uint_t x, u8g_uint_t y, const u8g_pgm_uint8_t *s) { return u8g_DrawStr180P(&u8g, x, y, s); }
    u8g_uint_t drawStr270P(u8g_uint_t x, u8g_uint_t y, const u8g_pgm_uint8_t *s) { return u8g_DrawStr270P(&u8g, x, y, s); }
    
    void setFontPosBaseline(void) { u8g_SetFontPosBaseline(&u8g); }
    void setFontPosBottom(void) { u8g_SetFontPosBottom(&u8g); }
    void setFontPosCenter(void) { u8g_SetFontPosCenter(&u8g); }
    void setFontPosTop(void) { u8g_SetFontPosTop(&u8g); }

    void setFontRefHeightText(void) { u8g_SetFontRefHeightText(&u8g); }
    void setFontRefHeightExtendedText(void) { u8g_SetFontRefHeightExtendedText(&u8g); }
    void setFontRefHeightAll(void) { u8g_SetFontRefHeightAll(&u8g); }  
    void setFontLineSpacingFactor(uint8_t factor) { u8g_SetFontLineSpacingFactor(&u8g, factor); }


    u8g_uint_t getStrPixelWidth(const char *s) { return u8g_GetStrPixelWidth(&u8g, s); }
    u8g_uint_t getStrPixelWidthP(u8g_pgm_uint8_t *s) { return u8g_GetStrPixelWidthP(&u8g, s); }
    u8g_uint_t getStrWidth(const char *s) { return u8g_GetStrWidth(&u8g, s); } 
    u8g_uint_t getStrWidthP(u8g_pgm_uint8_t *s) { return u8g_GetStrWidthP(&u8g, s); }
    
    void setHardwareBackup(u8g_state_cb backup_cb) { u8g_SetHardwareBackup(&u8g, backup_cb); }
    
    u8g_uint_t drawStr(u8g_uint_t x, u8g_uint_t y, const __FlashStringHelper *s) { return u8g_DrawStrP(&u8g, x, y, (u8g_pgm_uint8_t *)s); }
    u8g_uint_t drawStr90(u8g_uint_t x, u8g_uint_t y, const __FlashStringHelper *s) { return u8g_DrawStr90P(&u8g, x, y, (u8g_pgm_uint8_t *)s); }
    u8g_uint_t drawStr180(u8g_uint_t x, u8g_uint_t y, const __FlashStringHelper *s) { return u8g_DrawStr180P(&u8g, x, y, (u8g_pgm_uint8_t *)s); }
    u8g_uint_t drawStr270(u8g_uint_t x, u8g_uint_t y, const __FlashStringHelper *s) { return u8g_DrawStr270P(&u8g, x, y, (u8g_pgm_uint8_t *)s); }
    
    u8g_uint_t getStrPixelWidth(const __FlashStringHelper *s) { return u8g_GetStrPixelWidthP(&u8g, (u8g_pgm_uint8_t *)s); }
    u8g_uint_t getStrWidth(const __FlashStringHelper *s) { return u8g_GetStrWidthP(&u8g, (u8g_pgm_uint8_t *)s); }

    /* cursor handling */
    void setCursorFont(const u8g_pgm_uint8_t *cursor_font) { u8g_SetCursorFont(&u8g, cursor_font); }
    void setCursorStyle(uint8_t encoding) { u8g_SetCursorStyle(&u8g, encoding); }
    void setCursorPos(u8g_uint_t cursor_x, u8g_uint_t cursor_y) { u8g_SetCursorPos(&u8g, cursor_x, cursor_y); }
    void setCursorColor(uint8_t fg, uint8_t bg) { u8g_SetCursorColor(&u8g, fg, bg); }
    void enableCursor(void) { u8g_EnableCursor(&u8g); }
    void disableCursor(void) { u8g_DisableCursor(&u8g); }
    void drawCursor(void)  { u8g_DrawCursor(&u8g); }
    
    /* virtual screen */
    
    void setVirtualScreenDimension(u8g_uint_t width, u8g_uint_t height) { u8g_SetVirtualScreenDimension(&u8g, width, height); }
    uint8_t addToVirtualScreen(u8g_uint_t x, u8g_uint_t y, OLED &child_u8g) { return u8g_AddToVirtualScreen(&u8g, x, y, &child_u8g.u8g); }

};



class OLED_SSD1306_128X64 : public OLED {
  public:
    OLED_SSD1306_128X64(uint8_t options = U8G_I2C_OPT_NONE) 
      : OLED(&u8g_dev_ssd1306_128x64_i2c, options)
      { }
};

class OLED_SSD1306_ADAFRUIT_128X64 : public OLED {
  public:
    OLED_SSD1306_ADAFRUIT_128X64(uint8_t options = U8G_I2C_OPT_NONE) 
      : OLED(&u8g_dev_ssd1306_adafruit_128x64_i2c, options)
      { }
};


class OLED_SSD1306_128X64_2X : public OLED {
  public:
    OLED_SSD1306_128X64_2X(uint8_t options = U8G_I2C_OPT_NONE) 
      : OLED(&u8g_dev_ssd1306_128x64_2x_i2c, options)
      { }
};


#endif /* _CPP_OLED */
