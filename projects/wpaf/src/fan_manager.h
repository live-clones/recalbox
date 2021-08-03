#ifndef __FAN_MANAGER_H
#define __FAN_MANAGER_H

#include <stdbool.h>
#include <stdint.h>

#include "fan_driver.h"

void start_fan_manager(fan_interface *, fan_handler *);
void manage_simple_fan(fan_interface * fan, fan_handler * handler);
void manage_pwm_fan(fan_interface * fan, fan_handler * handler);

#endif // __FAN_MANAGER_H
