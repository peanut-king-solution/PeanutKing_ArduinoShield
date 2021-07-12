/*
  oled_font.c
  Font High Level Interface
  Universal 8bit Graphics Library
  Copyright (c)
  All rights reserved.
*/

#include "oled.h"

/* font api */

/* pointer to the start adress of the glyph, points to progmem area */
typedef void * oled_glyph_t;

/* size of the font data structure, there is no struct or class... */
#define OLED_FONT_DATA_STRUCT_SIZE 17

/*
  ... instead the fields of the font data structure are accessed directly by offset 
  font information 
  offset
  0             font format
  1             FONTBOUNDINGBOX width           unsigned
  2             FONTBOUNDINGBOX height          unsigned
  3             FONTBOUNDINGBOX x-offset        signed
  4             FONTBOUNDINGBOX y-offset        signed
  5             capital A height                unsigned
  6             start 'A'
  8             start 'a'
  10            encoding start
  11            encoding end
  12            descent 'g'                     negative: below baseline
  13            font max ascent
  14            font min decent                 negative: below baseline 
  15            font xascent
  16            font xdecent                    negative: below baseline 
  
*/

/* use case: What is the width and the height of the minimal box into which string s fints? */
void oled_font_GetStrSize(const void *font, const char *s, uint8_t *width, uint8_t *height);
void oled_font_GetStrSizeP(const void *font, const char *s, uint8_t *width, uint8_t *height);

/* use case: lower left edge of a minimal box is known, what is the correct x, y position for the string draw procedure */
void oled_font_AdjustXYToDraw(const void *font, const char *s, uint8_t *x, uint8_t *y);
void oled_font_AdjustXYToDrawP(const void *font, const char *s, uint8_t *x, uint8_t *y);

/* use case: Baseline origin known, return minimal box */
void oled_font_GetStrMinBox(oled_t *oled, const void *font, const char *s, uint8_t *x, uint8_t *y, uint8_t *width, uint8_t *height);

/* procedures */

/*========================================================================*/
/* low level byte and word access */

/* removed NOINLINE, because it leads to smaller code, might also be faster */
//static uint8_t oled_font_get_byte(const oled_fntpgm_uint8_t *font, uint8_t offset) OLED_NOINLINE;
static uint8_t oled_font_get_byte(const oled_fntpgm_uint8_t *font, uint8_t offset) {
  font += offset;
  return oled_pgm_read( (oled_pgm_uint8_t *)font );  
}

static uint16_t oled_font_get_word(const oled_fntpgm_uint8_t *font, uint8_t offset) OLED_NOINLINE; 
static uint16_t oled_font_get_word(const oled_fntpgm_uint8_t *font, uint8_t offset) {
    uint16_t pos;
    font += offset;
    pos = oled_pgm_read( (oled_pgm_uint8_t *)font );
    font++;
    pos <<= 8;
    pos += oled_pgm_read( (oled_pgm_uint8_t *)font);
    return pos;
}

/*========================================================================*/
/* direct access on the font */

static uint8_t oled_font_GetFormat(const oled_fntpgm_uint8_t *font) OLED_NOINLINE;
static uint8_t oled_font_GetFormat(const oled_fntpgm_uint8_t *font) {
  return oled_font_get_byte(font, 0);
}

static uint8_t oled_font_GetFontGlyphStructureSize(const oled_fntpgm_uint8_t *font) OLED_NOINLINE;
static uint8_t oled_font_GetFontGlyphStructureSize(const oled_fntpgm_uint8_t *font) {
  switch(oled_font_GetFormat(font)) {
    case 0: return 6;
    case 1: return 3;
    case 2: return 6;
  }
  return 3;
}

static uint8_t oled_font_GetBBXWidth(const void *font) {
  return oled_font_get_byte(font, 1);
}

static uint8_t oled_font_GetBBXHeight(const void *font) {
  return oled_font_get_byte(font, 2);
}

static int8_t oled_font_GetBBXOffX(const void *font) {
  return oled_font_get_byte(font, 3);
}

static int8_t oled_font_GetBBXOffY(const void *font) {
  return oled_font_get_byte(font, 4);
}

uint8_t oled_font_GetCapitalAHeight(const void *font) {
  return oled_font_get_byte(font, 5);
}

uint16_t oled_font_GetEncoding65Pos(const void *font) OLED_NOINLINE;
uint16_t oled_font_GetEncoding65Pos(const void *font) {
    return oled_font_get_word(font, 6);
}

uint16_t oled_font_GetEncoding97Pos(const void *font) OLED_NOINLINE;
uint16_t oled_font_GetEncoding97Pos(const void *font) {
    return oled_font_get_word(font, 8);
}

uint8_t oled_font_GetFontStartEncoding(const void *font) {
  return oled_font_get_byte(font, 10);
}

uint8_t oled_font_GetFontEndEncoding(const void *font) {
  return oled_font_get_byte(font, 11);
}

int8_t oled_font_GetLowerGDescent(const void *font) {
  return oled_font_get_byte(font, 12);
}

int8_t oled_font_GetFontAscent(const void *font) {
  return oled_font_get_byte(font, 13);
}

int8_t oled_font_GetFontDescent(const void *font) {
  return oled_font_get_byte(font, 14);
}

int8_t oled_font_GetFontXAscent(const void *font) {
  return oled_font_get_byte(font, 15);
}

int8_t oled_font_GetFontXDescent(const void *font) {
  return oled_font_get_byte(font, 16);
}


