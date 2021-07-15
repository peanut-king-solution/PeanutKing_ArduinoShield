/*

  u8g_ellipse.c

  Utility to draw empty and filled ellipses.
  
  Universal 8bit Graphics Library
  
  Copyright (c) 2011, bjthom@gmail.com
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
  
  Addition to the U8G Library as of 02/29/12
  Adapted from Bresenham's Algorithm and the following websites:
	http://free.pages.at/easyfilter/bresenham.html
	http://homepage.smc.edu/kennedy_john/belipse.pdf
  
*/

#include "u8g.h"

#if defined(U8G_16BIT)
typedef  int32_t u8g_long_t;
#else
typedef  int16_t u8g_long_t;
#endif


/*
  Source: 
    ftp://pc.fk0.name/pub/books/programming/bezier-ellipse.pdf
    Foley, Computer Graphics, p 90
*/
static void u8g_draw_ellipse_section(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t x0, u8g_uint_t y0, uint8_t option) U8G_NOINLINE;
static void u8g_draw_ellipse_section(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t x0, u8g_uint_t y0, uint8_t option)
{
    /* upper right */
    if ( option & U8G_DRAW_UPPER_RIGHT )
    {
      u8g_DrawPixel(u8g, x0 + x, y0 - y);
    }
    
    /* upper left */
    if ( option & U8G_DRAW_UPPER_LEFT )
    {
      u8g_DrawPixel(u8g, x0 - x, y0 - y);
    }
    
    /* lower right */
    if ( option & U8G_DRAW_LOWER_RIGHT )
    {
      u8g_DrawPixel(u8g, x0 + x, y0 + y);
    }
    
    /* lower left */
    if ( option & U8G_DRAW_LOWER_LEFT )
    {
      u8g_DrawPixel(u8g, x0 - x, y0 + y);
    }
}

void u8g_draw_ellipse(u8g_t *u8g, u8g_uint_t x0, u8g_uint_t y0, u8g_uint_t rx, u8g_uint_t ry, uint8_t option)
{
  u8g_uint_t x, y;
  u8g_long_t xchg, ychg;
  u8g_long_t err;
  u8g_long_t rxrx2;
  u8g_long_t ryry2;
  u8g_long_t stopx, stopy;
  
  rxrx2 = rx;
  rxrx2 *= rx;
  rxrx2 *= 2;
  
  ryry2 = ry;
  ryry2 *= ry;
  ryry2 *= 2;
  
  x = rx;
  y = 0;
  
  xchg = 1;
  xchg -= rx;
  xchg -= rx;
  xchg *= ry;
  xchg *= ry;
  
  ychg = rx;
  ychg *= rx;
  
  err = 0;
  
  stopx = ryry2;
  stopx *= rx;
  stopy = 0;
  
  while( stopx >= stopy )
  {
    u8g_draw_ellipse_section(u8g, x, y, x0, y0, option);
    y++;
    stopy += rxrx2;
    err += ychg;
    ychg += rxrx2;
    if ( 2*err+xchg > 0 )
    {
      x--;
      stopx -= ryry2;
      err += xchg;
      xchg += ryry2;      
    }
  }

  x = 0;
  y = ry;
  
  xchg = ry;
  xchg *= ry;
  
  ychg = 1;
  ychg -= ry;
  ychg -= ry;
  ychg *= rx;
  ychg *= rx;
  
  err = 0;
  
  stopx = 0;

  stopy = rxrx2;
  stopy *= ry;
  

  while( stopx <= stopy )
  {
    u8g_draw_ellipse_section(u8g, x, y, x0, y0, option);
    x++;
    stopx += ryry2;
    err += xchg;
    xchg += ryry2;
    if ( 2*err+ychg > 0 )
    {
      y--;
      stopy -= rxrx2;
      err += ychg;
      ychg += rxrx2;
    }
  }
  
}

void u8g_DrawEllipse(u8g_t *u8g, u8g_uint_t x0, u8g_uint_t y0, u8g_uint_t rx, u8g_uint_t ry, uint8_t option)
{
  /* check for bounding box */
  {
    u8g_uint_t rxp, rxp2;
    u8g_uint_t ryp, ryp2;
    
    rxp = rx;
    rxp++;
    rxp2 = rxp;
    rxp2 *= 2;

    ryp = ry;
    ryp++;
    ryp2 = ryp;
    ryp2 *= 2;
    
    if ( u8g_IsBBXIntersection(u8g, x0-rxp, y0-ryp, rxp2, ryp2) == 0)
      return;    
  }
  
  u8g_draw_ellipse(u8g, x0, y0, rx, ry, option);
}

