#ifndef __BOARD_DRIVER_H
#define __BOARD_DRIVER_H

#include <stdio.h>
#include <stdint.h>

#include "display_driver.h"
#include "fan_driver.h"

typedef struct {
  display_interface * display;
  fan_interface * fan;
  display_handler * o_handler;
  fan_handler * f_handler;
} board_handler;

typedef struct {
  board_handler * (*init)(void);
  void (*close)(board_handler *);
  uint32_t (*get_temp_setpoint)(void);
} board_interface ;

#endif // __BOARD_DRIVER_H
