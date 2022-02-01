#ifndef __FAN_SHIM_H
#define __FAN_SHIM_H

#include "board_driver.h"

#define FAN_SHIM_TEMP_SETPOINT 50
#define FAN_SHIM_PWM_MAX 100
#define FAN_SHIM_PWM_MIN -100
#define FAN_SHIM_PWM_KP 5
#define FAN_SHIM_PWM_KI 0.5
#define FAN_SHIM_PWM_KD 0.5

extern board_interface fan_shim_interface;

board_handler * fan_shim_init();
void fan_shim_close(board_handler *);
uint32_t fan_shim_get_temp_setpoint();
int32_t fan_shim_query(uint32_t, void *);

#endif //__FAN_SHIM_H

