#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "config.h"
#include "ssd1306_oled_controller.h"
#include "oled_i2c_driver.h"

oled_interface ssd1306_interface = {
  &ssd1306_oled_init,
  &ssd1306_oled_close,
  &ssd1306_oled_clear,
  &ssd1306_oled_send_buffer,
  &ssd1306_oled_send_partial_buffer,
  &ssd1306_set_screen_size,
  &ssd1306_oled_hscroll
};

oled_handler * ssd1306_oled_init(uint32_t oled_address) {
  oled_handler * handler = malloc(sizeof(oled_handler));
  int i;
  int init_sequence[24]= {
    0xAE,
    0x2E,// end scroll
    0x40,//---set low column address
    0xB0,//---set high column address
    0xC8,//-not offset
    0x81,
    0xff,
    0xa1,
    0xa6,
    0xa8,
    0x1f,
    0xd3,
    0x00,
    0xd5,
    0xf0,
    0xd9,
    0x22,
    0xda,
    0x02,
    0xdb,
    0x49,
    0x8d,
    0x14,
    0xaf
  };

  handler->fd = wiringPiI2CSetup(oled_address);
  
  for(i=0; i<24; i++) {
    oled_write_register(handler->fd, init_sequence[i]);
  }

  ssd1306_oled_clear(handler);
  ssd1306_set_screen_size(handler, 0, 0);
  return handler;
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
    oled_write_register(oled->fd, 0xb0 + page); //Set page address
    oled_write_register(oled->fd, 0x00);        //Set display position - column low address
    oled_write_register(oled->fd, 0x10);        //Set display position - column high address
    for(column = 0; column < OLED_COLUMN; column++) {
      oled_write_memory(oled->fd, *ptr);
      ptr++;
    }
  }
}

/** untested **/
void ssd1306_oled_send_partial_buffer(oled_handler * oled, uint32_t x, uint32_t y, uint32_t end_x, uint32_t end_y) {
  int column,page;
  uint8_t * ptr = oled->buffer;
  for(page = y; page < end_y/8; page++) {
    oled_write_register(oled->fd, 0xb0 + page); //Set page address
    oled_write_register(oled->fd, 0x00);        //Set display position - column low address
    oled_write_register(oled->fd, 0x10);        //Set display position - column high address
    for(column = x; column < end_x; column++) {
      oled_write_memory(oled->fd, *ptr);
      ptr++;
    }
  }
}
void ssd1306_oled_close(oled_handler * oled) {
  if (oled->buffer) free(oled->buffer);
  if (oled->fd) close(oled->fd);
}

void ssd1306_oled_hscroll(oled_handler * oled, uint32_t page_start, uint32_t page_end, uint32_t time_interval) {
  oled_write_register(oled->fd, 0x2e); // end scroll
  oled_write_register(oled->fd, 0x27); // set scroll
  oled_write_register(oled->fd, 0x00); // dummy
  oled_write_register(oled->fd, page_start);
  oled_write_register(oled->fd, time_interval);
  oled_write_register(oled->fd, page_end);
  oled_write_register(oled->fd, 0x00); // dummy
  oled_write_register(oled->fd, 0xff); // dummy
  oled_write_register(oled->fd, 0x2f); // start scroll
}

