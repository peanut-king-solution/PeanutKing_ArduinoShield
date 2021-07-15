/*

*/

#include <stddef.h>
#include "u8g.h"

void u8g_state_dummy_cb(uint8_t msg) {
  /* the dummy procedure does nothing */
}

void u8g_SetHardwareBackup(u8g_t *u8g, u8g_state_cb backup_cb) {
  u8g->state_cb = backup_cb;
  /* in most cases the init message was already sent, so this will backup the */
  /* current u8g state */
  backup_cb(U8G_STATE_MSG_BACKUP_U8G);
}

/*===============================================================*/
/* register variable for restoring interrupt state */

#if defined(__AVR__)
uint8_t global_SREG_backup;
#endif

void u8g_backup_spi(uint8_t msg) {}

uint8_t u8g_call_dev_fn(u8g_t *u8g, u8g_dev_t *dev, uint8_t msg, void *arg) {
  return dev->dev_fn(u8g, dev, msg, arg); 
}


/*================= page =====================*/
/*====================================================================*/
/*
  setup page count structure
  conditions: page_height <= total_height
*/
void u8g_page_Init(u8g_page_t *p, u8g_uint_t page_height, u8g_uint_t total_height ) {
  p->page_height = page_height;
  p->total_height = total_height;
  p->page = 0;
  u8g_page_First(p);
}

void u8g_page_First(u8g_page_t *p) {
  p->page_y0 = 0;
  p->page_y1 = p->page_height;
  p->page_y1--;
  p->page = 0;
}

uint8_t u8g_page_Next(u8g_page_t * p) {
  register u8g_uint_t y1;
  p->page_y0 += p->page_height;
  if ( p->page_y0 >= p->total_height )
    return 0;
  p->page++;
  y1 = p->page_y1;
  y1 += p->page_height;
  if ( y1 >= p->total_height )   {
    y1 = p->total_height;
    y1--;
  }
  p->page_y1 = y1;
  
  return 1;
}

/*====================================================================*/

uint8_t u8g_InitLL(u8g_t *u8g, u8g_dev_t *dev) {
  uint8_t r;
  u8g->state_cb(U8G_STATE_MSG_BACKUP_ENV);
  r =  u8g_call_dev_fn(u8g, dev, U8G_DEV_MSG_INIT, NULL);
  u8g->state_cb(U8G_STATE_MSG_BACKUP_U8G);
  u8g->state_cb(U8G_STATE_MSG_RESTORE_ENV);
  return r;
}

void u8g_FirstPageLL(u8g_t *u8g, u8g_dev_t *dev) {  
  u8g->state_cb(U8G_STATE_MSG_BACKUP_ENV);
  u8g->state_cb(U8G_STATE_MSG_RESTORE_U8G);
  u8g_call_dev_fn(u8g, dev, U8G_DEV_MSG_PAGE_FIRST, NULL);
  u8g_call_dev_fn(u8g, dev, U8G_DEV_MSG_GET_PAGE_BOX, &(u8g->current_page));
  u8g->state_cb(U8G_STATE_MSG_RESTORE_ENV);
}

uint8_t u8g_NextPageLL(u8g_t *u8g, u8g_dev_t *dev) {  
  uint8_t r;
  u8g->state_cb(U8G_STATE_MSG_BACKUP_ENV);
  u8g->state_cb(U8G_STATE_MSG_RESTORE_U8G);
  r = u8g_call_dev_fn(u8g, dev, U8G_DEV_MSG_PAGE_NEXT, NULL);
  if ( r != 0 )
  {
    u8g_call_dev_fn(u8g, dev, U8G_DEV_MSG_GET_PAGE_BOX, &(u8g->current_page));
  }
  u8g->state_cb(U8G_STATE_MSG_RESTORE_ENV);
  return r;
}

uint8_t u8g_SetContrastLL(u8g_t *u8g, u8g_dev_t *dev, uint8_t contrast) {  
  return u8g_call_dev_fn(u8g, dev, U8G_DEV_MSG_CONTRAST, &contrast);
}