/* return the data start for a font and the glyph pointer */
static uint8_t *oled_font_GetGlyphDataStart(const void *font, oled_glyph_t g) {
  return ((oled_fntpgm_uint8_t *)g) + oled_font_GetFontGlyphStructureSize(font);
}

/* calculate the overall length of the font, only used to create the picture for the google wiki */
size_t oled_font_GetSize(const void *font) {
  uint8_t *p = (uint8_t *)(font);
  uint8_t font_format = oled_font_GetFormat(font);
  uint8_t data_structure_size = oled_font_GetFontGlyphStructureSize(font);
  uint8_t start, end;
  uint8_t i;
  uint8_t mask = 255;
  
  start = oled_font_GetFontStartEncoding(font);
  end = oled_font_GetFontEndEncoding(font);

  if ( font_format == 1 )
    mask = 15;

  p += OLED_FONT_DATA_STRUCT_SIZE;       /* skip font general information */  

  i = start;  
  for(;;) {
    if ( oled_pgm_read((oled_pgm_uint8_t *)(p)) == 255 )
    {
      p += 1;
    }
    else
    {
      p += oled_pgm_read( ((oled_pgm_uint8_t *)(p)) + 2 ) & mask;
      p += data_structure_size;
    }
    if ( i == end )
      break;
    i++;
  }
    
  return p - (uint8_t *)font;
}

/*========================================================================*/
/* oled interface, font access */

uint8_t oled_GetFontBBXWidth(oled_t *oled) {
  return oled_font_GetBBXWidth(oled->font);
}

uint8_t oled_GetFontBBXHeight(oled_t *oled) {
  return oled_font_GetBBXHeight(oled->font);
}

int8_t oled_GetFontBBXOffX(oled_t *oled) OLED_NOINLINE;
int8_t oled_GetFontBBXOffX(oled_t *oled) {
  return oled_font_GetBBXOffX(oled->font);
}

int8_t oled_GetFontBBXOffY(oled_t *oled) OLED_NOINLINE;
int8_t oled_GetFontBBXOffY(oled_t *oled) {
  return oled_font_GetBBXOffY(oled->font);
}

uint8_t oled_GetFontCapitalAHeight(oled_t *oled) OLED_NOINLINE; 
uint8_t oled_GetFontCapitalAHeight(oled_t *oled) {
  return oled_font_GetCapitalAHeight(oled->font);
}

/*========================================================================*/
/* glyph handling */

static void oled_CopyGlyphDataToCache(oled_t *oled, oled_glyph_t g) {
  uint8_t tmp;
  switch( oled_font_GetFormat(oled->font) ) {
    case 0:
    case 2:
  /*
    format 0
    glyph information 
    offset
    0             BBX width                                       unsigned
    1             BBX height                                      unsigned
    2             data size                                          unsigned    (BBX width + 7)/8 * BBX height
    3             DWIDTH                                          signed
    4             BBX xoffset                                    signed
    5             BBX yoffset                                    signed
  byte 0 == 255 indicates empty glyph
  */
      oled->glyph_width =  oled_pgm_read( ((oled_pgm_uint8_t *)g) + 0 );
      oled->glyph_height =  oled_pgm_read( ((oled_pgm_uint8_t *)g) + 1 );
      oled->glyph_dx =  oled_pgm_read( ((oled_pgm_uint8_t *)g) + 3 );
      oled->glyph_x =  oled_pgm_read( ((oled_pgm_uint8_t *)g) + 4 );
      oled->glyph_y =  oled_pgm_read( ((oled_pgm_uint8_t *)g) + 5 );
      break;
    case 1:
    default:
      /*
format 1
  0             BBX xoffset                                    signed   --> upper 4 Bit
  0             BBX yoffset                                    signed --> lower 4 Bit
  1             BBX width                                       unsigned --> upper 4 Bit
  1             BBX height                                      unsigned --> lower 4 Bit
  2             data size                                           unsigned -(BBX width + 7)/8 * BBX height  --> lower 4 Bit
  2             DWIDTH                                          signed --> upper  4 Bit
  byte 0 == 255 indicates empty glyph
      */
    
      tmp = oled_pgm_read( ((oled_pgm_uint8_t *)g) + 0 );
      oled->glyph_y =  tmp & 15;
      oled->glyph_y-=2;
      tmp >>= 4;
      oled->glyph_x =  tmp;
    
      tmp = oled_pgm_read( ((oled_pgm_uint8_t *)g) + 1 );
      oled->glyph_height =  tmp & 15;
      tmp >>= 4;
      oled->glyph_width =  tmp;
      
      tmp = oled_pgm_read( ((oled_pgm_uint8_t *)g) + 2 );
      tmp >>= 4;
      oled->glyph_dx = tmp;
    
      
      break;
  }
}

//void oled_FillEmptyGlyphCache(oled_t *oled) OLED_NOINLINE;
static void oled_FillEmptyGlyphCache(oled_t *oled) {
  oled->glyph_dx = 0;
  oled->glyph_width = 0;
  oled->glyph_height = 0;
  oled->glyph_x = 0;
  oled->glyph_y = 0;
}

