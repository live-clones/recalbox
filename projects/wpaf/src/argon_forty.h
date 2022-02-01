#ifndef __ARGON_FORTY_H
#define __ARGON_FORTY_H

#include "board_driver.h"

#define ARGON_FORTY_FAN_CONTROLLER_ADDR 0x1a

#define ARGON_FORTY_TEMP_SETPOINT 53
#define ARGON_FORTY_PWM_MIN -3
#define ARGON_FORTY_PWM_MAX 3
#define ARGON_FORTY_PWM_KP  0.01
#define ARGON_FORTY_PWM_KI  0.001
#define ARGON_FORTY_PWM_KD  0.01

extern board_interface argon_forty_interface;

board_handler * argon_forty_init();
void argon_forty_close(board_handler *);
uint32_t argon_forty_get_temp_setpoint();
int32_t argon_forty_query(uint32_t queryid, void*);

#endif //__ARGON_FORTY_H
