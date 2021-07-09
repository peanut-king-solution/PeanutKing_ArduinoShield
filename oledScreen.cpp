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

#include "oledScreen.h"


#if defined(U8G_WITH_PINLIST)
uint8_t U8GLIB::initSPI(u8g_dev_t *dev, uint8_t sck, uint8_t mosi, uint8_t cs, uint8_t a0, uint8_t reset) {
  prepare();
  return u8g_InitSPI(&u8g, dev, sck, mosi, cs, a0, reset);
}

uint8_t U8GLIB::initHWSPI(u8g_dev_t *dev, uint8_t cs, uint8_t a0, uint8_t reset) {
  prepare();
  return u8g_InitHWSPI(&u8g, dev, cs, a0, reset);
}

uint8_t U8GLIB::initI2C(u8g_dev_t *dev, uint8_t options) {
  prepare();
  return u8g_InitI2C(&u8g, dev, options);
}

uint8_t U8GLIB::init8Bit(u8g_dev_t *dev, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7, uint8_t en, uint8_t cs1, uint8_t cs2, uint8_t di, uint8_t rw, uint8_t reset) {
  prepare();
  return u8g_Init8Bit(&u8g, dev, d0, d1, d2, d3, d4, d5, d6, d7, en, cs1, cs2, di, rw, reset); 
}

uint8_t U8GLIB::init8BitFixedPort(u8g_dev_t *dev, uint8_t en, uint8_t cs, uint8_t di, uint8_t rw, uint8_t reset) {
  prepare();
  return u8g_Init8BitFixedPort(&u8g, dev, en, cs, di, rw, reset);
}

uint8_t U8GLIB::initRW8Bit(u8g_dev_t *dev, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3, uint8_t d4, uint8_t d5, uint8_t d6, uint8_t d7, uint8_t cs, uint8_t a0, uint8_t wr, uint8_t rd, uint8_t reset) {
  prepare();
  return u8g_InitRW8Bit(&u8g, dev, d0, d1, d2, d3, d4, d5, d6, d7, cs, a0, wr, rd, reset); 
}
#endif



/*======================================== Circle ======================================*/
/*======================================================================================*/


static void u8g_draw_circle_section(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t x0, u8g_uint_t y0, uint8_t option) U8G_NOINLINE;

static void u8g_draw_circle_section(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t x0, u8g_uint_t y0, uint8_t option) {
  if ( option & U8G_DRAW_UPPER_RIGHT ) {  /* upper right */
    u8g_DrawPixel(u8g, x0 + x, y0 - y);
    u8g_DrawPixel(u8g, x0 + y, y0 - x);
  } else
  if ( option & U8G_DRAW_UPPER_LEFT ) {   /* upper left */
    u8g_DrawPixel(u8g, x0 - x, y0 - y);
    u8g_DrawPixel(u8g, x0 - y, y0 - x);
  } else
  if ( option & U8G_DRAW_LOWER_RIGHT ) {  /* lower right */
    u8g_DrawPixel(u8g, x0 + x, y0 + y);
    u8g_DrawPixel(u8g, x0 + y, y0 + x);
  } else
  if ( option & U8G_DRAW_LOWER_LEFT ) {   /* lower left */
    u8g_DrawPixel(u8g, x0 - x, y0 + y);
    u8g_DrawPixel(u8g, x0 - y, y0 + x);
  }
}

void u8g_draw_circle(u8g_t *u8g, u8g_uint_t x0, u8g_uint_t y0, u8g_uint_t rad, uint8_t option) {
  u8g_int_t f     = 1 - rad;
  u8g_int_t ddF_x = 1;
  u8g_int_t ddF_y = - rad*2;
  u8g_uint_t x    = 0;
  u8g_uint_t y    = rad;

  u8g_draw_circle_section(u8g, x, y, x0, y0, option);
  
  while ( x < y ) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    u8g_draw_circle_section(u8g, x, y, x0, y0, option);    
  }
}

void u8g_DrawCircle(u8g_t *u8g, u8g_uint_t x0, u8g_uint_t y0, u8g_uint_t rad, uint8_t option) {
  /* check for bounding box */
  {
    u8g_uint_t radp, radp2;
    
    radp = rad;
    radp++;
    radp2 = radp;
    radp2 *= 2;
    
    if ( u8g_IsBBXIntersection(u8g, x0-radp, y0-radp, radp2, radp2) == 0)
      return;    
  }
  /* draw circle */
  u8g_draw_circle(u8g, x0, y0, rad, option);
}

static void u8g_draw_disc_section(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t x0, u8g_uint_t y0, uint8_t option) U8G_NOINLINE;