/*
  Find (with some speed optimization) and return a pointer to the glyph data structure
  Also uncompress (format 1) and copy the content of the data structure to the oled structure
*/
oled_glyph_t oled_GetGlyph(oled_t *oled, uint8_t requested_encoding) {
  uint8_t *p = (uint8_t *)(oled->font);
  uint8_t font_format = oled_font_GetFormat(oled->font);
  uint8_t data_structure_size = oled_font_GetFontGlyphStructureSize(oled->font);
  uint8_t start, end;
  uint16_t pos;
  uint8_t i;
  uint8_t mask = 255;

  if ( font_format == 1 )
    mask = 15;
  
  start = oled_font_GetFontStartEncoding(oled->font);
  end = oled_font_GetFontEndEncoding(oled->font);

  pos = oled_font_GetEncoding97Pos(oled->font);
  if ( requested_encoding >= 97 && pos > 0 ) {
    p+= pos;
    start = 97;
  }
  else  {
    pos = oled_font_GetEncoding65Pos(oled->font);
    if ( requested_encoding >= 65 && pos > 0 )
    {
      p+= pos;
      start = 65;
    }
    else
      p += OLED_FONT_DATA_STRUCT_SIZE;       /* skip font general information */  
  }
  
  if ( requested_encoding > end ) {
    oled_FillEmptyGlyphCache(oled);
    return NULL;                      /* not found */
  }
  
  i = start;
  if ( i <= end ) {
    for(;;)
    {
      if ( oled_pgm_read((oled_pgm_uint8_t *)(p)) == 255 )
      {
        p += 1;
      }
      else
      {
        if ( i == requested_encoding )
        {
          oled_CopyGlyphDataToCache(oled, p);
          return p;
        }
        p += oled_pgm_read( ((oled_pgm_uint8_t *)(p)) + 2 ) & mask;
        p += data_structure_size;
      }
      if ( i == end )
        break;
      i++;
    }
  }
  
  oled_FillEmptyGlyphCache(oled);
    
  return NULL;
}

uint8_t oled_IsGlyph(oled_t *oled, uint8_t requested_encoding) {
  if ( oled_GetGlyph(oled, requested_encoding) != NULL )
    return 1;
  return 0;
}

int8_t oled_GetGlyphDeltaX(oled_t *oled, uint8_t requested_encoding) {
  if ( oled_GetGlyph(oled, requested_encoding) == NULL )
    return 0;  /* should never happen, so return something */
  return oled->glyph_dx;
}


/*========================================================================*/
/* glyph drawing procedures */

#ifdef OBSOLETE
/*
  Draw a glyph
  x,y: left baseline position of the glyph
*/
int8_t oled_DrawGlyphDir(oled_t *oled, uint8_t x, uint8_t y, uint8_t dir, uint8_t encoding) {
  oled_glyph_t g;
  uint8_t w, h, i, j;
  const oled_pgm_uint8_t *data;
  uint8_t bytes_per_line;
  uint8_t ix, iy;

  g = oled_GetGlyph(oled, encoding);
  if ( g == NULL  )
    return 0;
  w = oled->glyph_width;
  h = oled->glyph_height;

  bytes_per_line = w;
  bytes_per_line += 7;
  bytes_per_line /= 8;
  
  data = oled_font_GetGlyphDataStart(oled->font, g);

  switch(dir) {
    case 0:
      x += oled->glyph_x;
      y -= oled->glyph_y;
      y--;
      //oled_DrawFrame(oled, x, y-h+1, w, h);
      if ( oled_IsBBXIntersection(oled, x, y-h+1, w, h) == 0 )
        return oled->glyph_dx;
      
      iy = y;
      iy -= h;
      iy++;
      
      for( j = 0; j < h; j++ )
      {
        ix = x;
        for( i = 0; i < bytes_per_line; i++ )
        {
          oled_Draw8Pixel(oled, ix, iy, dir, oled_pgm_read(data));
          data++;
          ix+=8;
        }
        iy++;
      }
      break;
    case 1:
      x += oled->glyph_y;
      x++;
      y += oled->glyph_x;
      //printf("enc %d, dir %d, x %d, y %d, w %d, h %d\n", encoding, dir, x, y, w, h);
      //oled_DrawFrame(oled, x, y, h, w);
      if ( oled_IsBBXIntersection(oled, x, y, h, w) == 0 )
        return oled->glyph_dx;
      
      ix = x;
      ix += h;
      ix--;
      for( j = 0; j < h; j++ )
      {
        iy = y;
        for( i = 0; i < bytes_per_line; i++ )
        {
          oled_Draw8Pixel(oled, ix, iy, dir, oled_pgm_read(data));
          data++;
          iy+=8;
        }
        ix--;
      }
      break;
    case 2:
      x -= oled->glyph_x;
      y += oled->glyph_y;
      y++;
      if ( oled_IsBBXIntersection(oled, x-w-1, y, w, h) == 0 )
        return oled->glyph_dx;
      
      iy = y;
      iy += h;
      iy--;
      for( j = 0; j < h; j++ )
      {
        ix = x;
        for( i = 0; i < bytes_per_line; i++ )
        {
          oled_Draw8Pixel(oled, ix, iy, dir, oled_pgm_read(data));
          data++;
          ix-=8;
        }
        iy--;
      }
      break;
    case 3:
      x -= oled->glyph_y;
      x--;
      y -= oled->glyph_x;
      
      if ( oled_IsBBXIntersection(oled, x-h-1, y-w-1, h, w) == 0 )
        return oled->glyph_dx;
      
      ix = x;
      ix -= h;
      ix++;
      
      for( j = 0; j < h; j++ )
      {
        iy = y;
        for( i = 0; i < bytes_per_line; i++ )
        {
          oled_Draw8Pixel(oled, ix, iy, dir, oled_pgm_read(data));
          data++;
          iy-=8;
        }
        ix++;
      }
      break;    
  }
  return oled->glyph_dx;
}
#endif

