#ifndef __NULL_DISPLAY_CONTROLLER_H
#define __NULL_DISPLAY_CONTROLLER_H

#include <stdint.h>

#include "display_driver.h"
#include "wpaf_i2c.h"


display_handler * null_display_init();
void null_display_close(display_handler *);
void null_display_null();

extern display_interface null_display_interface;

#endif //__NULL_DISPLAY_CONTROLLER_H
