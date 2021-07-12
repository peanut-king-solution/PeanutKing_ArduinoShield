/*
  oledScreen.h
  C++ Interface
  Universal 8bit Graphics LibraryN IF 
*/

#ifndef OLEDSCREEN_H
#define OLEDSCREEN_H

#include <Print.h>
#include "oled.h"


class OLEDLIB : public Print {
  private:
    oled_t oled;
    uint8_t tx, ty;          // current position for the Print base class procedures
    uint8_t is_begin;

    void prepare(void) { tx = 0; ty = 0; is_begin = 0; }
    void cbegin(void) { if ( is_begin == 0 ) { is_begin = 1; oled_Begin(&oled); } }
    uint8_t initSPI(oled_dev_t *dev, uint8_t sck, uint8_t mosi, uint8_t cs, uint8_t a0, uint8_t reset = OLED_PIN_NONE);
    uint8_t initHWSPI(oled_dev_t *dev, uint8_t cs, uint8_t a0, uint8_t reset = OLED_PIN_NONE);
    uint8_t initI2C(oled_dev_t *dev, uint8_t options);
  protected:
    uint8_t init8BitFixedPort(oled_dev_t *dev, uint8_t en, uint8_t cs, uint8_t di, uint8_t rw, uint8_t reset);
  private:
    uint8_t init8Bit(oled_dev_t *dev, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7, uint8_t en, uint8_t cs1, uint8_t cs2, uint8_t di, uint8_t rw = OLED_PIN_NONE, uint8_t reset = OLED_PIN_NONE);
    uint8_t initRW8Bit(oled_dev_t *dev, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7, uint8_t cs, uint8_t a0, uint8_t wr, uint8_t rd, uint8_t reset);
  public:
  
    /* constructor */
    OLEDLIB(void)
      { }
    OLEDLIB(oled_dev_t *dev)
      { prepare(); oled_Init(&oled, dev); }
    OLEDLIB(oled_dev_t *dev, oled_com_fnptr com_fn)
      { prepare(); oled_InitComFn(&oled, dev, com_fn); }
    OLEDLIB(oled_dev_t *dev, uint8_t sck, uint8_t mosi, uint8_t cs, uint8_t a0, uint8_t reset) 
      { initSPI(dev, sck, mosi, cs, a0, reset); }
    OLEDLIB(oled_dev_t *dev, uint8_t cs, uint8_t a0, uint8_t reset) 
      { initHWSPI(dev, cs, a0, reset); }
    OLEDLIB(oled_dev_t *dev, uint8_t options) 
      { initI2C(dev, options); }
    OLEDLIB(oled_dev_t *dev, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7, uint8_t en, uint8_t cs1, uint8_t cs2, uint8_t di, uint8_t rw, uint8_t reset) 
      { init8Bit(dev, d0, d1, d2, d3, d4, d5, d6, d7, en, cs1, cs2, di, rw, reset); }
    OLEDLIB(oled_dev_t *dev, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7, uint8_t cs, uint8_t a0, uint8_t wr, uint8_t rd, uint8_t reset) 
      { initRW8Bit(dev, d0, d1, d2, d3, d4, d5, d6, d7, cs, a0, wr, rd, reset); }

    uint8_t begin(void) { is_begin = 1; return oled_Begin(&oled); }
      
    void setPrintPos(uint8_t x, uint8_t y) { tx = x; ty = y; }
    oled_t *getU8g(void) { return &oled; }
    uint8_t getPrintCol(void) { return tx; }	
    uint8_t getPrintRow(void) { return ty; }    
    
    size_t write(uint8_t c) { 
      tx += oled_DrawGlyph(&oled, tx, ty, c);
      return 1;
    }
    
     /* screen rotation */
    void undoRotation(void) { oled_UndoRotation(&oled); }
    void setRot90(void)     { oled_SetRot90(&oled); }
    void setRot180(void)    { oled_SetRot180(&oled); }
    void setRot270(void)    { oled_SetRot270(&oled); }

     /* screen scaling */
    void undoScale(void)    { oled_UndoScale(&oled); }
    void setScale2x2(void)  { oled_SetScale2x2(&oled); }
    