int8_t oled_draw_glyph(oled_t *oled, uint8_t x, uint8_t y, uint8_t encoding) {
  const oled_pgm_uint8_t *data;
  uint8_t w, h;
  uint8_t i, j;
  uint8_t ix, iy;
  {
    oled_glyph_t g = oled_GetGlyph(oled, encoding);
    if ( g == NULL  )
      return 0;
    data = oled_font_GetGlyphDataStart(oled->font, g);
  }
  
  w = oled->glyph_width;
  h = oled->glyph_height;
  
  x += oled->glyph_x;
  y -= oled->glyph_y;
  y--;
  
  if ( oled_IsBBXIntersection(oled, x, y-h+1, w, h) == 0 )
    return oled->glyph_dx;

  /* now, w is reused as bytes per line */
  w += 7;
  w /= 8;
  
  iy = y;
  iy -= h;
  iy++;

  for( j = 0; j < h; j++ ) {
    ix = x;
    for( i = 0; i < w; i++ )
    {
      oled_Draw8Pixel(oled, ix, iy, 0, oled_pgm_read(data));
      data++;
      ix+=8;
    }
    iy++;
  }
  return oled->glyph_dx;
}

int8_t oled_DrawGlyph(oled_t *oled, uint8_t x, uint8_t y, uint8_t encoding) {
  y += oled->font_calc_vref(oled);
  return oled_draw_glyph(oled, x, y, encoding);
}

int8_t oled_draw_glyph90(oled_t *oled, uint8_t x, uint8_t y, uint8_t encoding) {
  const oled_pgm_uint8_t *data;
  uint8_t w, h;
  uint8_t i, j;
  uint8_t ix, iy;
 {
    oled_glyph_t g = oled_GetGlyph(oled, encoding);
    if ( g == NULL  )
      return 0;
    data = oled_font_GetGlyphDataStart(oled->font, g);
  }
  
  w = oled->glyph_width;
  h = oled->glyph_height;
  
  x += oled->glyph_y;
  x++;
  y += oled->glyph_x;
  
  if ( oled_IsBBXIntersection(oled, x, y, h, w) == 0 )
    return oled->glyph_dx;

  /* now, w is reused as bytes per line */
  w += 7;
  w /= 8;
  
  ix = x;
  ix += h;
  ix--;
  for( j = 0; j < h; j++ ) {
    iy = y;
    for( i = 0; i < w; i++ )
    {
      oled_Draw8Pixel(oled, ix, iy, 1, oled_pgm_read(data));
      data++;
      iy+=8;
    }
    ix--;
  }
  return oled->glyph_dx;
}

int8_t oled_DrawGlyph90(oled_t *oled, uint8_t x, uint8_t y, uint8_t encoding) {
  x -= oled->font_calc_vref(oled);
  return oled_draw_glyph90(oled, x, y, encoding);
}


int8_t oled_draw_glyph180(oled_t *oled, uint8_t x, uint8_t y, uint8_t encoding) {
  const oled_pgm_uint8_t *data;
  uint8_t w, h;
  uint8_t i, j;
  uint8_t ix, iy;
 {
    oled_glyph_t g = oled_GetGlyph(oled, encoding);
    if ( g == NULL  )
      return 0;
    data = oled_font_GetGlyphDataStart(oled->font, g);
  }
  
  w = oled->glyph_width;
  h = oled->glyph_height;
  
  x -= oled->glyph_x;
  y += oled->glyph_y;
  y++;
  
  if ( oled_IsBBXIntersection(oled, x-(w-1), y, w, h) == 0 )
    return oled->glyph_dx;

  /* now, w is reused as bytes per line */
  w += 7;
  w /= 8;
  
  iy = y;
  iy += h;
  iy--;
  for( j = 0; j < h; j++ ) {
    ix = x;
    for( i = 0; i < w; i++ ) {
      oled_Draw8Pixel(oled, ix, iy, 2, oled_pgm_read(data));
      data++;
      ix-=8;
    }
    iy--;
  }
  return oled->glyph_dx;
}

int8_t oled_DrawGlyph180(oled_t *oled, uint8_t x, uint8_t y, uint8_t encoding) {
  y -= oled->font_calc_vref(oled);
  return oled_draw_glyph180(oled, x, y, encoding);
}


int8_t oled_draw_glyph270(oled_t *oled, uint8_t x, uint8_t y, uint8_t encoding) {
  const oled_pgm_uint8_t *data;
  uint8_t w, h;
  uint8_t i, j;
  uint8_t ix, iy;
 {
    oled_glyph_t g = oled_GetGlyph(oled, encoding);
    if ( g == NULL  )
      return 0;
    data = oled_font_GetGlyphDataStart(oled->font, g);
  }
  
  w = oled->glyph_width;
  h = oled->glyph_height;
  
  x -= oled->glyph_y;
  x--;
  y -= oled->glyph_x;
  
  if ( oled_IsBBXIntersection(oled, x-(h-1), y-(w-1), h, w) == 0 )
    return oled->glyph_dx;
  

  /* now, w is reused as bytes per line */
  w += 7;
  w /= 8;  
      
  ix = x;
  ix -= h;
  ix++;
  
  for( j = 0; j < h; j++ ) {
    iy = y;
    for( i = 0; i < w; i++ ) {
      oled_Draw8Pixel(oled, ix, iy, 3, oled_pgm_read(data));
      data++;
      iy-=8;
    }
    ix++;
  }
  return oled->glyph_dx;
}

