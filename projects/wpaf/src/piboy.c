#include <stdlib.h>
#include <string.h>

#include "config.h"

#include "piboy.h"
#include "xpi_gamecon_fan_controller.h"
#include "board_driver.h"
#include "fan_driver.h"
#include "null_display_controller.h"

board_interface piboy_interface = {
  &piboy_init,
  &piboy_close,
  &piboy_get_temp_setpoint,
  &piboy_query,
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

uint32_t piboy_get_temp_setpoint() {
  return PIBOY_TEMP_SETPOINT;
}

int32_t piboy_query(uint32_t queryid, void * result) {
  double _k;
  int32_t _d;
  switch(queryid) {
      case BOARD_QUERY_PWM_KP:
          _k = PIBOY_PWM_KP;
          memcpy(result, &_k, sizeof(double));
          break;
      case BOARD_QUERY_PWM_KI:
          _k = PIBOY_PWM_KI;
          memcpy(result, &_k, sizeof(double));
          break;
      case BOARD_QUERY_PWM_KD:
          _k = PIBOY_PWM_KD;
          memcpy(result, &_k, sizeof(double));
          break;
      case BOARD_QUERY_PWM_MIN:
          _d = PIBOY_PWM_MIN;
          memcpy(result, &_d, sizeof(int32_t));
          break;
      case BOARD_QUERY_PWM_MAX:
          _d = PIBOY_PWM_MAX;
          memcpy(result, &_d, sizeof(int32_t));
          break;
      case BOARD_QUERY_TEMP_SETPOINT:
          _d = PIBOY_TEMP_SETPOINT;
          memcpy(result, &_d, sizeof(int32_t));
          break;
      default:
          return BOARD_QUERY_UNIMPLEMENTED;
  }
  return true;
}
