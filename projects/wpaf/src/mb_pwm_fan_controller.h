#ifndef __MB_PWM_FAN_CONTROLLER_H
#define __MB_PWM_FAN_CONTROLLER_H

#include <stdint.h>
#include "fan_driver.h"

#define MB_PWM_INTERFACE "/dev/vcio"

// public interface
fan_handler * mb_pwm_fan_init();
void mb_pwm_fan_on(fan_handler *);
void mb_pwm_fan_off(fan_handler *);
void mb_pwm_fan_close(fan_handler *);
uint32_t mb_pwm_fan_capabilities();
void mb_pwm_fan_set_pwm(fan_handler * handler, uint8_t value);

extern fan_interface mb_pwm_interface;

#endif