static void u8g_draw_filled_ellipse_section(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t x0, u8g_uint_t y0, uint8_t option) U8G_NOINLINE;
static void u8g_draw_filled_ellipse_section(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t x0, u8g_uint_t y0, uint8_t option)
{
    /* upper right */
    if ( option & U8G_DRAW_UPPER_RIGHT )
    {
      u8g_DrawVLine(u8g, x0+x, y0-y, y+1);
    }
    
    /* upper left */
    if ( option & U8G_DRAW_UPPER_LEFT )
    {
      u8g_DrawVLine(u8g, x0-x, y0-y, y+1);
    }
    
    /* lower right */
    if ( option & U8G_DRAW_LOWER_RIGHT )
    {
      u8g_DrawVLine(u8g, x0+x, y0, y+1);
    }
    
    /* lower left */
    if ( option & U8G_DRAW_LOWER_LEFT )
    {
      u8g_DrawVLine(u8g, x0-x, y0, y+1);
    }
}

void u8g_draw_filled_ellipse(u8g_t *u8g, u8g_uint_t x0, u8g_uint_t y0, u8g_uint_t rx, u8g_uint_t ry, uint8_t option)
{
  u8g_uint_t x, y;
  u8g_long_t xchg, ychg;
  u8g_long_t err;
  u8g_long_t rxrx2;
  u8g_long_t ryry2;
  u8g_long_t stopx, stopy;
  
  rxrx2 = rx;
  rxrx2 *= rx;
  rxrx2 *= 2;
  
  ryry2 = ry;
  ryry2 *= ry;
  ryry2 *= 2;
  
  x = rx;
  y = 0;
  
  xchg = 1;
  xchg -= rx;
  xchg -= rx;
  xchg *= ry;
  xchg *= ry;
  
  ychg = rx;
  ychg *= rx;
  
  err = 0;
  
  stopx = ryry2;
  stopx *= rx;
  stopy = 0;
  
  while( stopx >= stopy )
  {
    u8g_draw_filled_ellipse_section(u8g, x, y, x0, y0, option);
    y++;
    stopy += rxrx2;
    err += ychg;
    ychg += rxrx2;
    if ( 2*err+xchg > 0 )
    {
      x--;
      stopx -= ryry2;
      err += xchg;
      xchg += ryry2;      
    }
  }

  x = 0;
  y = ry;
  
  xchg = ry;
  xchg *= ry;
  
  ychg = 1;
  ychg -= ry;
  ychg -= ry;
  ychg *= rx;
  ychg *= rx;
  
  err = 0;
  
  stopx = 0;

  stopy = rxrx2;
  stopy *= ry;
  

  while( stopx <= stopy )
  {
    u8g_draw_filled_ellipse_section(u8g, x, y, x0, y0, option);
    x++;
    stopx += ryry2;
    err += xchg;
    xchg += ryry2;
    if ( 2*err+ychg > 0 )
    {
      y--;
      stopy -= rxrx2;
      err += ychg;
      ychg += rxrx2;
    }
  }
  
}

void u8g_DrawFilledEllipse(u8g_t *u8g, u8g_uint_t x0, u8g_uint_t y0, u8g_uint_t rx, u8g_uint_t ry, uint8_t option)
{
  /* check for bounding box */
  {
    u8g_uint_t rxp, rxp2;
    u8g_uint_t ryp, ryp2;
    
    rxp = rx;
    rxp++;
    rxp2 = rxp;
    rxp2 *= 2;

    ryp = ry;
    ryp++;
    ryp2 = ryp;
    ryp2 *= 2;
    
    if ( u8g_IsBBXIntersection(u8g, x0-rxp, y0-ryp, rxp2, ryp2) == 0)
      return;    
  }
  
  u8g_draw_filled_ellipse(u8g, x0, y0, rx, ry, option);
}
























/* u8g_rect.c */


void u8g_draw_hline(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t w)
{
  uint8_t pixel = 0x0ff;
  while( w >= 8 )
  {
    u8g_Draw8Pixel(u8g, x, y, 0, pixel);
    w-=8;
    x+=8;
  }
  if ( w != 0 )
  {
    w ^=7;
    w++;
    pixel <<= w&7;
    u8g_Draw8Pixel(u8g, x, y, 0, pixel);
  }
}

void u8g_draw_vline(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t h)
{
  uint8_t pixel = 0x0ff;
  while( h >= 8 )
  {
    u8g_Draw8Pixel(u8g, x, y, 1, pixel);
    h-=8;
    y+=8;
  }
  if ( h != 0 )
  {
    h ^=7;
    h++;
    pixel <<= h&7;
    u8g_Draw8Pixel(u8g, x, y, 1, pixel);
  }
}

