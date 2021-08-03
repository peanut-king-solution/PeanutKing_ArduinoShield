/*
*/

#include "u8g.h"
#include <string.h>


void u8g_pb8v1_Init(u8g_pb_t *b, void *buf, u8g_uint_t width) U8G_NOINLINE;
void u8g_pb8v1_set_pixel(u8g_pb_t *b, u8g_uint_t x, u8g_uint_t y, uint8_t color_index) U8G_NOINLINE;
void u8g_pb8v1_SetPixel(u8g_pb_t *b, const u8g_dev_arg_pixel_t * const arg_pixel) U8G_NOINLINE ;
void u8g_pb8v1_Set8PixelStd(u8g_pb_t *b, u8g_dev_arg_pixel_t *arg_pixel) U8G_NOINLINE;


void u8g_pb16v1_Init(u8g_pb_t *b, void *buf, u8g_uint_t width) U8G_NOINLINE;
void u8g_pb16v1_set_pixel(u8g_pb_t *b, u8g_uint_t x, u8g_uint_t y, uint8_t color_index) U8G_NOINLINE;
void u8g_pb16v1_SetPixel(u8g_pb_t *b, const u8g_dev_arg_pixel_t * const arg_pixel) U8G_NOINLINE ;
void u8g_pb16v1_Set8PixelStd(u8g_pb_t *b, u8g_dev_arg_pixel_t *arg_pixel) U8G_NOINLINE;



void u8g_pb_Clear(u8g_pb_t *b) {
  uint8_t *ptr = (uint8_t *)b->buf;
  uint8_t *end_ptr = ptr;
  end_ptr += b->width;
  do {
    *ptr++ = 0;
  } while( ptr != end_ptr );
}

uint8_t u8g_pb_IsYIntersection(u8g_pb_t *pb, u8g_uint_t v0, u8g_uint_t v1) {
  uint8_t c1, c2, c3, tmp;
  c1 = v0 <= pb->p.page_y1;
  c2 = v1 >= pb->p.page_y0;
  c3 = v0 > v1;
  
  tmp = c1;
  c1 &= c2;
  c2 &= c3;
  c3 &= tmp;
  c1 |= c2;
  c1 |= c3;
  return c1 & 1;
}


uint8_t u8g_pb_IsXIntersection(u8g_pb_t *b, u8g_uint_t v0, u8g_uint_t v1) {
  uint8_t /*c0, c1, */ c2, c3;
  /* conditions: b->p.page_y0 < b->p.page_y1
    there are no restriction on v0 and v1. If v0 > v1, then warp around unsigned is assumed*/
  c2 = v0 > b->width;
  c3 = v1 > b->width;
  if ( c2 && c3 ) return 0;
  return 1;
}

uint8_t u8g_pb_IsIntersection(u8g_pb_t *pb, u8g_dev_arg_bbx_t *bbx) {
  u8g_uint_t tmp = bbx->y + bbx->h;
  tmp--;
  
  if ( u8g_pb_IsYIntersection(pb, bbx->y, tmp) == 0 )  return 0;
  /* maybe this one can be skiped... probability is very high to have an intersection, so it would be ok to always return 1 */
  tmp = bbx->x + bbx->w;
  tmp--;
  
  return u8g_pb_IsXIntersection(pb, bbx->x, tmp);
}

void u8g_pb_GetPageBox(u8g_pb_t *pb, u8g_box_t *box) {
  box->x0 = 0;
  box->y0 = pb->p.page_y0;
  box->x1 = pb->width;
  box->x1--;
  box->y1 = pb->p.page_y1;
}

uint8_t u8g_pb_Is8PixelVisible(u8g_pb_t *b, u8g_dev_arg_pixel_t *arg_pixel) {
  u8g_uint_t v0, v1;
  v0 = arg_pixel->y;
  v1 = v0;
  switch( arg_pixel->dir ) {
    case 0:
      break;
    case 1:  
      v1 += 8;          /* this is independent from the page height */
      break;
    case 2:  
      break;
    case 3: 
      v0 -= 8;
      break;
  }
  return u8g_pb_IsYIntersection(b, v0, v1);
}






/* Obsolete, usually set by the init of the structure */
void u8g_pb8v1_Init(u8g_pb_t *b, void *buf, u8g_uint_t width) {
  b->buf = buf;
  b->width = width;
  u8g_pb_Clear(b);
}

void u8g_pb8v1_set_pixel(u8g_pb_t *b, u8g_uint_t x, u8g_uint_t y, uint8_t color_index) {
  register uint8_t mask;
  uint8_t *ptr = b->buf;
  
  y -= b->p.page_y0;
  mask = 1;
  y &= 0x07;
  mask <<= y;
  ptr += x;
  if ( color_index ) {
    *ptr |= mask;
  }
  else {
    mask ^=0xff;
    *ptr &= mask;
  }
}

void u8g_pb8v1_SetPixel(u8g_pb_t *b, const u8g_dev_arg_pixel_t * const arg_pixel) {
  if ( arg_pixel->y < b->p.page_y0 )  return;
  if ( arg_pixel->y > b->p.page_y1 )  return;
  if ( arg_pixel->x >= b->width )     return;
  u8g_pb8v1_set_pixel(b, arg_pixel->x, arg_pixel->y, arg_pixel->color);
}

