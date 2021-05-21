#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>

#include "config.h"

#include "fonts.h"
#include "pcf8574_fan_controller.h"
#include "wpaf_i2c.h"

fan_interface pcf8574_interface = {
    &pcf8574_fan_init,
    &pcf8574_fan_on,
    &pcf8574_fan_off,
    &pcf8574_fan_close,
};

// initialize i2c
// returns file handle
fan_handler * pcf8574_fan_init(uint32_t address) {
  fan_handler * pcf8574_handler = malloc(sizeof(fan_handler));
  pcf8574_handler->address = address;
  return pcf8574_handler;
}

// enable fan
void pcf8574_fan_on(fan_handler * pcf8574_handler) {
  wpaf_i2c_write1_to_addr(pcf8574_handler->address, (char)0xfe | wpaf_i2c_read1_from_addr(pcf8574_handler->address));
}
// disable fan
void pcf8574_fan_off(fan_handler * pcf8574_handler) {
  wpaf_i2c_write1_to_addr(pcf8574_handler->address, (char)0x01 | wpaf_i2c_read1_from_addr(pcf8574_handler->address));
}

void pcf8574_fan_close(fan_handler * pcf8574_handler) {
  free(pcf8574_handler);
}
