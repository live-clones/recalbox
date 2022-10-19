#ifndef __PIBOY_H
#define __PIBOY_H

#include "board_driver.h"

#define PIBOY_TEMP_SETPOINT 60
#define PIBOY_PWM_MAX 255
#define PIBOY_PWM_MIN -255
#define PIBOY_PWM_KP 10
#define PIBOY_PWM_KI 0.01
#define PIBOY_PWM_KD 1

extern board_interface piboy_interface;

board_handler * piboy_init();
void piboy_close(board_handler *);
uint32_t piboy_get_temp_setpoint();
int32_t piboy_query(uint32_t, void *);

#endif //__PIBOY_H

