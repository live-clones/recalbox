#include <stdlib.h>
#include <string.h>

#include "config.h"

#include "board_driver.h"
#include "fan_driver.h"
#include "log.h"
#include "ssd1306_display_controller.h"
#include "pcf8574_fan_controller.h"
#include "waveshare_poehatb.h"

board_interface waveshare_poehatb_interface = {
  &wspoehatb_init,
  &wspoehatb_close,
  &wspoehatb_get_temp_setpoint,
  &wspoehatb_query,
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
  board_handler * board = malloc(sizeof(board_handler));
  board->display = &ssd1306_interface;
  board->fan  = &pcf8574_interface;

  board->o_handler = board->display->init(WSPOEHATB_OLED_ADDRESS);
  board->f_handler = board->fan->init(WSPOEHATB_FAN_ADDRESS);
  if (!pcf8574_fan_set_ioport(board->f_handler, WSPOEHATB_PCF8574_IOPORT))
    log_error("Can't set ioport %d for pcf8574", WSPOEHATB_PCF8574_IOPORT);

  board->o_handler->extra_data = &wspoehatb_display_data;

  board->display->reset(board->o_handler);
  board->display->set_screen_size(board->o_handler, WSPOEHATB_OLED_X, WSPOEHATB_OLED_Y);

  return board;
}

void wspoehatb_close(board_handler * board) {
  board->display->close(board->o_handler);
  board->fan->close(board->f_handler);
}

uint32_t wspoehatb_get_temp_setpoint() {
  return WSPOEHATB_TEMP_SETPOINT;
}

int32_t wspoehatb_query(uint32_t queryid, void * result) {
  int32_t _d;
  switch(queryid) {
      case BOARD_QUERY_TEMP_SETPOINT:
          _d = WSPOEHATB_TEMP_SETPOINT;
          memcpy(result, &_d, sizeof(int32_t));
          break;
      case BOARD_QUERY_PWM_KP:
      case BOARD_QUERY_PWM_KI:
      case BOARD_QUERY_PWM_KD:
      case BOARD_QUERY_PWM_MIN:
      case BOARD_QUERY_PWM_MAX:
      default:
          return BOARD_QUERY_UNIMPLEMENTED;
  }
  return true;
}
