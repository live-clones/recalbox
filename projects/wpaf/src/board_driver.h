#ifndef __BOARD_DRIVER_H
#define __BOARD_DRIVER_H

#include <stdio.h>
#include <stdint.h>

#include "oled_driver.h"
#include "fan_driver.h"

typedef struct {
  oled_interface * oled;
  fan_interface * fan;
  oled_handler * o_handler;
  fan_handler * f_handler;
} board_handler;

typedef struct {
  board_handler * (*init)(void);
  void (*close)(board_handler *);
} board_interface ;

#endif // __BOARD_DRIVER_H
