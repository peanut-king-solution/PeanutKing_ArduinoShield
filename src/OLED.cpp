/*

*/

#include "OLED.h"





uint8_t OLED::initI2C(u8g_dev_t *dev, uint8_t options) {
  prepare();
  return u8g_InitI2C(&u8g, dev, options);
}

uint8_t u8g_dev_rot_dummy_fn(u8g_t *u8g, u8g_dev_t*dev, uint8_t msg, void *arg) {
  return 0;
}

u8g_dev_t u8g_dev_rot = { u8g_dev_rot_dummy_fn, NULL, NULL };