     /* picture loop */
    void firstPage(void)    { cbegin(); oled_FirstPage(&oled); }
    uint8_t nextPage(void)  { return oled_NextPage(&oled); }
    
    /* system commands */
    uint8_t setContrast(uint8_t contrast) { cbegin(); return oled_SetContrast(&oled, contrast); }
    void sleepOn(void) { oled_SleepOn(&oled); }
    void sleepOff(void) { oled_SleepOff(&oled); }
    
    /* graphic primitives */
    void setColorEntry(uint8_t color_index, uint8_t r, uint8_t g, uint8_t b) { oled_SetColorEntry(&oled, color_index, r, g, b); }
    void setHiColor(uint16_t  rgb) { oled_SetHiColor(&oled, rgb); }
    void setHiColorByRGB(uint8_t r, uint8_t g, uint8_t b) { oled_SetHiColorByRGB(&oled, r, g, b); }
    void setRGB(uint8_t r, uint8_t g, uint8_t b) { oled_SetRGB(&oled, r, g, b); }
    
    void setColorIndex(uint8_t color_index) { oled_SetColorIndex(&oled, color_index); }
    uint8_t getColorIndex(void) { return oled_GetColorIndex(&oled); }
    
    void setDefaultForegroundColor(void) { oled_SetDefaultForegroundColor(&oled);  }
    void setDefaultBackgroundColor(void) { oled_SetDefaultBackgroundColor(&oled);  }
    void setDefaultMidColor(void) { oled_SetDefaultMidColor(&oled);  }
    
    uint8_t getWidth(void)  { return oled_GetWidth(&oled); }
    uint8_t getHeight(void) { return oled_GetHeight(&oled); }
    uint8_t getMode(void)   { return oled_GetMode(&oled); }

