#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>

#include "config.h"

#include "fonts.h"
#include "log.h"
#include "pcf8574_fan_controller.h"
#include "wpaf_i2c.h"

fan_interface pcf8574_interface = {
    &pcf8574_fan_init,
    &pcf8574_fan_on,
    &pcf8574_fan_off,
    &pcf8574_fan_close,
    &pcf8574_fan_capabilities,
    &pcf8574_unimplemented,
    &pcf8574_unimplemented,
};

// initialize i2c
// returns file handle
fan_handler * pcf8574_fan_init(uint32_t address) {
  fan_handler * pcf8574_handler = malloc(sizeof(fan_handler));
  log_trace("pcf8574_fan_init()");
  pcf8574_handler->address = address;
  return pcf8574_handler;
}

// enable fan
void pcf8574_fan_on(fan_handler * pcf8574_handler) {
  log_trace("pcf8574_fan_on()");
  char x = wpaf_i2c_read1_from_addr(pcf8574_handler->address);
  x = 0xfe & x;
  wpaf_i2c_write1_to_addr(pcf8574_handler->address, x);
}
// disable fan
void pcf8574_fan_off(fan_handler * pcf8574_handler) {
  log_trace("pcf8574_fan_off()");
  char x = wpaf_i2c_read1_from_addr(pcf8574_handler->address);
  x = 0x01 | x;
  wpaf_i2c_write1_to_addr(pcf8574_handler->address, x);
}

void pcf8574_fan_close(fan_handler * pcf8574_handler) {
  log_trace("pcf8574_fan_close()");
  free(pcf8574_handler);
}

void* pcf8574_unimplemented(void) {
  return NULL;
}

uint32_t pcf8574_fan_capabilities() {
  log_trace("pcf8574_fan_capabilities()");
  return FAN_CAP_SIMPLE;
}
