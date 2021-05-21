#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "config.h"
#include "ssd1306_oled_controller.h"
#include "wpaf_i2c.h"

oled_interface ssd1306_interface = {
  &ssd1306_oled_init,
  &ssd1306_oled_close,
  &ssd1306_oled_clear,
  &ssd1306_oled_send_buffer,
  &ssd1306_oled_send_partial_buffer,
  &ssd1306_set_screen_size,
  &ssd1306_oled_hscroll,
  &ssd1306_oled_reset,
};

// allocate handler
oled_handler * ssd1306_oled_init(uint8_t address) {
  oled_handler * handler = malloc(sizeof(oled_handler));
  handler->address = address;
  return handler;
}

// reset ssd1306
// init sequence is passed through extra_data in handler
void ssd1306_oled_reset(oled_handler * oled) {
  int i;
  SSD1306_data_struct * data = oled->extra_data;
  
  for(i=0; i<data->oled_init_sequence_length; i++) {
    printf("add: %d - data: %d\n", oled->address,data->oled_init_sequence[i]);
    SSD1306_WRITE_REG(oled->address, data->oled_init_sequence[i]);
  }

  ssd1306_oled_clear(oled);
  ssd1306_set_screen_size(oled, 0, 0);
} 

void ssd1306_oled_clear(oled_handler * oled) {
  uint8_t * clean_buffer;
  uint8_t * save_buffer = oled->buffer; // save current buffer, as we need to pass a clean one
  clean_buffer = calloc(OLED_PAGE * OLED_COLUMN, sizeof(uint8_t));
  oled->buffer = clean_buffer;
  ssd1306_oled_send_buffer(oled);
  oled->buffer = save_buffer;
  free(clean_buffer);
}

void ssd1306_set_screen_size(oled_handler * oled, uint32_t columns, uint32_t pages) {
  oled->columns = columns;
  oled->pages = pages;
  oled->buffer = malloc(pages*columns);
}

void ssd1306_oled_send_buffer(oled_handler * oled) {
  int column,page;
  uint8_t * ptr = oled->buffer;
  for(page = 0; page < OLED_PAGE; page++) {
    SSD1306_WRITE_REG(oled->address, 0xb0 + page); //Set page address
    SSD1306_WRITE_REG(oled->address, 0x00);        //Set display position - column low address
    SSD1306_WRITE_REG(oled->address, 0x10);        //Set display position - column high address
    for(column = 0; column < OLED_COLUMN; column++) {
      SSD1306_WRITE_MEM(oled->address, *ptr);
      ptr++;
    }
  }
}

/** untested **/
void ssd1306_oled_send_partial_buffer(oled_handler * oled, uint32_t x, uint32_t y, uint32_t end_x, uint32_t end_y) {
  int column,page;
  uint8_t * ptr = oled->buffer;
  for(page = y; page < end_y/8; page++) {
    SSD1306_WRITE_REG(oled->address, 0xb0 + page); //Set page address
    SSD1306_WRITE_REG(oled->address, 0x00);        //Set display position - column low address
    SSD1306_WRITE_REG(oled->address, 0x10);        //Set display position - column high address
    for(column = x; column < end_x; column++) {
      SSD1306_WRITE_MEM(oled->address, *ptr);
      ptr++;
    }
  }
}

// free handler
void ssd1306_oled_close(oled_handler * oled) {
  if (oled->buffer) free(oled->buffer);
}

void ssd1306_oled_hscroll(oled_handler * oled, uint32_t page_start, uint32_t page_end, uint32_t time_interval) {
  SSD1306_WRITE_REG(oled->address, 0x2e); // end scroll
  SSD1306_WRITE_REG(oled->address, 0x27); // set scroll
  SSD1306_WRITE_REG(oled->address, 0x00); // dummy
  SSD1306_WRITE_REG(oled->address, page_start);
  SSD1306_WRITE_REG(oled->address, time_interval);
  SSD1306_WRITE_REG(oled->address, page_end);
  SSD1306_WRITE_REG(oled->address, 0x00); // dummy
  SSD1306_WRITE_REG(oled->address, 0xff); // dummy
  SSD1306_WRITE_REG(oled->address, 0x2f); // start scroll
}

void _ssd1306_write_byte(uint8_t address, uint8_t cmd, uint8_t value) {
  char wbuf[2]={cmd, value};
  wpaf_i2c_write_to_addr(address, wbuf, 2);
}
  
