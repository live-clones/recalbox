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
  int32_t (*query)(uint32_t, void*);
} board_interface ;

// queries
enum {
  BOARD_QUERY_TEMP_SETPOINT,
  BOARD_QUERY_PWM_MIN,
  BOARD_QUERY_PWM_MAX,
  BOARD_QUERY_PWM_KP,
  BOARD_QUERY_PWM_KI,
  BOARD_QUERY_PWM_KD,
};
#define BOARD_QUERY_UNIMPLEMENTED -1

#endif // __BOARD_DRIVER_H
