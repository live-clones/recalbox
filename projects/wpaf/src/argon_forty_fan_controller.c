#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>

#include "config.h"

#include "argon_forty_fan_controller.h"
#include "wpaf_i2c.h"

fan_interface argon_forty_fan_interface = {
    &argon_forty_fan_init,
    &argon_forty_fan_on,
    &argon_forty_fan_off,
    &argon_forty_fan_close,
    &argon_forty_fan_capabilities,
    &argon_forty_fan_set_pwm,
};

// initialize i2c
// returns file handle
fan_handler * argon_forty_fan_init(uint32_t address) {
  fan_handler * handler = malloc(sizeof(fan_handler));
  handler->address = address;
  return handler;
}

// enable fan
void argon_forty_fan_on(fan_handler * handler) {
  wpaf_i2c_write1_to_addr(handler->address, 3);
}
// disable fan
void argon_forty_fan_off(fan_handler * handler) {
  wpaf_i2c_write1_to_addr(handler->address, 0);
}

void argon_forty_fan_close(fan_handler * handler) {
  free(handler);
}

uint32_t argon_forty_fan_capabilities() {
  return FAN_CAP_PWM;
}

void argon_forty_fan_set_pwm(fan_handler * handler, uint8_t value) {
  wpaf_i2c_write1_to_addr(handler->address, value);
}

