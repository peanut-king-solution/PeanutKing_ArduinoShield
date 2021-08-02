/*
*/

#include "u8g.h"

uint8_t u8g_dev_scale_2x2_fn(u8g_t *u8g, u8g_dev_t *dev, uint8_t msg, void *arg);


u8g_dev_t u8g_dev_scale = { u8g_dev_scale_2x2_fn, NULL, NULL };

void u8g_UndoScale(u8g_t *u8g) {
  if ( u8g->dev != &u8g_dev_scale )
    return;
  u8g->dev = u8g_dev_scale.dev_mem;
  u8g_UpdateDimension(u8g);
}

void u8g_SetScale2x2(u8g_t *u8g) {
  if ( u8g->dev != &u8g_dev_scale ) {
    u8g_dev_scale.dev_mem = u8g->dev;
    u8g->dev = &u8g_dev_scale;
  }
  u8g_dev_scale.dev_fn = u8g_dev_scale_2x2_fn;
  u8g_UpdateDimension(u8g);
}


uint8_t u8g_dev_scale_2x2_fn(u8g_t *u8g, u8g_dev_t *dev, uint8_t msg, void *arg) {
  u8g_dev_t *chain = (u8g_dev_t *)(dev->dev_mem);
  uint8_t pixel;
  uint16_t scaled_pixel;
  uint8_t i;
  uint8_t dir;
  u8g_uint_t x, y, xx,yy;
  
  switch(msg) {
    default:
      return u8g_call_dev_fn(u8g, chain, msg, arg);
    case U8G_DEV_MSG_GET_WIDTH:
      *((u8g_uint_t *)arg) = u8g_GetWidthLL(u8g, chain) / 2;
      break;
    case U8G_DEV_MSG_GET_HEIGHT:
      *((u8g_uint_t *)arg) = u8g_GetHeightLL(u8g, chain) / 2;
      break;
    case U8G_DEV_MSG_GET_PAGE_BOX:
      /* get page size from next device in the chain */
      u8g_call_dev_fn(u8g, chain, msg, arg);
      ((u8g_box_t *)arg)->x0 /= 2;
      ((u8g_box_t *)arg)->x1 /= 2;
      ((u8g_box_t *)arg)->y0 /= 2;
      ((u8g_box_t *)arg)->y1 /= 2;
      return 1;
    case U8G_DEV_MSG_SET_PIXEL:
      x = ((u8g_dev_arg_pixel_t *)arg)->x;
      x *= 2;
      y = ((u8g_dev_arg_pixel_t *)arg)->y;
      y *= 2;
      ((u8g_dev_arg_pixel_t *)arg)->x = x;
      ((u8g_dev_arg_pixel_t *)arg)->y = y;
      u8g_call_dev_fn(u8g, chain, msg, arg);
      x++;
      ((u8g_dev_arg_pixel_t *)arg)->x = x;
      ((u8g_dev_arg_pixel_t *)arg)->y = y;
      u8g_call_dev_fn(u8g, chain, msg, arg);
      y++;
      ((u8g_dev_arg_pixel_t *)arg)->x = x;
      ((u8g_dev_arg_pixel_t *)arg)->y = y;
      u8g_call_dev_fn(u8g, chain, msg, arg);
      x--;
      ((u8g_dev_arg_pixel_t *)arg)->x = x;
      ((u8g_dev_arg_pixel_t *)arg)->y = y;
      u8g_call_dev_fn(u8g, chain, msg, arg);    
      break;
    case U8G_DEV_MSG_SET_8PIXEL:
      pixel = ((u8g_dev_arg_pixel_t *)arg)->pixel;
      dir = ((u8g_dev_arg_pixel_t *)arg)->dir;
      scaled_pixel = 0;
      for( i = 0; i < 8; i++ ) {
        scaled_pixel<<=2;
        if ( pixel & 128 ) {
          scaled_pixel |= 3;
        }
        pixel<<=1;
      }
      x = ((u8g_dev_arg_pixel_t *)arg)->x;
      x *= 2;
      xx = x;
      y = ((u8g_dev_arg_pixel_t *)arg)->y;
      y *= 2;
      yy = y;
      if ( ((u8g_dev_arg_pixel_t *)arg)->dir & 1 ) {
        xx++;
      }
      else {
        yy++;
      }
      
      ((u8g_dev_arg_pixel_t *)arg)->pixel = scaled_pixel>>8;      
      ((u8g_dev_arg_pixel_t *)arg)->x = x;
      ((u8g_dev_arg_pixel_t *)arg)->y = y;
      ((u8g_dev_arg_pixel_t *)arg)->dir = dir;
      u8g_call_dev_fn(u8g, chain, msg, arg);    

      
      ((u8g_dev_arg_pixel_t *)arg)->x = xx;
      ((u8g_dev_arg_pixel_t *)arg)->y = yy;
      ((u8g_dev_arg_pixel_t *)arg)->dir = dir;
      u8g_call_dev_fn(u8g, chain, msg, arg);    
      
      ((u8g_dev_arg_pixel_t *)arg)->pixel = scaled_pixel&255;
      //((u8g_dev_arg_pixel_t *)arg)->pixel = 0x00;
      switch(dir) {
        case 0:
          x+=8;
          xx+=8;
          break;
        case 1:
          y+=8;
          yy+=8;
          break;
        case 2:
          x-=8;
          xx-=8;
          break;
        case 3:
          y-=8;
          yy-=8;
          break;
      }
      ((u8g_dev_arg_pixel_t *)arg)->x = x;
      ((u8g_dev_arg_pixel_t *)arg)->y = y;
      ((u8g_dev_arg_pixel_t *)arg)->dir = dir;
      u8g_call_dev_fn(u8g, chain, msg, arg);    
      
      ((u8g_dev_arg_pixel_t *)arg)->x = xx;
      ((u8g_dev_arg_pixel_t *)arg)->y = yy;
      ((u8g_dev_arg_pixel_t *)arg)->dir = dir;
      u8g_call_dev_fn(u8g, chain, msg, arg);    
      break;
  }
  return 1;
}

