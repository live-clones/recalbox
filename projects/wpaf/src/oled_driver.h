#ifndef __OLED_DRIVER_H
#define __OLED_DRIVER_H

#include <stdio.h>
#include <stdint.h>

typedef struct {
  uint8_t address;
  uint8_t * buffer;
  uint32_t pages;
  uint32_t columns;
  void * extra_data;
} oled_handler;

typedef struct {
  oled_handler * (*init)(uint8_t);
  void (*close)(oled_handler *);
  void (*clear)(oled_handler *);
  void (*send_buffer)(oled_handler *);
  void (*send_partial_buffer)(oled_handler *, uint32_t, uint32_t, uint32_t, uint32_t); ;
  void (*set_screen_size)(oled_handler *, uint32_t, uint32_t);
  void (*horizontal_scroll)(oled_handler *, uint32_t, uint32_t, uint32_t);
  void (*reset)(oled_handler *);
} oled_interface ;

#define SSD1306_CONTROLLER 0

#endif // __OLED_DRIVER_H