static void u8g_draw_disc_section(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t x0, u8g_uint_t y0, uint8_t option) {
  if ( option & U8G_DRAW_UPPER_RIGHT ) {  /* upper right */
    u8g_DrawVLine(u8g, x0+x, y0-y, y+1);
    u8g_DrawVLine(u8g, x0+y, y0-x, x+1);
  } else
  if ( option & U8G_DRAW_UPPER_LEFT ) {   /* upper left */
    u8g_DrawVLine(u8g, x0-x, y0-y, y+1);
    u8g_DrawVLine(u8g, x0-y, y0-x, x+1);
  } else
  if ( option & U8G_DRAW_LOWER_RIGHT ) {  /* lower right */
    u8g_DrawVLine(u8g, x0+x, y0, y+1);
    u8g_DrawVLine(u8g, x0+y, y0, x+1);
  } else
  if ( option & U8G_DRAW_LOWER_LEFT ) {   /* lower left */
    u8g_DrawVLine(u8g, x0-x, y0, y+1);
    u8g_DrawVLine(u8g, x0-y, y0, x+1);
  }
}

void u8g_draw_disc(u8g_t *u8g, u8g_uint_t x0, u8g_uint_t y0, u8g_uint_t rad, uint8_t option) {
  u8g_int_t f     = 1 - rad;
  u8g_int_t ddF_x = 1;
  u8g_int_t ddF_y = - rad*2;
  u8g_uint_t x    = 0;
  u8g_uint_t y    = rad;

  u8g_draw_disc_section(u8g, x, y, x0, y0, option);
  
  while ( x < y ) {
    if (f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    u8g_draw_disc_section(u8g, x, y, x0, y0, option);    
  }
}

void u8g_DrawDisc(u8g_t *u8g, u8g_uint_t x0, u8g_uint_t y0, u8g_uint_t rad, uint8_t option) {
  /* check for bounding box */
  {
    u8g_uint_t radp, radp2;
    
    radp = rad;
    radp++;
    radp2 = radp;
    radp2 *= 2;
    
    if ( u8g_IsBBXIntersection(u8g, x0-radp, y0-radp, radp2, radp2) == 0)
      return;    
  }
  
  /* draw disc */
  u8g_draw_disc(u8g, x0, y0, rad, option);
}


/*======================================== Bitmap ======================================*/
/*======================================================================================*/
void u8g_DrawHBitmap(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t cnt, const uint8_t *bitmap) {
  while( cnt > 0 ) {
    u8g_Draw8Pixel(u8g, x, y, 0, *bitmap);
    bitmap++;
    cnt--;
    x+=8;
  }
}

void u8g_DrawBitmap(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t cnt, u8g_uint_t h, const uint8_t *bitmap) {
  if ( u8g_IsBBXIntersection(u8g, x, y, cnt*8, h) == 0 )
    return;
  while( h > 0 ) {
    u8g_DrawHBitmap(u8g, x, y, cnt, bitmap);
    bitmap += cnt;
    y++;
    h--;
  }
}


void u8g_DrawHBitmapP(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t cnt, const u8g_pgm_uint8_t *bitmap) {
  while( cnt > 0 ) {
    u8g_Draw8Pixel(u8g, x, y, 0, u8g_pgm_read(bitmap));
    bitmap++;
    cnt--;
    x+=8;
  }
}

void u8g_DrawBitmapP(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t cnt, u8g_uint_t h, const u8g_pgm_uint8_t *bitmap) {
  if ( u8g_IsBBXIntersection(u8g, x, y, cnt*8, h) == 0 )
    return;
  while( h > 0 ) {
    u8g_DrawHBitmapP(u8g, x, y, cnt, bitmap);
    bitmap += cnt;
    y++;
    h--;
  }
}

/*=========================================================================*/

static void u8g_DrawHXBM(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t w, const uint8_t *bitmap) {
  uint8_t d;
  x+=7;
  while( w >= 8 ) {
    u8g_Draw8Pixel(u8g, x, y, 2, *bitmap);
    bitmap++;
    w-= 8;
    x+=8;
  }
  if ( w > 0 ) {
    d = *bitmap;
    x -= 7;
    do {
      if ( d & 1 )
        u8g_DrawPixel(u8g, x, y);
      x++;
      w--;
      d >>= 1;      
    } while ( w > 0 );
  }
}

void u8g_DrawXBM(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t w, u8g_uint_t h, const uint8_t *bitmap) {
  u8g_uint_t b;
  b = w;
  b += 7;
  b >>= 3;
  
  if ( u8g_IsBBXIntersection(u8g, x, y, w, h) == 0 )
    return;
  
  while( h > 0 ) {
    u8g_DrawHXBM(u8g, x, y, w, bitmap);
    bitmap += b;
    y++;
    h--;
  }
}

static void u8g_DrawHXBMP(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t w, const u8g_pgm_uint8_t *bitmap) {
  uint8_t d;
  x+=7;
  while( w >= 8 ) {
    u8g_Draw8Pixel(u8g, x, y, 2, u8g_pgm_read(bitmap));
    bitmap++;
    w-= 8;
    x+=8;
  }
  if ( w > 0 ) {
    d = u8g_pgm_read(bitmap);
    x -= 7;
    do {
      if ( d & 1 )
        u8g_DrawPixel(u8g, x, y);
      x++;
      w--;
      d >>= 1;      
    } while ( w > 0 );
  }
}

void u8g_DrawXBMP(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t w, u8g_uint_t h, const u8g_pgm_uint8_t *bitmap) {
  u8g_uint_t b;
  b = w;
  b += 7;
  b >>= 3;
  
  if ( u8g_IsBBXIntersection(u8g, x, y, w, h) == 0 )
    return;
  while( h > 0 ) {
    u8g_DrawHXBMP(u8g, x, y, w, bitmap);
    bitmap += b;
    y++;
    h--;
  }
}



/*===================================== Rectangle ======================================*/
/*======================================================================================*/

void u8g_draw_hline(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t w) {
  uint8_t pixel = 0x0ff;
  while( w >= 8 ) {
    u8g_Draw8Pixel(u8g, x, y, 0, pixel);
    w-=8;
    x+=8;
  }
  if ( w != 0 ) {
    w ^=7;
    w++;
    pixel <<= w&7;
    u8g_Draw8Pixel(u8g, x, y, 0, pixel);
  }
}

void u8g_draw_vline(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t h) {}
  uint8_t pixel = 0x0ff;
  while( h >= 8 ) {
    u8g_Draw8Pixel(u8g, x, y, 1, pixel);
    h-=8;
    y+=8;
  }
  if ( h != 0 ) {
    h ^=7;
    h++;
    pixel <<= h&7;
    u8g_Draw8Pixel(u8g, x, y, 1, pixel);
  }
}

