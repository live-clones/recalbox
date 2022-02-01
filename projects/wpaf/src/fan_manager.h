#ifndef __FAN_MANAGER_H
#define __FAN_MANAGER_H

#include <stdbool.h>
#include <stdint.h>

#include "board_driver.h"
#include "fan_driver.h"
#include "wpaf.h"

void start_fan_manager(board_interface *, board_handler *, __ARGS *);
void manage_simple_fan(board_interface *, board_handler *, __ARGS *);
void manage_pwm_fan(board_interface *, board_handler *, __ARGS *);

#endif // __FAN_MANAGER_H
