#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>

#include "config.h"

#include "fan_driver.h"
#include "fonts.h"
#include "log.h"
#include "pcf8574_fan_controller.h"
#include "wpaf_i2c.h"

/**
 * The PCF8574 device provides general-purpose
 * remote I/O expansion for most microcontroller
 * families by way of the I2C interface [serial clock
 * (SCL), serial data (SDA)].
 * The device features an 8-bit quasi-bidirectional I/O
 * port (P0–P7), including latched outputs with high-
 * current drive capability for directly driving LEDs. Each
 * quasi-bidirectional I/O can be used as an input or
 * output without the use of a data-direction control
 * signal. At power on, the I/Os are high. In this mode,
 * only a current source to VCC is active.
 */

fan_interface pcf8574_interface = {
    &pcf8574_fan_init,
    &pcf8574_fan_on,
    &pcf8574_fan_off,
    &pcf8574_fan_close,
    &pcf8574_fan_capabilities,
    &fan_func_unimplemented,
    &fan_func_unimplemented,
};

// initialize i2c
// returns file handle
fan_handler * pcf8574_fan_init(uint32_t address) {
  fan_handler * pcf8574_handler = malloc(sizeof(fan_handler));
  log_trace("pcf8574_fan_init()");
  pcf8574_handler->address = address;
  pcf8574_handler->controller_info = calloc(1, sizeof(pcf8574_controller_info));
  return pcf8574_handler;
}

// enable fan
void pcf8574_fan_on(fan_handler * pcf8574_handler) {
  log_trace("pcf8574_fan_on()");
  char x = wpaf_i2c_read1_from_addr(pcf8574_handler->address);
  char ioport = ((pcf8574_controller_info*)pcf8574_handler->controller_info)->ioport;
  x = (!(char)pow(2, ioport)) & x;
  wpaf_i2c_write1_to_addr(pcf8574_handler->address, x);
}
// disable fan
void pcf8574_fan_off(fan_handler * pcf8574_handler) {
  log_trace("pcf8574_fan_off()");
  char x = wpaf_i2c_read1_from_addr(pcf8574_handler->address);
  char ioport = ((pcf8574_controller_info*)pcf8574_handler->controller_info)->ioport;
  x = ((char)pow(2, ioport)) | x;
  wpaf_i2c_write1_to_addr(pcf8574_handler->address, x);
}

void pcf8574_fan_close(fan_handler * pcf8574_handler) {
  log_trace("pcf8574_fan_close()");
  free(pcf8574_handler);
}

uint32_t pcf8574_fan_capabilities() {
  log_trace("pcf8574_fan_capabilities()");
  return FAN_CAP_SIMPLE;
}

bool pcf8574_fan_set_ioport(fan_handler * pcf8574_handler, uint8_t ioport) {
  if (ioport > 7)
      return false;
  ((pcf8574_controller_info*)pcf8574_handler->controller_info)->ioport = ioport;
  return true;
}

