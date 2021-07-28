#ifndef __DISPLAY_DRIVER_H
#define __DISPLAY_DRIVER_H

#include <stdio.h>
#include <stdint.h>

typedef struct {
  uint8_t address;
  uint8_t * buffer;
  uint32_t pages;
  uint32_t columns;
  void * extra_data;
} display_handler;

typedef struct {
  display_handler * (*init)(uint8_t);
  void (*close)(display_handler *);
  void (*clear)(display_handler *);
  void (*send_buffer)(display_handler *);
  void (*send_partial_buffer)(display_handler *, uint32_t, uint32_t, uint32_t, uint32_t); ;
  void (*set_screen_size)(display_handler *, uint32_t, uint32_t);
  void (*horizontal_scroll)(display_handler *, uint32_t, uint32_t, uint32_t);
  void (*reset)(display_handler *);
} display_interface ;

#define SSD1306_CONTROLLER 0

#endif // __DISPLAY_DRIVER_H