void u8g_DrawHLine(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t w) {
  if ( u8g_IsBBXIntersection(u8g, x, y, w, 1) == 0 )
    return;
  u8g_draw_hline(u8g, x, y, w);
}

void u8g_DrawVLine(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t w) {
  if ( u8g_IsBBXIntersection(u8g, x, y, 1, w) == 0 )
    return;
  u8g_draw_vline(u8g, x, y, w);
}

/* restrictions: w > 0 && h > 0 */
void u8g_DrawFrame(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t w, u8g_uint_t h) {
  u8g_uint_t xtmp = x;
  
  if ( u8g_IsBBXIntersection(u8g, x, y, w, h) == 0 )
    return;

  u8g_draw_hline(u8g, x, y, w);
  u8g_draw_vline(u8g, x, y, h);
  x+=w;
  x--;
  u8g_draw_vline(u8g, x, y, h);
  y+=h;
  y--;
  u8g_draw_hline(u8g, xtmp, y, w);
}

void u8g_draw_box(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t w, u8g_uint_t h) {
  do {
    u8g_draw_hline(u8g, x, y, w);
    y++;    
    h--;
  } while( h != 0 );
}

/* restrictions: h > 0 */
void u8g_DrawBox(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t w, u8g_uint_t h) {
  if ( u8g_IsBBXIntersection(u8g, x, y, w, h) == 0 )
    return;
  u8g_draw_box(u8g, x, y, w, h);
}


void u8g_DrawRFrame(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t w, u8g_uint_t h, u8g_uint_t r) {
  u8g_uint_t xl, yu;

  if ( u8g_IsBBXIntersection(u8g, x, y, w, h) == 0 )
    return;

  xl = x;
  xl += r;
  yu = y;
  yu += r;
 
  {
    u8g_uint_t yl, xr;
      
    xr = x;
    xr += w;
    xr -= r;
    xr -= 1;
    
    yl = y;
    yl += h;
    yl -= r; 
    yl -= 1;

    u8g_draw_circle(u8g, xl, yu, r, U8G_DRAW_UPPER_LEFT);
    u8g_draw_circle(u8g, xr, yu, r, U8G_DRAW_UPPER_RIGHT);
    u8g_draw_circle(u8g, xl, yl, r, U8G_DRAW_LOWER_LEFT);
    u8g_draw_circle(u8g, xr, yl, r, U8G_DRAW_LOWER_RIGHT);
  }

  {
    u8g_uint_t ww, hh;

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
    u8g_draw_hline(u8g, xl, y, ww);
    u8g_draw_hline(u8g, xl, y+h, ww);
    u8g_draw_vline(u8g, x,         yu, hh);
    u8g_draw_vline(u8g, x+w, yu, hh);
  }
}

void u8g_DrawRBox(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t w, u8g_uint_t h, u8g_uint_t r) {
  u8g_uint_t xl, yu;
    u8g_uint_t yl, xr;

  if ( u8g_IsBBXIntersection(u8g, x, y, w, h) == 0 )
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

  u8g_draw_disc(u8g, xl, yu, r, U8G_DRAW_UPPER_LEFT);
  u8g_draw_disc(u8g, xr, yu, r, U8G_DRAW_UPPER_RIGHT);
  u8g_draw_disc(u8g, xl, yl, r, U8G_DRAW_LOWER_LEFT);
  u8g_draw_disc(u8g, xr, yl, r, U8G_DRAW_LOWER_RIGHT);

  {
    u8g_uint_t ww, hh;

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
    u8g_draw_box(u8g, xl, y, ww, r+1);
    u8g_draw_box(u8g, xl, yl, ww, r+1);
    //u8g_draw_hline(u8g, xl, y+h, ww);
    u8g_draw_box(u8g, x, yu, w, hh);
    //u8g_draw_vline(u8g, x+w, yu, hh);
  }
}










