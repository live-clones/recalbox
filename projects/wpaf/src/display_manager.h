#ifndef __DISPLAY_MANAGER_H
#define __DISPLAY_MANAGER_H

#include <stdint.h>

#include "board_driver.h"
#include "wpaf.h"

uint32_t start_display_manager(board_interface * interface, board_handler * handler, __ARGS *);
void manage_display(display_interface * display, display_handler * handler, __ARGS *) ;

#endif // __DISPLAY_MANAGER_H
