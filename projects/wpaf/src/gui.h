#ifndef __GUI_H
#define __GUI_H

#include <stdint.h>

#include "oled_driver.h"
#include "fonts.h"

#define WHITE     1
#define BLACK     0
#define OLED_BACKGROUND   BLACK
#define FONT_BACKGROUND   BLACK
#define FONT_FOREGROUND   WHITE

void gui_init(oled_interface * oled, oled_handler * handler,uint32_t columns, uint32_t pages);
void gui_clear(oled_interface * oled, oled_handler * handler);
void gui_point(oled_handler*, uint32_t, uint32_t, uint32_t);
void gui_draw(oled_interface *, oled_handler *);
void gui_partial_draw(oled_interface *, oled_handler *, uint32_t, uint32_t, uint32_t, uint32_t);
void gui_char(oled_handler * handler, uint32_t x, uint32_t y, const char character,
                   sFONT* Font, uint32_t background_color, uint32_t foreground_color);
void gui_string(oled_handler * handler, uint32_t x, uint32_t y, const char * string,
                        sFONT* Font, uint32_t background_color, uint32_t foreground_color);

#endif //__GUI_H