int8_t oled_DrawGlyph270(oled_t *oled, uint8_t x, uint8_t y, uint8_t encoding) {
  x += oled->font_calc_vref(oled);
  return oled_draw_glyph270(oled, x, y, encoding);
}



#ifdef OBSOLETE
/*
  Draw a glyph
  x,y: lower left corner of the font bounding box
*/
int8_t oled_DrawGlyphFontBBX(oled_t *oled, uint8_t x, uint8_t y, uint8_t dir, uint8_t encoding) {
  /* TODO: apply "dir" */
  x -= oled_GetFontBBXOffX(oled);
  y += oled_GetFontBBXOffY(oled);
  return oled_DrawGlyphDir(oled, x, y, dir, encoding);
}
#endif

/*========================================================================*/
/* string drawing procedures */


uint8_t oled_DrawStr(oled_t *oled, uint8_t x, uint8_t y, const char *s) {
  uint8_t t = 0;
  int8_t d;
  
  //uint8_t oled_GetStrWidth(oled, s);
  //oled_font_GetFontAscent(oled->font)-oled_font_GetFontDescent(oled->font);
  
  y += oled->font_calc_vref(oled);
  
  while( *s != '\0' ) {
    d = oled_draw_glyph(oled, x, y, *s);
    x += d;
    t += d;
    s++;
  }
  return t;
}

uint8_t oled_DrawStr90(oled_t *oled, uint8_t x, uint8_t y, const char *s) {
  uint8_t t = 0;
  int8_t d;
    
  x -= oled->font_calc_vref(oled);

  while( *s != '\0' ) {
    d = oled_draw_glyph90(oled, x, y, *s);
    y += d;
    t += d;
    s++;
  }
  return t;
}

uint8_t oled_DrawStr180(oled_t *oled, uint8_t x, uint8_t y, const char *s) {
  uint8_t t = 0;
  int8_t d;

  y -= oled->font_calc_vref(oled);
  
  while( *s != '\0' ) {
    d = oled_draw_glyph180(oled, x, y, *s);
    x -= d;
    t += d;
    s++;
  }
  return t;
}

uint8_t oled_DrawStr270(oled_t *oled, uint8_t x, uint8_t y, const char *s) {
  uint8_t t = 0;
  int8_t d;

  x += oled->font_calc_vref(oled);

  while( *s != '\0' ) {
    d = oled_draw_glyph270(oled, x, y, *s);
    y -= d;
    t += d;
    s++;
  }
  return t;
}

uint8_t oled_DrawStrDir(oled_t *oled, uint8_t x, uint8_t y, uint8_t dir, const char *s) {
  switch(dir) {
    case 0:
      return oled_DrawStr(oled, x, y, s);
    case 1:
      return oled_DrawStr90(oled, x, y, s);
    case 2:
      return oled_DrawStr180(oled, x, y, s);
    case 3:
      return oled_DrawStr270(oled, x, y, s);
  }
  return 0;
}

uint8_t oled_DrawStrP(oled_t *oled, uint8_t x, uint8_t y, const oled_pgm_uint8_t *s) {
  uint8_t t = 0;
  int8_t d;
  uint8_t c;
  
  y += oled->font_calc_vref(oled);
  
  for(;;) {
    c = oled_pgm_read(s);
    if ( c == '\0' )
      break;
    d = oled_draw_glyph(oled, x, y, c);
    x += d;
    t += d;
    s++;
  }
  return t;
}

uint8_t oled_DrawStr90P(oled_t *oled, uint8_t x, uint8_t y, const oled_pgm_uint8_t *s) {
  uint8_t t = 0;
  int8_t d;
  uint8_t c;
  
  x -= oled->font_calc_vref(oled);

  for(;;) {
    c = oled_pgm_read(s);
    if ( c == '\0' )
      break;
    d = oled_DrawGlyph90(oled, x, y, c);
    y += d;
    t += d;
    s++;
  }
  return t;
}

uint8_t oled_DrawStr180P(oled_t *oled, uint8_t x, uint8_t y, const oled_pgm_uint8_t *s) {
  uint8_t t = 0;
  int8_t d;
  uint8_t c;

  y -= oled->font_calc_vref(oled);
  
  for(;;) {
    c = oled_pgm_read(s);
    if ( c == '\0' )
      break;
    d = oled_DrawGlyph180(oled, x, y, c);
    x -= d;
    t += d;
    s++;
  }
  return t;
}

uint8_t oled_DrawStr270P(oled_t *oled, uint8_t x, uint8_t y, const oled_pgm_uint8_t *s) {
  uint8_t t = 0;
  int8_t d;
  uint8_t c;

  x += oled->font_calc_vref(oled);

  for(;;) {
    c = oled_pgm_read(s);
    if ( c == '\0' )
      break;
    d = oled_DrawGlyph270(oled, x, y, c);
    y -= d;
    t += d;
    s++;
  }
  return t;
}

uint8_t oled_DrawStrFontBBX(oled_t *oled, uint8_t x, uint8_t y, uint8_t dir, const char *s) {
  x -= oled_GetFontBBXOffX(oled);
  y += oled_GetFontBBXOffY(oled);
  return oled_DrawStrDir(oled, x, y, dir, s);
}

