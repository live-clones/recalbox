#ifndef __DISPLAY_MANAGER_H
#define __DISPLAY_MANAGER_H

#include <stdint.h>

#include "board_driver.h"

uint32_t start_display_manager(display_interface * display, display_handler * handler) ;
void manage_display(display_interface * display, display_handler * handler) ;

#endif // __DISPLAY_MANAGER_H
