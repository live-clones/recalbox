#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "config.h"
#include "null_display_controller.h"
#include "wpaf_i2c.h"

display_interface null_display_interface = {
  &null_display_init,
  &null_display_close,
  &null_display_null,
  &null_display_null,
  &null_display_null,
  &null_display_null,
  &null_display_null,
  &null_display_null,
};

// allocate handler
display_handler * null_display_init() {
  display_handler * handler = calloc(1, sizeof(display_handler));
  return handler;
}

void null_display_null() {

} 

// free handler
void null_display_close(display_handler * display) {
  free(display);
}
  
