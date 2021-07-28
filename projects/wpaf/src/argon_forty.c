#include <stdlib.h>

#include "config.h"

#include "argon_forty.h"
#include "argon_forty_fan_controller.h"
#include "board_driver.h"
#include "fan_driver.h"
#include "null_display_controller.h"

board_interface argon_forty_interface = {
  &argon_forty_init,
  &argon_forty_close
};

board_handler * argon_forty_init() {
  board_handler * board = malloc(sizeof(board_handler));
  board->fan = &argon_forty_fan_interface;
  board->display = &null_display_interface;

  board->f_handler = board->fan->init(ARGON_FORTY_FAN_CONTROLLER_ADDR);
  board->o_handler = board->display->init(NULL);

  return board;
}

void argon_forty_close(board_handler * board) {
  board->fan->close(board->f_handler);
  board->display->close(board->o_handler);
}
