#ifndef __RPI_POE_PLUS_H
#define __RPI_POE_PLUS_H

#include "board_driver.h"

#define RPI_POE_PLUS_TEMP_SETPOINT 51
#define RPI_POE_PLUS_PWM_MIN -255
#define RPI_POE_PLUS_PWM_MAX 255
#define RPI_POE_PLUS_PWM_KP  3
#define RPI_POE_PLUS_PWM_KI  0.8
#define RPI_POE_PLUS_PWM_KD  0.1

extern board_interface rpi_poe_plus_interface;

board_handler * rpi_poe_plus_init();
void rpi_poe_plus_close(board_handler *);
uint32_t rpi_poe_plus_get_temp_setpoint();
int32_t rpi_poe_plus_query(uint32_t queryid, void*);

#endif //__RPI_POE_PLUS_H