/* still used by picgen.c, dir argument is ignored */
int8_t oled_DrawGlyphFontBBX(oled_t *oled, uint8_t x, uint8_t y, uint8_t dir, uint8_t encoding) {
  x -= oled_GetFontBBXOffX(oled);
  y += oled_GetFontBBXOffY(oled);
  oled_draw_glyph(oled, x, y, encoding);
  return 0;
}


/*========================================================================*/
/* set ascent/descent for reference point calculation */

void oled_UpdateRefHeight(oled_t *oled) {
  uint16_t ls;
  if ( oled->font == NULL )
    return;
  if ( oled->font_height_mode == OLED_FONT_HEIGHT_MODE_TEXT ) {
    oled->font_ref_ascent = oled_font_GetCapitalAHeight(oled->font);
    oled->font_ref_descent = oled_font_GetLowerGDescent(oled->font);
  }
  else if ( oled->font_height_mode == OLED_FONT_HEIGHT_MODE_XTEXT ) {
    oled->font_ref_ascent = oled_font_GetFontXAscent(oled->font);
    oled->font_ref_descent = oled_font_GetFontXDescent(oled->font);
  }
  else
  {
    oled->font_ref_ascent = oled_font_GetFontAscent(oled->font);
    oled->font_ref_descent = oled_font_GetFontDescent(oled->font);
  }
  
  ls = oled->font_ref_ascent - oled->font_ref_descent;
  if ( oled->font_line_spacing_factor != 64 ) {
    ls &= 255;
    ls *= oled->font_line_spacing_factor;
    ls >>= 6;
  }
  oled->line_spacing = ls;
}

void oled_SetFontRefHeightText(oled_t *oled) {
  oled->font_height_mode = OLED_FONT_HEIGHT_MODE_TEXT;
  oled_UpdateRefHeight(oled);
}

void oled_SetFontRefHeightExtendedText(oled_t *oled) {
  oled->font_height_mode = OLED_FONT_HEIGHT_MODE_XTEXT;
  oled_UpdateRefHeight(oled);
}


void oled_SetFontRefHeightAll(oled_t *oled) {
  oled->font_height_mode = OLED_FONT_HEIGHT_MODE_ALL;
  oled_UpdateRefHeight(oled);
}

/* factor = 64: linespaceing == ascent and descent */
void oled_SetFontLineSpacingFactor(oled_t *oled, uint8_t  factor) {
  oled->font_line_spacing_factor = factor;
  oled_UpdateRefHeight(oled);
}



/*========================================================================*/
/* callback procedures to correct the y position */

uint8_t oled_font_calc_vref_font(oled_t *oled) {
  return 0;
}

void oled_SetFontPosBaseline(oled_t *oled) {
  oled->font_calc_vref = oled_font_calc_vref_font;
}


uint8_t oled_font_calc_vref_bottom(oled_t *oled) {
  /* y += (uint8_t)(int8_t)(oled->font_ref_descent); */
  return (uint8_t)(int8_t)(oled->font_ref_descent);
}

void oled_SetFontPosBottom(oled_t *oled) {
  oled->font_calc_vref = oled_font_calc_vref_bottom;
}

uint8_t oled_font_calc_vref_top(oled_t *oled) {
  uint8_t tmp;
  /* reference pos is one pixel above the upper edge of the reference glyph */

  /*
  y += (uint8_t)(int8_t)(oled->font_ref_ascent);
  y++;
  */
  tmp = (uint8_t)(int8_t)(oled->font_ref_ascent);
  tmp++;
  return tmp;
}

void oled_SetFontPosTop(oled_t *oled) {
  oled->font_calc_vref = oled_font_calc_vref_top;
}

uint8_t oled_font_calc_vref_center(oled_t *oled) {
  int8_t tmp;
  tmp = oled->font_ref_ascent;
  tmp -= oled->font_ref_descent;
  tmp /= 2;
  tmp += oled->font_ref_descent;  
  /* y += (uint8_t)(int8_t)(tmp); */
  return tmp;
}

void oled_SetFontPosCenter(oled_t *oled) {
  oled->font_calc_vref = oled_font_calc_vref_center;
}

/*========================================================================*/
/* string pixel width calculation */

char oled_font_get_char(const void *s) {
  return *(const char *)(s);
}

char oled_font_get_charP(const void *s) {
  return oled_pgm_read(s);
}

typedef char (*oled_font_get_char_fn)(const void *s);


uint8_t oled_font_calc_str_pixel_width(oled_t *oled, const char *s, oled_font_get_char_fn get_char ) {
  uint8_t  w;
  uint8_t enc;
  
  /* reset the total minimal width to zero, this will be expanded during calculation */
  w = 0;
    
  enc = get_char(s);
  
  /* check for empty string, width is already 0 */
  if ( enc == '\0' ) {
    return w;
  }
  
  /* get the glyph information of the first char. This must be valid, because we already checked for the empty string */
  /* if *s is not inside the font, then the cached parameters of the glyph are all zero */
  oled_GetGlyph(oled, enc);

  /* strlen(s) == 1:       width = width(s[0]) */
  /* strlen(s) == 2:       width = - offx(s[0]) + deltax(s[0]) + offx(s[1]) + width(s[1]) */
  /* strlen(s) == 3:       width = - offx(s[0]) + deltax(s[0]) + deltax(s[1]) + offx(s[2]) + width(s[2]) */
  
  /* assume that the string has size 2 or more, than start with negative offset-x */
  /* for string with size 1, this will be nullified after the loop */
  w = -oled->glyph_x;  
  for(;;) {
    
    /* check and stop if the end of the string is reached */
    s++;
    if ( get_char(s) == '\0' )
      break;
    
    /* if there are still more characters, add the delta to the next glyph */
    w += oled->glyph_dx;
    
    /* store the encoding in a local variable, used also after the for(;;) loop */
    enc = get_char(s);
    
    /* load the next glyph information */
    oled_GetGlyph(oled, enc);
  }
  
  /* finally calculate the width of the last char */
  /* here is another exception, if the last char is a black, use the dx value instead */
  if ( enc != ' ' ) {
    /* if g was not updated in the for loop (strlen() == 1), then the initial offset x gets removed */
    w += oled->glyph_width;
    w += oled->glyph_x;
  }
  else
  {
    w += oled->glyph_dx;
  }
  
  
  return w;
}

