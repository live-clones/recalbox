#ifndef __ARGON_FORTY_H
#define __ARGON_FORTY_H

#include "board_driver.h"

#define ARGON_FORTY_FAN_CONTROLLER_ADDR 0x1a

#define ARGON_FORTY_TEMP_SETPOINT 53

extern board_interface argon_forty_interface;

board_handler * argon_forty_init();
void argon_forty_close(board_handler *);
uint32_t argon_forty_get_temp_setpoint();

#endif //__ARGON_FORTY_H
