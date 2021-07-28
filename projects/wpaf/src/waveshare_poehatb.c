#include <stdlib.h>

#include "config.h"

#include "board_driver.h"
#include "fan_driver.h"
#include "ssd1306_display_controller.h"
#include "pcf8574_fan_controller.h"
#include "waveshare_poehatb.h"

board_interface waveshare_poehatb_interface = {
  &wspoehatb_init,
  &wspoehatb_close
};

SSD1306_data_struct wspoehatb_display_data = {
 .display_init_sequence = {
    0xAE,
    0x2E,// end scroll
    0x40,//---set low column address
    0xB0,//---set high column address
    0xC8,//-not offset
    0x81,
    0xff,
    0xa1,
    0xa6,
    0xa8,
    0x1f,
    0xd3,
    0x00,
    0xd5,
    0xf0,
    0xd9,
    0x22,
    0xda,
    0x02,
    0xdb,
    0x49,
    0x8d,
    0x14,
    0xaf
  },
  .display_init_sequence_length = 24,
  .display_address = WSPOEHATB_OLED_ADDRESS,
};

board_handler * wspoehatb_init() {
  printf("init\n");
  board_handler * board = malloc(sizeof(board_handler));
  board->display = &ssd1306_interface;
  board->fan  = &pcf8574_interface;

  printf("init ssd1306\n");
  board->o_handler = board->display->init(WSPOEHATB_OLED_ADDRESS);
  printf("init pcf8574\n");
  board->f_handler = board->fan->init(WSPOEHATB_FAN_ADDRESS);

  printf("extra display\n");
  board->o_handler->extra_data = &wspoehatb_display_data;

  printf("reset display\n");
  board->display->reset(board->o_handler);
  printf("set screen size display\n");
  board->display->set_screen_size(board->o_handler, WSPOEHATB_OLED_X, WSPOEHATB_OLED_Y);

  return board;
}

void wspoehatb_close(board_handler * board) {
  board->display->close(board->o_handler);
  board->fan->close(board->f_handler);
}
