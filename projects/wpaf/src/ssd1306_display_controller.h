#ifndef __SSD1306_OLED_CONTROLLER_H
#define __SSD1306_OLED_CONTROLLER_H

#include <stdint.h>

#include "display_driver.h"
#include "wpaf_i2c.h"

// defines screen height and length
#define OLED_WIDTH   128
#define OLED_HEIGHT  32
#define OLED_COLUMN  OLED_WIDTH
#define OLED_PAGE    OLED_HEIGHT / 8

#define SSD1306_IIC_CMD 0x00
#define SSD1306_IIC_RAM 0x40

#define SSD1306_WRITE_REG(Address, Reg)  _ssd1306_write_byte(Address, SSD1306_IIC_CMD, Reg);
#define SSD1306_WRITE_MEM(Address, Data) _ssd1306_write_byte(Address, SSD1306_IIC_RAM, Data);

typedef struct ssd1306_extra_data_struct {
  int display_init_sequence[256];
  int display_init_sequence_length;
  uint8_t display_address;
} SSD1306_data_struct;

display_handler * ssd1306_display_init(uint8_t);
void ssd1306_display_reset(display_handler *);
void ssd1306_display_clear(display_handler *);
void ssd1306_display_send_buffer(display_handler *);
void ssd1306_display_send_partial_buffer(display_handler *, uint32_t, uint32_t, uint32_t, uint32_t);
void ssd1306_set_screen_size(display_handler *, uint32_t, uint32_t);
void ssd1306_display_close(display_handler *);
void ssd1306_display_hscroll(display_handler *, uint32_t, uint32_t, uint32_t );

void _ssd1306_write_byte(uint8_t address, uint8_t Cmd, uint8_t value);

extern display_interface ssd1306_interface;

#endif //__SSD1306_OLED_CONTROLLER_H