    void drawPixel(uint8_t x, uint8_t y) { return oled_DrawPixel(&oled, x, y); }
    void drawHLine(uint8_t x, uint8_t y, uint8_t w) { oled_DrawHLine(&oled, x, y, w); }  
    void drawVLine(uint8_t x, uint8_t y, uint8_t h) { oled_DrawVLine(&oled, x, y, h); }
    void drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) { oled_DrawLine(&oled, x1, y1, x2, y2); }
    
    void drawFrame(uint8_t x, uint8_t y, uint8_t w, uint8_t h) { oled_DrawFrame(&oled, x, y, w, h); }
    void drawRFrame(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r) { oled_DrawRFrame(&oled, x, y, w, h,r); }
    void drawBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h) { oled_DrawBox(&oled, x, y, w, h); }
    void drawRBox(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t r) { oled_DrawRBox(&oled, x, y, w, h,r); }

    void drawCircle(uint8_t x0, uint8_t y0, uint8_t rad, uint8_t opt = OLED_DRAW_ALL) { oled_DrawCircle(&oled, x0, y0, rad, opt); }
    void drawDisc(uint8_t x0, uint8_t y0, uint8_t rad, uint8_t opt = OLED_DRAW_ALL) { oled_DrawDisc(&oled, x0, y0, rad, opt); }
     
    void drawEllipse(uint8_t x0, uint8_t y0, uint8_t rx, uint8_t ry, uint8_t opt = OLED_DRAW_ALL) { oled_DrawEllipse(&oled, x0, y0, rx, ry, opt); }
    void drawFilledEllipse(uint8_t x0, uint8_t y0, uint8_t rx, uint8_t ry, uint8_t opt = OLED_DRAW_ALL) { oled_DrawFilledEllipse(&oled, x0, y0, rx, ry, opt); }    

    void drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) 
      { oled_DrawTriangle(&oled, x0, y0, x1, y1, x2, y2); }
      

    
    /* bitmap handling */
    void drawBitmap(uint8_t x, uint8_t y, uint8_t cnt, uint8_t h, const uint8_t *bitmap) 
      { oled_DrawBitmap(&oled, x, y, cnt, h, bitmap); }
    void drawBitmapP(uint8_t x, uint8_t y, uint8_t cnt, uint8_t h, const oled_pgm_uint8_t *bitmap)
      { oled_DrawBitmapP(&oled, x, y, cnt, h, bitmap); }

    void drawXBM(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t *bitmap)
      { oled_DrawXBM(&oled, x, y, w, h, bitmap); }
    void drawXBMP(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const oled_pgm_uint8_t *bitmap)
      { oled_DrawXBMP(&oled, x, y, w, h, bitmap); }
    
      
    /* font handling */
    void setFont(const oled_fntpgm_uint8_t *font) {oled_SetFont(&oled, font); }
    int8_t getFontAscent(void) { return oled_GetFontAscent(&oled); }
    int8_t getFontDescent(void) { return oled_GetFontDescent(&oled); }
    int8_t getFontLineSpacing(void) { return oled_GetFontLineSpacing(&oled); }
    
    uint8_t drawStr(uint8_t x, uint8_t y, const char *s) { return oled_DrawStr(&oled, x, y, s); }
    uint8_t drawStr90(uint8_t x, uint8_t y, const char *s) { return oled_DrawStr90(&oled, x, y, s); }
    uint8_t drawStr180(uint8_t x, uint8_t y, const char *s) { return oled_DrawStr180(&oled, x, y, s); }
    uint8_t drawStr270(uint8_t x, uint8_t y, const char *s) { return oled_DrawStr270(&oled, x, y, s); }
    uint8_t drawStrP(uint8_t x, uint8_t y, const oled_pgm_uint8_t *s) { return oled_DrawStrP(&oled, x, y, s); }
    uint8_t drawStr90P(uint8_t x, uint8_t y, const oled_pgm_uint8_t *s) { return oled_DrawStr90P(&oled, x, y, s); }
    uint8_t drawStr180P(uint8_t x, uint8_t y, const oled_pgm_uint8_t *s) { return oled_DrawStr180P(&oled, x, y, s); }
    uint8_t drawStr270P(uint8_t x, uint8_t y, const oled_pgm_uint8_t *s) { return oled_DrawStr270P(&oled, x, y, s); }
    
    void setFontPosBaseline(void) { oled_SetFontPosBaseline(&oled); }
    void setFontPosBottom(void) { oled_SetFontPosBottom(&oled); }
    void setFontPosCenter(void) { oled_SetFontPosCenter(&oled); }
    void setFontPosTop(void) { oled_SetFontPosTop(&oled); }

    void setFontRefHeightText(void) { oled_SetFontRefHeightText(&oled); }
    void setFontRefHeightExtendedText(void) { oled_SetFontRefHeightExtendedText(&oled); }
    void setFontRefHeightAll(void) { oled_SetFontRefHeightAll(&oled); }  
    void setFontLineSpacingFactor(uint8_t factor) { oled_SetFontLineSpacingFactor(&oled, factor); }


    uint8_t getStrPixelWidth(const char *s) { return oled_GetStrPixelWidth(&oled, s); }
    uint8_t getStrPixelWidthP(oled_pgm_uint8_t *s) { return oled_GetStrPixelWidthP(&oled, s); }
    uint8_t getStrWidth(const char *s) { return oled_GetStrWidth(&oled, s); } 
    uint8_t getStrWidthP(oled_pgm_uint8_t *s) { return oled_GetStrWidthP(&oled, s); }
    
    void setHardwareBackup(oled_state_cb backup_cb) { oled_SetHardwareBackup(&oled, backup_cb); }
    
#if defined(ARDUINO) && ARDUINO >= 100
    // support for the F() macro
    
    uint8_t drawStr(uint8_t x, uint8_t y, const __FlashStringHelper *s) { return oled_DrawStrP(&oled, x, y, (oled_pgm_uint8_t *)s); }
    uint8_t drawStr90(uint8_t x, uint8_t y, const __FlashStringHelper *s) { return oled_DrawStr90P(&oled, x, y, (oled_pgm_uint8_t *)s); }
    uint8_t drawStr180(uint8_t x, uint8_t y, const __FlashStringHelper *s) { return oled_DrawStr180P(&oled, x, y, (oled_pgm_uint8_t *)s); }
    uint8_t drawStr270(uint8_t x, uint8_t y, const __FlashStringHelper *s) { return oled_DrawStr270P(&oled, x, y, (oled_pgm_uint8_t *)s); }
    
    uint8_t getStrPixelWidth(const __FlashStringHelper *s) { return oled_GetStrPixelWidthP(&oled, (oled_pgm_uint8_t *)s); }
    uint8_t getStrWidth(const __FlashStringHelper *s) { return oled_GetStrWidthP(&oled, (oled_pgm_uint8_t *)s); }
