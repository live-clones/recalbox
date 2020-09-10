#ifndef __PCF8574_FAN_CONTROLLER_H
#define __PCF8574_FAN_CONTROLLER_H

#include <stdint.h>
#include "fan_driver.h"

fan_handler * pcf8574_fan_init(uint32_t address);
void pcf8574_fan_on(fan_handler *);
void pcf8574_fan_off(fan_handler *);

extern fan_interface pcf8574_interface;

#endif
