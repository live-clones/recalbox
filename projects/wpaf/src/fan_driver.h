#ifndef __FAN_DRIVER_H
#define __FAN_DRIVER_H

#include <stdio.h>
#include <stdint.h>

typedef struct {
  uint8_t address;
} fan_handler;

typedef struct {
  fan_handler * (*init)(uint32_t address);
  void (*on)(fan_handler *);
  void (*off)(fan_handler *);
  void (*close)(fan_handler *);
  uint32_t (*fan_capabilities)(); // see FAN_CAP_
  void (*set_pwm)(fan_handler *, uint8_t);
} fan_interface ;

#define FAN_CAP_SIMPLE 0x00000001 // on/off, usually gpio controlled
#define FAN_CAP_PWM    0x00000002 // variable fan speed

#endif //__FAN_DRIVER_H
