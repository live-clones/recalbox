#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>

#include "config.h"

#include "log.h"
#include "xpi_gamecon_fan_controller.h"

fan_interface xpi_gamecon_fan_interface = {
    &xpi_gamecon_fan_init,
    &xpi_gamecon_fan_on,
    &xpi_gamecon_fan_off,
    &xpi_gamecon_fan_close,
    &xpi_gamecon_fan_capabilities,
    &xpi_gamecon_fan_set_pwm,
};

// initialize i2c
// returns file handle
fan_handler * xpi_gamecon_fan_init() {
  fan_handler * xpi_gamecon_handler = malloc(sizeof(fan_handler));
  return xpi_gamecon_handler;
}

// enable fan
void xpi_gamecon_fan_on(fan_handler * xpi_gamecon_handler) {
  __xpi_gamecon_set_fan_speed(xpi_gamecon_handler, 100);
}
// disable fan
void xpi_gamecon_fan_off(fan_handler * xpi_gamecon_handler) {
  __xpi_gamecon_set_fan_speed(xpi_gamecon_handler, 0);
}

void xpi_gamecon_fan_close(fan_handler * xpi_gamecon_handler) {
  free(xpi_gamecon_handler);
}

uint32_t xpi_gamecon_fan_capabilities() {
  return FAN_CAP_PWM;
}

void xpi_gamecon_fan_set_pwm(fan_handler * handler, uint8_t value) {
  __xpi_gamecon_set_fan_speed(handler, value);
}

void __xpi_gamecon_set_fan_speed(fan_handler * xpi_gamecon_handler, uint8_t speed) {
  FILE * fan_fd;
  
  if (speed > 100)
    speed = 100;
  
  log_debug("Setting xpi fan to %d", speed);
  fan_fd = fopen(XPI_GAMECON_FAN_FILE, "w");
  if (fan_fd) {
    fprintf(fan_fd, "%d", speed);
    fclose(fan_fd);
  }else {
    log_error("Can't write to xpi fan file");
  }
}