uint8_t oled_GetStrPixelWidth(oled_t *oled, const char *s) {
  return oled_font_calc_str_pixel_width(oled, s, oled_font_get_char);
}

uint8_t oled_GetStrPixelWidthP(oled_t *oled, const oled_pgm_uint8_t *s) {
  return oled_font_calc_str_pixel_width(oled, (const char *)s, oled_font_get_charP);
}

int8_t oled_GetStrX(oled_t *oled, const char *s) {
  oled_GetGlyph(oled, *s);
  return oled->glyph_x;  
}

int8_t oled_GetStrXP(oled_t *oled, const oled_pgm_uint8_t *s) {
  oled_GetGlyph(oled, oled_pgm_read(s));
  return oled->glyph_x;  
}

/*========================================================================*/
/* string width calculation */

uint8_t oled_GetStrWidth(oled_t *oled, const char *s) {
  uint8_t  w;
  uint8_t encoding;
  
  /* reset the total width to zero, this will be expanded during calculation */
  w = 0;
  
  for(;;) {
    encoding = *s;
    if ( encoding == 0 )
      break;

    /* load glyph information */
    oled_GetGlyph(oled, encoding);
    w += oled->glyph_dx;    
    
    /* goto next char */
    s++;
  }
  
  return w;  
}


uint8_t oled_GetStrWidthP(oled_t *oled, const oled_pgm_uint8_t *s) {
  uint8_t  w;
  uint8_t encoding;
  
  /* reset the total width to zero, this will be expanded during calculation */
  w = 0;
  
  for(;;) {
    encoding = oled_pgm_read(s);
    if ( encoding == 0 )
      break;

    /* load glyph information */
    oled_GetGlyph(oled, encoding);
    w += oled->glyph_dx;    
    
    /* goto next char */
    s++;
  }
  
  return w;  
}


/*========================================================================*/
/* calculation of font/glyph/string characteristics */


/*
  Description:
    Calculate parameter for the minimal bounding box on a given string
  Output
    buf->y_min          extend of the lower left edge if the string below (y_min<0) or above (y_min>0) baseline (descent)
    buf->y_max          extend of the upper left edge if the string below (y_min<0) or above (y_min>0) baseline (ascent)
    buf->w                 the width of the string
*/
struct oled_str_size_struct
{
  int8_t y_min;         /* descent */
  int8_t y_max;         /* ascent */
  int8_t x, y;             /* the reference point of the font (negated!) */
  uint8_t w;         /* width of the overall string */
};
typedef struct oled_str_size_struct oled_str_size_t;

static void oled_font_calc_str_min_box(oled_t *oled, const char *s, oled_str_size_t *buf) {
  /* oled_glyph_t g; */
  int8_t tmp;
  
  /* reset the total minimal width to zero, this will be expanded during calculation */
  buf->w = 0;
    
  /* check for empty string, width is already 0, but also reset y_min and y_max to 0 */
  if ( *s == '\0' ) {
    buf->y_min = 0;
    buf->y_max = 0;
    buf->x = 0;
    buf->y = 0;
    return;
  }
  
  /* reset y_min to the largest possible value. Later we search for the smallest value */
  /* y_min contains the position [pixel] of the lower left edge of the glyph above (y_min>0) or below (y_min<0) baseline  */
  buf->y_min = 127;
  /* reset y_max to the smallest possible value. Later we search for the highest value */
  /* y_max contains the position [pixel] of the upper left edge of the glyph above (y_max>0) or below (y_max<0) baseline  */
  buf->y_max = -128;

  /* get the glyph information of the first char. This must be valid, because we already checked for the empty string */
  oled_GetGlyph(oled, *s);

  /* strlen(s) == 1:       width = width(s[0]) */
  /* strlen(s) == 2:       width = - offx(s[0]) + deltax(s[0]) + offx(s[1]) + width(s[1]) */
  /* strlen(s) == 3:       width = - offx(s[0]) + deltax(s[0]) + deltax(s[1]) + offx(s[2]) + width(s[2]) */
  
  /* assume that the string has size 2 or more, than start with negative offset-x */
  /* for string with size 1, this will be nullified after the loop */
  // buf->w = - oled_font_GetGlyphBBXOffX(oled->font, g);
  buf->w = - oled->glyph_x;
  
  /* Also copy the position of the first glyph. This is the reference point of the string (negated) */
  buf->x = oled->glyph_x;
  buf->y = oled->glyph_y;
  
  for(;;) {
    
    /* calculated y position of the upper left corner (y_max) and lower left corner (y_min) of the string */
    /* relative to the base line */
    
    tmp = oled->glyph_y;
    if ( buf->y_min > tmp )
      buf->y_min = tmp;
    
    tmp +=oled->glyph_height;
    if ( buf->y_max < tmp )
      buf->y_max = tmp;
    
    /* check and stop if the end of the string is reached */
    s++;
    if ( *s == '\0' )
      break;
    
    /* if there are still more characters, add the delta to the next glyph */
    buf->w += oled->glyph_dx;
    
    /* load the next glyph information */
    oled_GetGlyph(oled, *s);
  }
  
  /* finally calculate the width of the last char */
  /* if g was not updated in the for loop (strlen() == 1), then the initial offset x gets removed */
  buf->w += oled->glyph_width;
  // buf->w += oled_font_GetGlyphBBXOffX(oled->font, g);
  
  buf->w += oled->glyph_x;
}

