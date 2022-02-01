#ifdef HAVE_LIBBCM2835
#include <bcm2835.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>

#include "config.h"

#include "log.h"
#include "rpi_pwm_fan_controller.h"

#define PWM_RANGE 100

fan_interface rpi_pwm_fan_interface = {
    &rpi_pwm_fan_init,
    &rpi_pwm_fan_on,
    &rpi_pwm_fan_off,
    &rpi_pwm_fan_close,
    &rpi_pwm_fan_capabilities,
    &rpi_pwm_fan_set_pwm,
};

// initialize pwm
// returns fan handler
fan_handler * rpi_pwm_fan_init() {
  fan_handler * rpi_pwm_handler;
 
  log_trace("rpi_pwm_fan_init()");
  if (!(rpi_pwm_handler = malloc(sizeof(fan_handler))))
    return NULL;

  if (!(rpi_pwm_handler->controller_info = calloc(1, sizeof(rpi_pwm_controller_info)))) {
    free(rpi_pwm_handler);
    return NULL;
  }

  // map pwm port ID with our port
  for (int i = 0; i < 20; i++)
    pwm_io_map[i] = -1;
  pwm_io_map[12] = 0;
  pwm_io_map[13] = 1;
  pwm_io_map[18] = 2;
  pwm_io_map[19] = 3;

#ifdef HAVE_LIBBCM2835
  if (!bcm2835_init()) {
    log_error("can't initialize bcm2835");
    free(rpi_pwm_handler->controller_info);
    free(rpi_pwm_handler);
    return NULL;
  }
#else
  log_error("no libbcm2835 on this system");
#endif

  return rpi_pwm_handler;
}

// 
void rpi_pwm_fan_set_port(fan_handler * rpi_pwm_handler, uint8_t port) {
  int index = pwm_io_map[port];
  struct pwm_info *info = &pwm_io[index];
  
  log_trace("rpi_pwm_fan_set_port(port=%d, channel=%d, func=%d)", port, info->channel, info->alt_fun);
  ((rpi_pwm_controller_info*)rpi_pwm_handler->controller_info)->port = port;
#ifdef HAVE_LIBBCM2835
  bcm2835_gpio_fsel(port, info->alt_fun);
  bcm2835_pwm_set_clock(1);
  bcm2835_pwm_set_mode(info->channel, true, true);
  bcm2835_pwm_set_range(info->channel, PWM_RANGE); // 100 steps
#endif
}

// enable fan
void rpi_pwm_fan_on(fan_handler * rpi_pwm_handler) {
  rpi_pwm_fan_set_pwm(rpi_pwm_handler, 100);
}
// disable fan
void rpi_pwm_fan_off(fan_handler * rpi_pwm_handler) {
  rpi_pwm_fan_set_pwm(rpi_pwm_handler, 0);
}

void rpi_pwm_fan_close(fan_handler * rpi_pwm_handler) {
  free(rpi_pwm_handler->controller_info);
  free(rpi_pwm_handler);
#ifdef HAVE_LIBBCM2835
  bcm2835_close();
#endif
}

uint32_t rpi_pwm_fan_capabilities() {
  return FAN_CAP_PWM;
}

void rpi_pwm_fan_set_pwm(fan_handler * handler, uint8_t speed) {
  uint8_t port = ((rpi_pwm_controller_info*)handler->controller_info)->port;
  int index = pwm_io_map[port];
  struct pwm_info *info = &pwm_io[index];
  log_trace("rpi_pwm_fan_set_pwm(channelr=%d, port=%d, speed=%d)", info->channel, port, PWM_RANGE*speed/100);
  
#ifdef HAVE_LIBBCM2835
  bcm2835_pwm_set_data(info->channel, PWM_RANGE*speed/100);
#endif
}

