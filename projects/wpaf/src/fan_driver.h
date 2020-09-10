#ifndef __FAN_DRIVER_H
#define __FAN_DRIVER_H

#include <stdio.h>
#include <stdint.h>

typedef struct {
  uint32_t fd;
} fan_handler;

typedef struct {
  fan_handler * (*init)(uint32_t address);
  void (*on)(fan_handler *);
  void (*off)(fan_handler *);
} fan_interface ;

#define PCF8574_CONTROLLER 0

#endif //__FAN_DRIVER_H
