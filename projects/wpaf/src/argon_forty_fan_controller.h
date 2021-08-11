#ifndef __ARGON_FORTY_FAN_CONTROLLER_H
#define __ARGON_FORTY_FAN_CONTROLLER_H

#include <stdint.h>
#include "fan_driver.h"

fan_handler * argon_forty_fan_init(uint32_t address);
void argon_forty_fan_on(fan_handler *);
void argon_forty_fan_off(fan_handler *);
void argon_forty_fan_close(fan_handler *);
uint32_t argon_forty_fan_capabilities();
void argon_forty_fan_set_pwm(fan_handler *, uint8_t);

extern fan_interface argon_forty_fan_interface;

#endif
