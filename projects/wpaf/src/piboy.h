#ifndef __PIBOY_H
#define __PIBOY_H

#include "board_driver.h"

#define PIBOY_TEMP_SETPOINT 60

extern board_interface piboy_interface;

board_handler * piboy_init();
void piboy_close(board_handler *);
uint32_t piboy_get_temp_setpoint();

#endif //__PIBOY_H