void u8g_DrawHLine(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t w)
{
  if ( u8g_IsBBXIntersection(u8g, x, y, w, 1) == 0 )
    return;
  u8g_draw_hline(u8g, x, y, w);
}

void u8g_DrawVLine(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t w)
{
  if ( u8g_IsBBXIntersection(u8g, x, y, 1, w) == 0 )
    return;
  u8g_draw_vline(u8g, x, y, w);
}

/* restrictions: w > 0 && h > 0 */
void u8g_DrawFrame(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t w, u8g_uint_t h)
{
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

void u8g_draw_box(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t w, u8g_uint_t h)
{
  do
  { 
    u8g_draw_hline(u8g, x, y, w);
    y++;    
    h--;
  } while( h != 0 );
}

/* restrictions: h > 0 */
void u8g_DrawBox(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t w, u8g_uint_t h)
{
  if ( u8g_IsBBXIntersection(u8g, x, y, w, h) == 0 )
    return;
  u8g_draw_box(u8g, x, y, w, h);
}


void u8g_DrawRFrame(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t w, u8g_uint_t h, u8g_uint_t r)
{
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

void u8g_DrawRBox(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t w, u8g_uint_t h, u8g_uint_t r)
{
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





/* u8g_bitmap.c */

void u8g_DrawHBitmap(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t cnt, const uint8_t *bitmap)
{
  while( cnt > 0 )
  {
    u8g_Draw8Pixel(u8g, x, y, 0, *bitmap);
    bitmap++;
    cnt--;
    x+=8;
  }
}

void u8g_DrawBitmap(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t cnt, u8g_uint_t h, const uint8_t *bitmap)
{
  if ( u8g_IsBBXIntersection(u8g, x, y, cnt*8, h) == 0 )
    return;
  while( h > 0 )
  {
    u8g_DrawHBitmap(u8g, x, y, cnt, bitmap);
    bitmap += cnt;
    y++;
    h--;
  }
}


void u8g_DrawHBitmapP(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t cnt, const u8g_pgm_uint8_t *bitmap)
{
  while( cnt > 0 )
  {
    u8g_Draw8Pixel(u8g, x, y, 0, u8g_pgm_read(bitmap));
    bitmap++;
    cnt--;
    x+=8;
  }
}

void u8g_DrawBitmapP(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t cnt, u8g_uint_t h, const u8g_pgm_uint8_t *bitmap)
{
  if ( u8g_IsBBXIntersection(u8g, x, y, cnt*8, h) == 0 )
    return;
  while( h > 0 )
  {
    u8g_DrawHBitmapP(u8g, x, y, cnt, bitmap);
    bitmap += cnt;
    y++;
    h--;
  }
}

/*=========================================================================*/

static void u8g_DrawHXBM(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t w, const uint8_t *bitmap)
{
  uint8_t d;
  x+=7;
  while( w >= 8 )
  {
    u8g_Draw8Pixel(u8g, x, y, 2, *bitmap);
    bitmap++;
    w-= 8;
    x+=8;
  }
  if ( w > 0 )
  {
    d = *bitmap;
    x -= 7;
    do
    {
      if ( d & 1 )
        u8g_DrawPixel(u8g, x, y);
      x++;
      w--;
      d >>= 1;      
    } while ( w > 0 );
  }
}

void u8g_DrawXBM(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t w, u8g_uint_t h, const uint8_t *bitmap)
{
  u8g_uint_t b;
  b = w;
  b += 7;
  b >>= 3;
  
  if ( u8g_IsBBXIntersection(u8g, x, y, w, h) == 0 )
    return;
  
  while( h > 0 )
  {
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
    do
    {
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














/* u8g_line.c */
void u8g_DrawLine(u8g_t *u8g, u8g_uint_t x1, u8g_uint_t y1, u8g_uint_t x2, u8g_uint_t y2)
{
  u8g_uint_t tmp;
  u8g_uint_t x,y;
  u8g_uint_t dx, dy;
  u8g_int_t err;
  u8g_int_t ystep;

  uint8_t swapxy = 0;
  
  /* no BBX intersection check at the moment, should be added... */

  if ( x1 > x2 ) dx = x1-x2; else dx = x2-x1;
  if ( y1 > y2 ) dy = y1-y2; else dy = y2-y1;

  if ( dy > dx ) 
  {
    swapxy = 1;
    tmp = dx; dx =dy; dy = tmp;
    tmp = x1; x1 =y1; y1 = tmp;
    tmp = x2; x2 =y2; y2 = tmp;
  }
  if ( x1 > x2 ) 
  {
    tmp = x1; x1 =x2; x2 = tmp;
    tmp = y1; y1 =y2; y2 = tmp;
  }
  err = dx >> 1;
  if ( y2 > y1 ) ystep = 1; else ystep = -1;
  y = y1;

#ifndef  U8G_16BIT
  if ( x2 == 255 )
    x2--;
#else
  if ( x2 == 0xffff )
    x2--;
#endif

  for( x = x1; x <= x2; x++ )
  {
    if ( swapxy == 0 ) 
      u8g_DrawPixel(u8g, x, y); 
    else 
      u8g_DrawPixel(u8g, y, x); 
    err -= (uint8_t)dy;
    if ( err < 0 ) 
    {
      y += (u8g_uint_t)ystep;
      err += (u8g_uint_t)dx;
    }
  }
}



/*=========================================================================*/
/* u8g_circle.c */

static void u8g_draw_circle_section(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t x0, u8g_uint_t y0, uint8_t option) U8G_NOINLINE;

static void u8g_draw_circle_section(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t x0, u8g_uint_t y0, uint8_t option) {
    /* upper right */
    if ( option & U8G_DRAW_UPPER_RIGHT ) {
      u8g_DrawPixel(u8g, x0 + x, y0 - y);
      u8g_DrawPixel(u8g, x0 + y, y0 - x);
    }
    
    /* upper left */
    if ( option & U8G_DRAW_UPPER_LEFT )  {
      u8g_DrawPixel(u8g, x0 - x, y0 - y);
      u8g_DrawPixel(u8g, x0 - y, y0 - x);
    }
    
    /* lower right */
    if ( option & U8G_DRAW_LOWER_RIGHT )  {
      u8g_DrawPixel(u8g, x0 + x, y0 + y);
      u8g_DrawPixel(u8g, x0 + y, y0 + x);
    }
    
    /* lower left */
    if ( option & U8G_DRAW_LOWER_LEFT )  {
      u8g_DrawPixel(u8g, x0 - x, y0 + y);
      u8g_DrawPixel(u8g, x0 - y, y0 + x);
    }
}

void u8g_draw_circle(u8g_t *u8g, u8g_uint_t x0, u8g_uint_t y0, u8g_uint_t rad, uint8_t option)
{
    u8g_int_t f;
    u8g_int_t ddF_x;
    u8g_int_t ddF_y;
    u8g_uint_t x;
    u8g_uint_t y;

    f = 1;
    f -= rad;
    ddF_x = 1;
    ddF_y = 0;
    ddF_y -= rad;
    ddF_y *= 2;
    x = 0;
    y = rad;

    u8g_draw_circle_section(u8g, x, y, x0, y0, option);
    
    while ( x < y )
    {
      if (f >= 0) 
      {
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

void u8g_DrawCircle(u8g_t *u8g, u8g_uint_t x0, u8g_uint_t y0, u8g_uint_t rad, uint8_t option)
{
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

static void u8g_draw_disc_section(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, u8g_uint_t x0, u8g_uint_t y0, uint8_t option)
{
    /* upper right */
    if ( option & U8G_DRAW_UPPER_RIGHT )
    {
      u8g_DrawVLine(u8g, x0+x, y0-y, y+1);
      u8g_DrawVLine(u8g, x0+y, y0-x, x+1);
    }
    
    /* upper left */
    if ( option & U8G_DRAW_UPPER_LEFT )
    {
      u8g_DrawVLine(u8g, x0-x, y0-y, y+1);
      u8g_DrawVLine(u8g, x0-y, y0-x, x+1);
    }
    
    /* lower right */
    if ( option & U8G_DRAW_LOWER_RIGHT )
    {
      u8g_DrawVLine(u8g, x0+x, y0, y+1);
      u8g_DrawVLine(u8g, x0+y, y0, x+1);
    }
    
    /* lower left */
    if ( option & U8G_DRAW_LOWER_LEFT )
    {
      u8g_DrawVLine(u8g, x0-x, y0, y+1);
      u8g_DrawVLine(u8g, x0-y, y0, x+1);
    }
}

void u8g_draw_disc(u8g_t *u8g, u8g_uint_t x0, u8g_uint_t y0, u8g_uint_t rad, uint8_t option)
{
  u8g_int_t f;
  u8g_int_t ddF_x;
  u8g_int_t ddF_y;
  u8g_uint_t x;
  u8g_uint_t y;

  f = 1;
  f -= rad;
  ddF_x = 1;
  ddF_y = 0;
  ddF_y -= rad;
  ddF_y *= 2;
  x = 0;
  y = rad;

  u8g_draw_disc_section(u8g, x, y, x0, y0, option);
  
  while ( x < y )
  {
    if (f >= 0) 
    {
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

void u8g_DrawDisc(u8g_t *u8g, u8g_uint_t x0, u8g_uint_t y0, u8g_uint_t rad, uint8_t option)
{
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


