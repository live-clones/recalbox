#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>

#include "config.h"

#include "fonts.h"

#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "pcf8574_fan_controller.h"

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
  pcf8574_handler->fd = wiringPiI2CSetup(address) ;
  return pcf8574_handler;
}

// enable fan
void pcf8574_fan_on(fan_handler * pcf8574_handler) {
  wiringPiI2CWrite(pcf8574_handler->fd, 0xfe | wiringPiI2CRead(pcf8574_handler->fd));
}
// disable fan
void pcf8574_fan_off(fan_handler * pcf8574_handler) {
  wiringPiI2CWrite(pcf8574_handler->fd, 0x01 | wiringPiI2CRead(pcf8574_handler->fd));
}

void pcf8574_fan_close(fan_handler * pcf8574_handler) {
  close(pcf8574_handler->fd);
  free(pcf8574_handler);
}
