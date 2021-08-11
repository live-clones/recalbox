#ifndef __XPI_GAMECON_FAN_CONTROLLER_H
#define __XPI_GAMECON_FAN_CONTROLLER_H

#include <stdint.h>
#include "fan_driver.h"

#define XPI_GAMECON_FAN_FILE "/sys/kernel/xpi_gamecon/fan"

fan_handler * xpi_gamecon_fan_init();
void xpi_gamecon_fan_on(fan_handler *);
void xpi_gamecon_fan_off(fan_handler *);
void xpi_gamecon_fan_close(fan_handler *);
uint32_t xpi_gamecon_fan_capabilities();
void xpi_gamecon_fan_set_pwm(fan_handler *, uint8_t);

extern fan_interface xpi_gamecon_fan_interface;

void __xpi_gamecon_set_fan_speed(fan_handler * xpi_gamecon_handler, uint8_t speed);

#endif
