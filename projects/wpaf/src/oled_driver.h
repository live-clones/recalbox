#ifndef __OLED_DRIVER_H
#define __OLED_DRIVER_H

#include <stdio.h>
#include <stdint.h>

typedef struct {
  uint32_t fd;
  uint8_t * buffer;
  uint32_t pages;
  uint32_t columns;
} oled_handler;

typedef struct {
  oled_handler * (*init)(uint32_t address);
  void (*close)(oled_handler *);
  void (*clear)(oled_handler *);
  void (*send_buffer)(oled_handler *);
  void (*set_screen_size)(oled_handler *, uint32_t, uint32_t);
} oled_interface ;

#define SSD1306_CONTROLLER 0

#endif // __OLED_DRIVER_H
