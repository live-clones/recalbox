#ifndef __PIBOY_H
#define __PIBOY_H

#include "board_driver.h"

extern board_interface piboy_interface;

board_handler * piboy_init();
void piboy_close(board_handler *);

#endif //__PIBOY_H
