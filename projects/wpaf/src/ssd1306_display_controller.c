#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "config.h"
#include "ssd1306_display_controller.h"
#include "wpaf_i2c.h"

display_interface ssd1306_interface = {
  &ssd1306_display_init,
  &ssd1306_display_close,
  &ssd1306_display_clear,
  &ssd1306_display_send_buffer,
  &ssd1306_display_send_partial_buffer,
  &ssd1306_set_screen_size,
  &ssd1306_display_hscroll,
  &ssd1306_display_reset,
};

// allocate handler
display_handler * ssd1306_display_init(uint8_t address) {
  display_handler * handler = malloc(sizeof(display_handler));
  handler->address = address;
  return handler;
}

// reset ssd1306
// init sequence is passed through extra_data in handler
void ssd1306_display_reset(display_handler * display) {
  int i;
  SSD1306_data_struct * data = display->extra_data;
  
  for(i=0; i<data->display_init_sequence_length; i++) {
    printf("add: %d - data: %d\n", display->address,data->display_init_sequence[i]);
    SSD1306_WRITE_REG(display->address, data->display_init_sequence[i]);
  }

  ssd1306_display_clear(display);
  ssd1306_set_screen_size(display, 0, 0);
} 

void ssd1306_display_clear(display_handler * display) {
  uint8_t * clean_buffer;
  uint8_t * save_buffer = display->buffer; // save current buffer, as we need to pass a clean one
  clean_buffer = calloc(OLED_PAGE * OLED_COLUMN, sizeof(uint8_t));
  display->buffer = clean_buffer;
  ssd1306_display_send_buffer(display);
  display->buffer = save_buffer;
  free(clean_buffer);
}

void ssd1306_set_screen_size(display_handler * display, uint32_t columns, uint32_t pages) {
  display->columns = columns;
  display->pages = pages;
  display->buffer = malloc(pages*columns);
}

void ssd1306_display_send_buffer(display_handler * display) {
  int column,page;
  uint8_t * ptr = display->buffer;
  for(page = 0; page < OLED_PAGE; page++) {
    SSD1306_WRITE_REG(display->address, 0xb0 + page); //Set page address
    SSD1306_WRITE_REG(display->address, 0x00);        //Set display position - column low address
    SSD1306_WRITE_REG(display->address, 0x10);        //Set display position - column high address
    for(column = 0; column < OLED_COLUMN; column++) {
      SSD1306_WRITE_MEM(display->address, *ptr);
      ptr++;
    }
  }
}

/** untested **/
void ssd1306_display_send_partial_buffer(display_handler * display, uint32_t x, uint32_t y, uint32_t end_x, uint32_t end_y) {
  int column,page;
  uint8_t * ptr = display->buffer;
  for(page = y; page < end_y/8; page++) {
    SSD1306_WRITE_REG(display->address, 0xb0 + page); //Set page address
    SSD1306_WRITE_REG(display->address, 0x00);        //Set display position - column low address
    SSD1306_WRITE_REG(display->address, 0x10);        //Set display position - column high address
    for(column = x; column < end_x; column++) {
      SSD1306_WRITE_MEM(display->address, *ptr);
      ptr++;
    }
  }
}

// free handler
void ssd1306_display_close(display_handler * display) {
  if (display->buffer) free(display->buffer);
}

void ssd1306_display_hscroll(display_handler * display, uint32_t page_start, uint32_t page_end, uint32_t time_interval) {
  SSD1306_WRITE_REG(display->address, 0x2e); // end scroll
  SSD1306_WRITE_REG(display->address, 0x27); // set scroll
  SSD1306_WRITE_REG(display->address, 0x00); // dummy
  SSD1306_WRITE_REG(display->address, page_start);
  SSD1306_WRITE_REG(display->address, time_interval);
  SSD1306_WRITE_REG(display->address, page_end);
  SSD1306_WRITE_REG(display->address, 0x00); // dummy
  SSD1306_WRITE_REG(display->address, 0xff); // dummy
  SSD1306_WRITE_REG(display->address, 0x2f); // start scroll
}

void _ssd1306_write_byte(uint8_t address, uint8_t cmd, uint8_t value) {
  char wbuf[2]={cmd, value};
  wpaf_i2c_write_to_addr(address, wbuf, 2);
}
  
