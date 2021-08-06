#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "config.h"

#include "board_driver.h"
#include "cpu_controller.h"
#include "display_manager.h"
#include "gui.h"
#include "log.h"
#include "wpaf.h"

uint32_t start_display_manager(board_interface * interface, board_handler * handler, __ARGS * arguments) {
  manage_display(handler->display, handler->o_handler, arguments);
  return 1;
}

void manage_display(display_interface * display, display_handler * handler, __ARGS * arguments) {
  char string_buffer[256];
  char recalbox_version[50];
  char recalbox_string[256];

  /*
   * get recalbox version from fs
   */
  FILE * fd = fopen("/recalbox/recalbox.version", "r");
  if (fd)
    fgets(recalbox_version, sizeof(recalbox_version), fd);
  fclose(fd);
  snprintf(recalbox_string, sizeof(recalbox_string), "Recalbox %s", recalbox_version);

  gui_init(display, handler, handler->columns, handler->pages);

  while(true) {
    float cputemp = get_cpu_temp();
    snprintf(string_buffer, sizeof(string_buffer), "CPU temp: %.2f", cputemp);
    gui_clear(display, handler);
    gui_string(handler, 0, 0, string_buffer, &Font12, BLACK, WHITE);
    gui_string(handler, 0, 16, recalbox_string, &Font12, BLACK, WHITE);
    gui_draw(display, handler);
    sleep(1);
  }
}