void u8g_DrawPixelLL(u8g_t *u8g, u8g_dev_t *dev, u8g_uint_t x, u8g_uint_t y) {
  u8g_dev_arg_pixel_t *arg = &(u8g->arg_pixel);
  arg->x = x;
  arg->y = y;
  u8g_call_dev_fn(u8g, dev, U8G_DEV_MSG_SET_PIXEL, arg);
}

void u8g_Draw8PixelLL(u8g_t *u8g, u8g_dev_t *dev, u8g_uint_t x, u8g_uint_t y, uint8_t dir, uint8_t pixel) {
  u8g_dev_arg_pixel_t *arg = &(u8g->arg_pixel);
  arg->x = x;
  arg->y = y;
  arg->dir = dir;
  arg->pixel = pixel;
  u8g_call_dev_fn(u8g, dev, U8G_DEV_MSG_SET_8PIXEL, arg);
}

void u8g_Draw4TPixelLL(u8g_t *u8g, u8g_dev_t *dev, u8g_uint_t x, u8g_uint_t y, uint8_t dir, uint8_t pixel) {
  u8g_dev_arg_pixel_t *arg = &(u8g->arg_pixel);
  arg->x = x;
  arg->y = y;
  arg->dir = dir;
  arg->pixel = pixel;
  u8g_call_dev_fn(u8g, dev, U8G_DEV_MSG_SET_4TPIXEL, arg);
}

u8g_uint_t u8g_GetWidthLL(u8g_t *u8g, u8g_dev_t *dev) {
  u8g_uint_t r;
  u8g_call_dev_fn(u8g, dev, U8G_DEV_MSG_GET_WIDTH, &r);
  return r;
}

u8g_uint_t u8g_GetHeightLL(u8g_t *u8g, u8g_dev_t *dev) {       
  u8g_uint_t r;
  u8g_call_dev_fn(u8g, dev, U8G_DEV_MSG_GET_HEIGHT, &r);
  return r;
}

u8g_uint_t u8g_GetModeLL(u8g_t *u8g, u8g_dev_t *dev) {       
  return u8g_call_dev_fn(u8g, dev, U8G_DEV_MSG_GET_MODE, NULL);
}



/*====================================================================*/

void u8g_UpdateDimension(u8g_t *u8g) {
  u8g->width = u8g_GetWidthLL(u8g, u8g->dev);
  u8g->height = u8g_GetHeightLL(u8g, u8g->dev);
  u8g->mode = u8g_GetModeLL(u8g, u8g->dev);
  /* 9 Dec 2012: u8g_scale.c requires update of current page */
  u8g_call_dev_fn(u8g, u8g->dev, U8G_DEV_MSG_GET_PAGE_BOX, &(u8g->current_page));
}

static void u8g_init_data(u8g_t *u8g) {
  u8g->font = NULL;
  u8g->cursor_font = NULL;
  u8g->cursor_bg_color = 0;
  u8g->cursor_fg_color = 1;
  u8g->cursor_encoding = 34;
  u8g->cursor_fn = (u8g_draw_cursor_fn)0;

#if defined(U8G_WITH_PINLIST)  
  {
    uint8_t i;
    for( i = 0; i < U8G_PIN_LIST_LEN; i++ )
      u8g->pin_list[i] = U8G_PIN_NONE;
  }
#endif
  
  u8g_SetColorIndex(u8g, 1);

  u8g_SetFontPosBaseline(u8g);
  
  u8g->font_height_mode = U8G_FONT_HEIGHT_MODE_XTEXT;
  u8g->font_ref_ascent = 0;
  u8g->font_ref_descent = 0;
  u8g->font_line_spacing_factor = 64;           /* 64 = 1.0, 77 = 1.2 line spacing factor */
  u8g->line_spacing = 0;
  
  u8g->state_cb = u8g_state_dummy_cb;

}

uint8_t u8g_Begin(u8g_t *u8g) {
  /* call and init low level driver and com device */
  if ( u8g_InitLL(u8g, u8g->dev) == 0 )
    return 0;
  /* fetch width and height from the low level */
  u8g_UpdateDimension(u8g);
  return 1;
}

