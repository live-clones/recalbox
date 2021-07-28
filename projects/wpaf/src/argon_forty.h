#ifndef __ARGON_FORTY_H
#define __ARGON_FORTY_H

#include "board_driver.h"

#define ARGON_FORTY_FAN_CONTROLLER_ADDR 0x1a

extern board_interface argon_forty_interface;

board_handler * argon_forty_init();
void argon_forty_close(board_handler *);

#endif //__ARGON_FORTY_H