/* calculate minimal box */
void oled_font_box_min(oled_t *oled, const char *s, oled_str_size_t *buf) {
  oled_font_calc_str_min_box(oled, s, buf);
}

/* calculate gA box, but do not calculate the overall width */
void oled_font_box_left_gA(oled_t *oled, const char *s, oled_str_size_t *buf) {
  
}

/* calculate gA box, including overall width */
void oled_font_box_all_gA(oled_t *oled, const char *s, oled_str_size_t *buf) {
  
}


static void oled_font_get_str_box_fill_args(oled_t *oled, const char *s, oled_str_size_t *buf, uint8_t *x, uint8_t *y, uint8_t *width, uint8_t *height) {
  /*
  oled_glyph_t g;
  g =
  */  
  oled_GetGlyph(oled, *s);
  *x += oled->glyph_x;
  *width = buf->w;
  *y -= buf->y_max;
  /* +1 because y_max is a height, this compensates the next step */
  //*y += 1;                      
  /* because the reference point is one below the string, this compensates the previous step */
  //*y -= 1;
  *height = buf->y_max;
  *height -= buf->y_min;
}


void oled_GetStrMinBox(oled_t *oled, const char *s, uint8_t *x, uint8_t *y, uint8_t *width, uint8_t *height) {
  oled_str_size_t buf;
  
  if ( *s == '\0' ) {
    *width= 0;
    *height = 0;
    return;
  }
  
  oled_font_calc_str_min_box(oled, s, &buf);
  oled_font_get_str_box_fill_args(oled, s, &buf, x, y, width, height);
}


void oled_GetStrAMinBox(oled_t *oled, const char *s, uint8_t *x, uint8_t *y, uint8_t *width, uint8_t *height) {
  oled_str_size_t buf;
  uint8_t cap_a;
  
  if ( *s == '\0' ) {
    *width= 0;
    *height = 0;
    return;
  }
  
  cap_a = oled_font_GetCapitalAHeight(oled->font);
  oled_font_calc_str_min_box(oled, s, &buf);
  if ( buf.y_max < cap_a )
    buf.y_max = cap_a;
  oled_font_get_str_box_fill_args(oled, s, &buf, x, y, width, height);
}

void oled_SetFont(oled_t *oled, const oled_fntpgm_uint8_t  *font) {
  if ( oled->font != font ) {
    oled->font = font;
    oled_UpdateRefHeight(oled);
    oled_SetFontPosBaseline(oled);
  }
}

/*========================================================================*/
/* anti aliasing fonts */

int8_t oled_draw_aa_glyph(oled_t *oled, uint8_t x, uint8_t y, uint8_t encoding) {
  const oled_pgm_uint8_t *data;
  uint8_t w, h;
  uint8_t i, j;
  uint8_t ix, iy;
 {
    oled_glyph_t g = oled_GetGlyph(oled, encoding);
    if ( g == NULL  )
      return 0;
    data = oled_font_GetGlyphDataStart(oled->font, g);
  }
  
  w = oled->glyph_width;
  h = oled->glyph_height;
  
  x += oled->glyph_x;
  y -= oled->glyph_y;
  y--;
  
  if ( oled_IsBBXIntersection(oled, x, y-h+1, w, h) == 0 )
    return oled->glyph_dx;

  /* now, w is reused as bytes per line */
  w += 3;
  w /= 4;
  
  iy = y;
  iy -= h;
  iy++;

  for( j = 0; j < h; j++ ) {
    ix = x;
    for( i = 0; i < w; i++ )
    {
      oled_Draw4TPixel(oled, ix, iy, 0, oled_pgm_read(data));
      data++;
      ix+=4;
    }
    iy++;
  }
  return oled->glyph_dx;
}

int8_t oled_DrawAAGlyph(oled_t *oled, uint8_t x, uint8_t y, uint8_t encoding) {
  y += oled->font_calc_vref(oled);
  return oled_draw_aa_glyph(oled, x, y, encoding);
}

uint8_t oled_DrawAAStr(oled_t *oled, uint8_t x, uint8_t y, const char *s) {
  uint8_t t = 0;
  int8_t d;

  if ( oled_font_GetFormat(oled->font)  != 2 )
    return 0;
  //uint8_t oled_GetStrWidth(oled, s);
  //oled_font_GetFontAscent(oled->font)-oled_font_GetFontDescent(oled->font);
  
  y += oled->font_calc_vref(oled);
  
  while( *s != '\0' ) {
    d = oled_draw_aa_glyph(oled, x, y, *s);
    x += d;
    t += d;
    s++;
  }
  return t;
}