uint8_t u8g_Init(u8g_t *u8g, u8g_dev_t *dev) {
  u8g_init_data(u8g);
  u8g->dev = dev;
  
  /* On the Arduino Environment this will lead to two calls to u8g_Begin(), the following line will be called first (by U8glib constructors) */
  /* if - in future releases - this is removed, then still call u8g_UpdateDimension() */
  /* if Arduino call u8g_UpdateDimension else u8g_Begin */
  /* issue 146 */
  return u8g_Begin(u8g);
}

/* special init for pure ARM systems */
uint8_t u8g_InitComFn(u8g_t *u8g, u8g_dev_t *dev, u8g_com_fnptr com_fn) {
  u8g_init_data(u8g);
  
#if defined(U8G_WITH_PINLIST)  
  {
    uint8_t i;
    for( i = 0; i < U8G_PIN_LIST_LEN; i++ )
      u8g->pin_list[i] = U8G_PIN_DUMMY;
  }
#endif
  
  u8g->dev = dev;
  
  /* replace the device procedure with a custom communication procedure */
  u8g->dev->com_fn = com_fn;
  
  /* On the Arduino Environment this will lead to two calls to u8g_Begin(), the following line will be called first (by U8glib constructors) */
  /* if - in future releases - this is removed, then still call u8g_UpdateDimension() */
  /* if Arduino call u8g_UpdateDimension else u8g_Begin */
  /* issue 146 */
  return u8g_Begin(u8g);
}


#if defined(U8G_WITH_PINLIST)  
uint8_t u8g_InitI2C(u8g_t *u8g, u8g_dev_t *dev, uint8_t options) {
  /* fill data structure with some suitable values */
  u8g_init_data(u8g);
  u8g->dev = dev;
    
  u8g->pin_list[U8G_PI_I2C_OPTION] = options;
  
  return u8g_Begin(u8g);
}
#endif /* defined(U8G_WITH_PINLIST)  */

void u8g_FirstPage(u8g_t *u8g) {
  u8g_FirstPageLL(u8g, u8g->dev);
}

uint8_t u8g_NextPage(u8g_t *u8g) {
  if  ( u8g->cursor_fn != (u8g_draw_cursor_fn)0 )  {
    u8g->cursor_fn(u8g);
  }
  return u8g_NextPageLL(u8g, u8g->dev);
}

uint8_t u8g_SetContrast(u8g_t *u8g, uint8_t contrast) {
  return u8g_SetContrastLL(u8g, u8g->dev, contrast);
}

void u8g_SleepOn(u8g_t *u8g) {
  u8g_call_dev_fn(u8g, u8g->dev, U8G_DEV_MSG_SLEEP_ON, NULL);
}

void u8g_SleepOff(u8g_t *u8g) {
  u8g_call_dev_fn(u8g, u8g->dev, U8G_DEV_MSG_SLEEP_OFF, NULL);
}


void u8g_DrawPixel(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y) {
  u8g_DrawPixelLL(u8g, u8g->dev, x, y);
}

void u8g_Draw8Pixel(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, uint8_t dir, uint8_t pixel) {
  u8g_Draw8PixelLL(u8g, u8g->dev, x, y, dir, pixel);
}

void u8g_Draw4TPixel(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, uint8_t dir, uint8_t pixel) {
  u8g_Draw4TPixelLL(u8g, u8g->dev, x, y, dir, pixel);
}

void u8g_Draw8ColorPixel(u8g_t *u8g, u8g_uint_t x, u8g_uint_t y, uint8_t colpixel) {
  u8g_dev_arg_pixel_t *arg = &(u8g->arg_pixel);
  arg->x = x;
  arg->y = y;
  arg->dir = 0;
  arg->pixel = 0x80;
  arg->color = colpixel;
  u8g_call_dev_fn(u8g, u8g->dev, U8G_DEV_MSG_SET_8PIXEL, arg);
}

