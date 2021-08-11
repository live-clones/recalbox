#ifndef __RPI_PWM_FAN_CONTROLLER_H
#define __RPI_PWM_FAN_CONTROLLER_H

#ifdef HAVE_LIBBCM2835
#include <bcm2835.h>
#endif
#include <stdint.h>
#include "fan_driver.h"

typedef struct {
  uint8_t port; // pwm port
} rpi_pwm_controller_info;

struct pwm_info {
    uint8_t gpio;
    uint8_t channel;
    uint8_t alt_fun;
};

#ifdef HAVE_LIBBCM2835
static struct pwm_info pwm_io[] = {
    { 12, 0, BCM2835_GPIO_FSEL_ALT0 },
    { 13, 1, BCM2835_GPIO_FSEL_ALT0 },
    { 18, 0, BCM2835_GPIO_FSEL_ALT5 },
    { 19, 1, BCM2835_GPIO_FSEL_ALT5 }
};
#else
static struct pwm_info pwm_io[20];
#endif

static unsigned int pwm_io_map[20];

fan_handler * rpi_pwm_fan_init();
void rpi_pwm_fan_on(fan_handler *);
void rpi_pwm_fan_off(fan_handler *);
void rpi_pwm_fan_close(fan_handler *);
uint32_t rpi_pwm_fan_capabilities();
void rpi_pwm_fan_set_pwm(fan_handler *, uint8_t);

extern fan_interface rpi_pwm_fan_interface;

void rpi_pwm_fan_set_port(fan_handler * rpi_pwm_handler, uint8_t port);

#endif