void u8g_pb8v1_Set8PixelStd(u8g_pb_t *b, u8g_dev_arg_pixel_t *arg_pixel) {
  register uint8_t pixel = arg_pixel->pixel;
  do {
    if ( pixel & 128 ) {
      u8g_pb8v1_SetPixel(b, arg_pixel);
    }
    switch( arg_pixel->dir ) {
      case 0: arg_pixel->x++; break;
      case 1: arg_pixel->y++; break;
      case 2: arg_pixel->x--; break;
      case 3: arg_pixel->y--; break;
    }
    pixel <<= 1;
  } while( pixel != 0  );
}


void u8g_pb8v1_Set8PixelOpt2(u8g_pb_t *b, u8g_dev_arg_pixel_t *arg_pixel) {
  register uint8_t pixel = arg_pixel->pixel;
  u8g_uint_t dx = 0;
  u8g_uint_t dy = 0;
  
  switch( arg_pixel->dir ) {
    case 0: dx++; break;
    case 1: dy++; break;
    case 2: dx--; break;
    case 3: dy--; break;
  }
  
  do {
    if ( pixel & 128 )
      u8g_pb8v1_SetPixel(b, arg_pixel);
    arg_pixel->x += dx;
    arg_pixel->y += dy;
    pixel <<= 1;
  } while( pixel != 0  );
  
}

uint8_t u8g_dev_pb8v1_base_fn(u8g_t *u8g, u8g_dev_t *dev, uint8_t msg, void *arg) {
  u8g_pb_t *pb = (u8g_pb_t *)(dev->dev_mem);
  switch(msg) {
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




void u8g_pb16v1_Clear(u8g_pb_t *b) {
  uint8_t *ptr = (uint8_t *)b->buf;
  uint8_t *end_ptr = ptr;
  end_ptr += b->width*2;
  do {
    *ptr++ = 0;
  } while( ptr != end_ptr );
}

/* Obsolete, usually set by the init of the structure */
void u8g_pb16v1_Init(u8g_pb_t *b, void *buf, u8g_uint_t width) {
  b->buf = buf;
  b->width = width;
  u8g_pb16v1_Clear(b);
}

void u8g_pb16v1_set_pixel(u8g_pb_t *b, u8g_uint_t x, u8g_uint_t y, uint8_t color_index) {
  register uint8_t mask;
  uint8_t *ptr = b->buf;
  
  y -= b->p.page_y0;
  if ( y >= 8 ) {
    ptr += b->width;
    y &= 0x07;
  }
  mask = 1;
  mask <<= y;
  ptr += x;
  if ( color_index ) {
    *ptr |= mask;
  }
  else {
    mask ^=0xff;
    *ptr &= mask;
  }
}


void u8g_pb16v1_SetPixel(u8g_pb_t *b, const u8g_dev_arg_pixel_t * const arg_pixel) {
  if ( arg_pixel->y < b->p.page_y0 )  return;
  if ( arg_pixel->y > b->p.page_y1 )  return;
  if ( arg_pixel->x >= b->width )     return;
  u8g_pb16v1_set_pixel(b, arg_pixel->x, arg_pixel->y, arg_pixel->color);
}

void u8g_pb16v1_Set8PixelStd(u8g_pb_t *b, u8g_dev_arg_pixel_t *arg_pixel) {
  register uint8_t pixel = arg_pixel->pixel;
  do {
    if ( pixel & 128 ) {
      u8g_pb16v1_SetPixel(b, arg_pixel);
    }
    switch( arg_pixel->dir ) {
      case 0: arg_pixel->x++; break;
      case 1: arg_pixel->y++; break;
      case 2: arg_pixel->x--; break;
      case 3: arg_pixel->y--; break;
    }
    pixel <<= 1;
  } while( pixel != 0  );
}


void u8g_pb16v1_Set8PixelOpt2(u8g_pb_t *b, u8g_dev_arg_pixel_t *arg_pixel) {
  register uint8_t pixel = arg_pixel->pixel;
  u8g_uint_t dx = 0;
  u8g_uint_t dy = 0;
  
  switch( arg_pixel->dir ) {
    case 0: dx++; break;
    case 1: dy++; break;
    case 2: dx--; break;
    case 3: dy--; break;
  }
  
  do {
    if ( pixel & 128 )
      u8g_pb16v1_SetPixel(b, arg_pixel);
    arg_pixel->x += dx;
    arg_pixel->y += dy;
    pixel <<= 1;
  } while( pixel != 0  );
  
}

uint8_t u8g_dev_pb16v1_base_fn(u8g_t *u8g, u8g_dev_t *dev, uint8_t msg, void *arg) {
  u8g_pb_t *pb = (u8g_pb_t *)(dev->dev_mem);
  switch(msg) {
    case U8G_DEV_MSG_SET_8PIXEL:
      if ( u8g_pb_Is8PixelVisible(pb, (u8g_dev_arg_pixel_t *)arg) )
        u8g_pb16v1_Set8PixelOpt2(pb, (u8g_dev_arg_pixel_t *)arg);
      break;
    case U8G_DEV_MSG_SET_PIXEL:
        u8g_pb16v1_SetPixel(pb, (u8g_dev_arg_pixel_t *)arg);
      break;
    case U8G_DEV_MSG_INIT:
      break;
    case U8G_DEV_MSG_STOP:
      break;
    case U8G_DEV_MSG_PAGE_FIRST:
      u8g_pb16v1_Clear(pb);
      u8g_page_First(&(pb->p));
      break;
    case U8G_DEV_MSG_PAGE_NEXT:
      if ( u8g_page_Next(&(pb->p)) == 0 )
        return 0;
      u8g_pb16v1_Clear(pb);
      break;
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
 
  