/*
  idx: index for the palette entry (0..255)
  r: value for red (0..255)
  g: value for green (0..255)
  b: value for blue (0..255)
*/
void u8g_SetColorEntry(u8g_t *u8g, uint8_t idx, uint8_t r, uint8_t g, uint8_t b) {
  u8g_dev_arg_irgb_t irgb;
  irgb.idx = idx;
  irgb.r = r;
  irgb.g = g;
  irgb.b = b;  
  u8g_call_dev_fn(u8g, u8g->dev, U8G_DEV_MSG_SET_COLOR_ENTRY, &irgb);
}

void u8g_SetColorIndex(u8g_t *u8g, uint8_t idx) {
  u8g->arg_pixel.color = idx;
}

void u8g_SetHiColor(u8g_t *u8g, uint16_t rgb) {
  u8g->arg_pixel.color = rgb&255;
  u8g->arg_pixel.hi_color = rgb>>8;
}

void u8g_SetHiColorByRGB(u8g_t *u8g, uint8_t r, uint8_t g, uint8_t b) {
  r &= ~7;
  g >>= 2;
  b >>= 3;
  u8g->arg_pixel.color = b;
  u8g->arg_pixel.color |= (g & 7) << 5;
  u8g->arg_pixel.hi_color = r;
  u8g->arg_pixel.hi_color |= (g>>3) & 7;
}

void u8g_SetRGB(u8g_t *u8g, uint8_t r, uint8_t g, uint8_t b) {
  if ( u8g->mode == U8G_MODE_R3G3B2 ) {
    r &= 0x0e0;
    g &= 0x0e0;
    g >>= 3;
    b >>= 6;
    u8g->arg_pixel.color = r | g | b;
  }
  else if ( u8g->mode == U8G_MODE_HICOLOR ) {
    u8g_SetHiColorByRGB(u8g, r,g,b);
  }
  else {
    u8g->arg_pixel.color = r;
    u8g->arg_pixel.hi_color = g;
    u8g->arg_pixel.blue = b;
  }
}


uint8_t u8g_GetColorIndex(u8g_t *u8g) {
  return u8g->arg_pixel.color;
}

uint8_t u8g_GetDefaultForegroundColor(u8g_t *u8g) {
  uint8_t mode;
  mode = u8g_GetMode(u8g);
  if ( mode == U8G_MODE_R3G3B2 ) 
    return 255;     /* white */
  else if ( u8g_GetMode(u8g) == U8G_MODE_GRAY2BIT )
    return 3;         /* max intensity */
  /* if ( u8g.getMode() == U8G_MODE_BW ) */
  return 1;         /* pixel on */
}

void u8g_SetDefaultForegroundColor(u8g_t *u8g) {
  if ( u8g->mode == U8G_MODE_HICOLOR ) {
    u8g->arg_pixel.color = 0x0ff;
    u8g->arg_pixel.hi_color = 0x0ff;
  }
  else {
    u8g_SetColorIndex(u8g, u8g_GetDefaultForegroundColor(u8g));
  }
}

uint8_t u8g_GetDefaultBackgroundColor(u8g_t *u8g) {
  return 0;
}

void u8g_SetDefaultBackgroundColor(u8g_t *u8g) {
  u8g_SetColorIndex(u8g, u8g_GetDefaultBackgroundColor(u8g));         /* pixel on / black */
}

uint8_t u8g_GetDefaultMidColor(u8g_t *u8g) {
  uint8_t mode;
  mode = u8g_GetMode(u8g);
  if ( mode == U8G_MODE_R3G3B2 ) 
    return 0x06d;     /* gray: 01101101 */
  else if ( mode == U8G_MODE_GRAY2BIT )
    return 1;         /* low mid intensity */
  /* if ( u8g.getMode() == U8G_MODE_BW ) */
  return 1;         /* pixel on */
}

void u8g_SetDefaultMidColor(u8g_t *u8g) {
  u8g_SetColorIndex(u8g, u8g_GetDefaultMidColor(u8g));
}




