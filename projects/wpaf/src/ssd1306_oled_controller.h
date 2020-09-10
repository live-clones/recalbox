#ifndef __SSD1306_OLED_CONTROLLER_H
#define __SSD1306_OLED_CONTROLLER_H

#include <stdint.h>

#include "oled_driver.h"

// defines screen height and length
#define OLED_WIDTH   128
#define OLED_HEIGHT  32
#define OLED_COLUMN  OLED_WIDTH
#define OLED_PAGE    OLED_HEIGHT / 8

oled_handler * ssd1306_oled_init(uint32_t oled_address);
void ssd1306_oled_clear(oled_handler *);
void ssd1306_oled_send_buffer(oled_handler *);
void ssd1306_set_screen_size(oled_handler *, uint32_t, uint32_t);
void ssd1306_oled_close(oled_handler *);

extern oled_interface ssd1306_interface;

#endif //__SSD1306_OLED_CONTROLLER_H
