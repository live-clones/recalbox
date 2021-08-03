#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "display_driver.h"
#include "fonts.h"
#include "gui.h"
#include "log.h"

void gui_init(display_interface * display, display_handler * handler,uint32_t columns, uint32_t pages) {
  display->set_screen_size(handler, columns, pages);
}

void gui_clear(display_interface * display, display_handler * handler) {
  memset(handler->buffer, 0, handler->pages*handler->columns);
}

void gui_point(display_handler * handler, uint32_t x, uint32_t y, uint32_t color) {
  if (x > handler->columns || y > handler->pages)
    return;
  if(color == 1){
    handler->buffer[x + y / 8 * handler->columns] |= (0x01 << (y % 8));
  }else{
    handler->buffer[x + y / 8 * handler->columns] |= (0x00 << (y % 8));
  }
}

void gui_draw(display_interface * display, display_handler * handler) {
  display->send_buffer(handler);
}

void gui_partial_draw(display_interface * display, display_handler * handler, uint32_t start_x, uint32_t start_y, uint32_t end_x, uint32_t end_y) {
  display->send_partial_buffer(handler, start_x, start_y, end_x, end_y);
}

// draw one ASCII character
void gui_char(display_handler * handler, uint32_t x, uint32_t y, const char character,
                 sFONT* Font, uint32_t background_color, uint32_t foreground_color) {
    uint32_t page, column;

    if(x > handler->columns || y > handler->pages) {
      log_debug("gui_char Input exceeds the normal display range");
      return;
    }

    uint32_t Char_Offset = (character - ' ') * Font->Height *(Font->Width / 8 +(Font->Width % 8 ? 1 : 0));
    const unsigned char *ptr = &Font->table[Char_Offset];

    for(page = 0; page < Font->Height; page++ ) {
      for(column = 0; column < Font->Width; column ++ ) {

        //To determine whether the font background color and screen background color is consistent
        if(FONT_BACKGROUND == background_color) { //this process is to speed up the scan
          if(*ptr &(0x80 >>(column % 8)))
            gui_point(handler, x + column, y + page, foreground_color) ;//, DOT_PIXEL_DFT, DOT_STYLE_DFT);
        } else {
          if(*ptr &(0x80 >>(column % 8))) {
            gui_point(handler, x + column, y + page, foreground_color); //, DOT_PIXEL_DFT, DOT_STYLE_DFT);
          } else {
            gui_point(handler, x + column, y + page, background_color); //, DOT_PIXEL_DFT, DOT_STYLE_DFT);
          }
        }
        //One pixel is 8 bits
        if(column % 8 == 7)
          ptr++;
      }/* Write a line */
      if(Font->Width % 8 != 0)
        ptr++;
    }/* Write all */
}

void gui_string(display_handler * handler, uint32_t x, uint32_t y, const char * string,
                      sFONT* Font, uint32_t background_color, uint32_t foreground_color) {
    uint32_t Xpoint = x;
    uint32_t Ypoint = y;

    if(x > handler->columns || y > handler->pages) {
      log_debug("gui_string Input exceeds the normal display range");
      return;
    }

    while(* string != '\0') {
      //if X direction filled , reposition to(Xstart,Ypoint),Ypoint is Y direction plus the height of the character
      if((Xpoint + Font->Width ) > handler->columns ) {
        Xpoint = x;
        Ypoint += Font->Height;
      }

      // If the Y direction is full, reposition to(Xstart, Ystart)
      if((Ypoint  + Font->Height ) > handler->columns ) {
        Xpoint = x;
        Ypoint = y;
      }
      gui_char(handler, Xpoint, Ypoint, * string, Font, background_color, foreground_color);

        //The next character of the address
        string ++;

        //The next word of the abscissa increases the font of the broadband
        Xpoint += Font->Width;
    }
}
