#ifndef __SSD1306_OLED_CONTROLLER_H
#define __SSD1306_OLED_CONTROLLER_H

#include <stdint.h>

#include "oled_driver.h"
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
  int oled_init_sequence[256];
  int oled_init_sequence_length;
  uint8_t oled_address;
} SSD1306_data_struct;

oled_handler * ssd1306_oled_init(uint8_t);
void ssd1306_oled_reset(oled_handler *);
void ssd1306_oled_clear(oled_handler *);
void ssd1306_oled_send_buffer(oled_handler *);
void ssd1306_oled_send_partial_buffer(oled_handler *, uint32_t, uint32_t, uint32_t, uint32_t);
void ssd1306_set_screen_size(oled_handler *, uint32_t, uint32_t);
void ssd1306_oled_close(oled_handler *);
void ssd1306_oled_hscroll(oled_handler *, uint32_t, uint32_t, uint32_t );

void _ssd1306_write_byte(uint8_t address, uint8_t Cmd, uint8_t value);

extern oled_interface ssd1306_interface;

#endif //__SSD1306_OLED_CONTROLLER_H
