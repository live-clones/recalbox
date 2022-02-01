#ifndef __PCF8574_FAN_CONTROLLER_H
#define __PCF8574_FAN_CONTROLLER_H

#include <stdint.h>
#include "fan_driver.h"

typedef struct {
  uint8_t ioport; // port number where the fan is connected to, 0 to 7
} pcf8574_controller_info;

// public interface
fan_handler * pcf8574_fan_init(uint32_t address);
void pcf8574_fan_on(fan_handler *);
void pcf8574_fan_off(fan_handler *);
void pcf8574_fan_close(fan_handler *);
uint32_t pcf8574_fan_capabilities();
void* pcf8574_unimplemented(void);

// private interface
bool pcf8574_fan_set_ioport(fan_handler * pcf8574_handler, uint8_t ioport);

extern fan_interface pcf8574_interface;

#endif
