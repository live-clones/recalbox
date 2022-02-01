/*  
 *  Based on work from Andrew Duncan
 *      The MIT License (MIT)
 * mailbox PWM driver
 * before using this driver, remove rpi_poe_fan kernel module
 * rmmod rpi_poe_fan
 */

#include <fcntl.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/ioctl.h>

#include "config.h"

#include "fan_driver.h"
#include "fonts.h"
#include "log.h"
#include "mb_pwm_fan_controller.h"
#include "wpaf_i2c.h"

fan_interface mb_pwm_interface = {
    &mb_pwm_fan_init,
    &mb_pwm_fan_on,
    &mb_pwm_fan_off,
    &mb_pwm_fan_close,
    &mb_pwm_fan_capabilities,
    &mb_pwm_fan_set_pwm,
    &fan_func_unimplemented,
};

fan_handler * mb_pwm_fan_init() {
  fan_handler * handler;
  if (!(handler = malloc(sizeof(fan_handler))))
    return NULL;

  return handler;
}

void mb_pwm_fan_on(fan_handler * handler) {
  mb_pwm_fan_set_pwm(handler, 255);
}

void mb_pwm_fan_off(fan_handler * handler) {
  mb_pwm_fan_set_pwm(handler, 0);
}

void mb_pwm_fan_close(fan_handler * handler) {
  free(handler);
}

uint32_t mb_pwm_fan_capabilities() {
  return FAN_CAP_PWM;
}

void mb_pwm_fan_set_pwm(fan_handler * handler, uint8_t speed) {
  int fd;

  uint32_t property[32] = {
      0x00000000,      // total size
      0x00000000,      // start
      0x00038049,      // tag
      0x0000000c,      // buffer size
      0x00000004,      // req response size
      0x00000000,      // pwm reg
      (uint32_t)speed, // pwm value
      0x00000000,      // return value
      0x00000000,      // end
  };

  property[0] = 9 * sizeof(property[0]);

  fd = open(MB_PWM_INTERFACE, 0);
  if (fd == -1) {
      log_error("can't open %s: %s", MB_PWM_INTERFACE, strerror(errno));
      return;
  }
  if (ioctl(fd, _IOWR(100, 0, char *), property) == -1) {
      log_error("ioctl error: %s", strerror(errno));
  }
  close(fd);
}
