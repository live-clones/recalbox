#include <stdlib.h>

#include "config.h"

#include "piboy.h"
#include "xpi_gamecon_fan_controller.h"
#include "board_driver.h"
#include "fan_driver.h"
#include "null_display_controller.h"

board_interface piboy_interface = {
  &piboy_init,
  &piboy_close
};

board_handler * piboy_init() {
  board_handler * board = malloc(sizeof(board_handler));
  board->fan = &xpi_gamecon_fan_interface;
  board->display = &null_display_interface;

  board->f_handler = board->fan->init(NULL);
  board->o_handler = board->display->init(NULL);

  return board;
}

void piboy_close(board_handler * board) {
  board->fan->close(board->f_handler);
  board->display->close(board->o_handler);
}