#endif

    /* cursor handling */
    void setCursorFont(const oled_pgm_uint8_t *cursor_font) { oled_SetCursorFont(&oled, cursor_font); }
    void setCursorStyle(uint8_t encoding) { oled_SetCursorStyle(&oled, encoding); }
    void setCursorPos(uint8_t cursor_x, uint8_t cursor_y) { oled_SetCursorPos(&oled, cursor_x, cursor_y); }
    void setCursorColor(uint8_t fg, uint8_t bg) { oled_SetCursorColor(&oled, fg, bg); }
    void enableCursor(void) { oled_EnableCursor(&oled); }
    void disableCursor(void) { oled_DisableCursor(&oled); }
    void drawCursor(void)  { oled_DrawCursor(&oled); }
    
    /* virtual screen */
    
    void setVirtualScreenDimension(uint8_t width, uint8_t height) { oled_SetVirtualScreenDimension(&oled, width, height); }
    uint8_t addToVirtualScreen(uint8_t x, uint8_t y, OLEDLIB &child_oled) { return oled_AddToVirtualScreen(&oled, x, y, &child_oled.oled); }

};












class OLEDLIB_SSD1306_128X64 : public OLEDLIB {
 public:
  OLEDLIB_SSD1306_128X64(uint8_t sck, uint8_t mosi, uint8_t cs, uint8_t a0, uint8_t reset = OLED_PIN_NONE) 
    : OLEDLIB(&oled_dev_ssd1306_128x64_sw_spi, sck, mosi, cs, a0, reset)
    { }
  OLEDLIB_SSD1306_128X64(uint8_t cs, uint8_t a0, uint8_t reset = OLED_PIN_NONE) 
    : OLEDLIB(&oled_dev_ssd1306_128x64_hw_spi, cs, a0, reset)
    { }
  OLEDLIB_SSD1306_128X64(uint8_t options = OLED_I2C_OPT_NONE) 
    : OLEDLIB(&oled_dev_ssd1306_128x64_i2c, options)
    { }
};

class OLEDLIB_SSD1306_ADAFRUIT_128X64 : public OLEDLIB {
 public:
  OLEDLIB_SSD1306_ADAFRUIT_128X64(uint8_t sck, uint8_t mosi, uint8_t cs, uint8_t a0, uint8_t reset = OLED_PIN_NONE) 
    : OLEDLIB(&oled_dev_ssd1306_adafruit_128x64_sw_spi, sck, mosi, cs, a0, reset)
    { }
  OLEDLIB_SSD1306_ADAFRUIT_128X64(uint8_t cs, uint8_t a0, uint8_t reset = OLED_PIN_NONE) 
    : OLEDLIB(&oled_dev_ssd1306_adafruit_128x64_hw_spi, cs, a0, reset)
    { }
  OLEDLIB_SSD1306_ADAFRUIT_128X64(uint8_t options = OLED_I2C_OPT_NONE) 
    : OLEDLIB(&oled_dev_ssd1306_adafruit_128x64_i2c, options)
    { }
};

class OLEDLIB_SSD1306_128X64_2X : public OLEDLIB {
 public:
  OLEDLIB_SSD1306_128X64_2X(uint8_t sck, uint8_t mosi, uint8_t cs, uint8_t a0, uint8_t reset = OLED_PIN_NONE) 
    : OLEDLIB(&oled_dev_ssd1306_128x64_2x_sw_spi, sck, mosi, cs, a0, reset)
    { }
  OLEDLIB_SSD1306_128X64_2X(uint8_t cs, uint8_t a0, uint8_t reset = OLED_PIN_NONE) 
    : OLEDLIB(&oled_dev_ssd1306_128x64_2x_hw_spi, cs, a0, reset)
    { }
  OLEDLIB_SSD1306_128X64_2X(uint8_t options = OLED_I2C_OPT_NONE) 
    : OLEDLIB(&oled_dev_ssd1306_128x64_2x_i2c, options)
    { }
};



#endif
