#include <stdlib.h>

#include "config.h"

#include "board_driver.h"
#include "oled_driver.h"
#include "fan_driver.h"
#include "ssd1306_oled_controller.h"
#include "pcf8574_fan_controller.h"
#include "waveshare_poehatb.h"

board_interface waveshare_poehatb_interface = {
  &wspoehatb_init,
  &wspoehatb_close
};

board_handler * wspoehatb_init() {
  board_handler * board = malloc(sizeof(board_handler));
  board->oled = &ssd1306_interface;
  board->fan  = &pcf8574_interface;

  board->o_handler = board->oled->init(WSPOEHATB_OLED_ADDRESS);
  board->f_handler = board->fan->init(WSPOEHATB_FAN_ADDRESS);

  board->oled->set_screen_size(board->o_handler, WSPOEHATB_OLED_X, WSPOEHATB_OLED_Y);

  return board;
}

void wspoehatb_close(board_handler * board) {
  board->oled->close(board->o_handler);
  board->fan->close(board->f_handler);
}
