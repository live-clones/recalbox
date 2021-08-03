#ifndef __PCF8574_FAN_CONTROLLER_H
#define __PCF8574_FAN_CONTROLLER_H

#include <stdint.h>
#include "fan_driver.h"

fan_handler * pcf8574_fan_init(uint32_t address);
void pcf8574_fan_on(fan_handler *);
void pcf8574_fan_off(fan_handler *);
void pcf8574_fan_close(fan_handler *);
uint32_t pcf8574_fan_capabilities();

extern fan_interface pcf8574_interface;

void* pcf8574_unimplemented(void);
#endif
