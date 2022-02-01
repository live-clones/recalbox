#ifndef __FAN_DRIVER_H
#define __FAN_DRIVER_H

#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

typedef struct {
  uint8_t address;
  void * controller_info;
} fan_handler;

typedef struct {
  fan_handler * (*init)(uint32_t address);
  void (*on)(fan_handler *); // for FAN_CAP_SIMPLE
  void (*off)(fan_handler *);// for FAN_CAP_SIMPLE
  void (*close)(fan_handler *);
  uint32_t (*fan_capabilities)(); // see FAN_CAP_
  void (*set_pwm)(fan_handler *, uint8_t); // for FAN_CAP_PWM
  uint32_t (*status)(fan_handler *); // for FAN_CAP_STATUS
} fan_interface ;

void * fan_func_unimplemented(void);

#define FAN_CAP_SIMPLE 0x00000001 // on/off, usually gpio controlled
#define FAN_CAP_PWM    0x00000002 // variable fan speed
#define FAN_CAP_STATUS 0x00000004 // give back fan status

#endif //__FAN_DRIVER_H
