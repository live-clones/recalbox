#include <stdlib.h>
#include <string.h>

#include "config.h"

#include "argon_forty.h"
#include "argon_forty_fan_controller.h"
#include "board_driver.h"
#include "fan_driver.h"
#include "null_display_controller.h"

board_interface argon_forty_interface = {
  &argon_forty_init,
  &argon_forty_close,
  &argon_forty_get_temp_setpoint,
  &argon_forty_query,
};

board_handler * argon_forty_init() {
  board_handler * board = malloc(sizeof(board_handler));
  board->fan = &argon_forty_fan_interface;
  board->display = &null_display_interface;

  board->f_handler = board->fan->init(ARGON_FORTY_FAN_CONTROLLER_ADDR);
  board->o_handler = board->display->init(0);

  return board;
}

void argon_forty_close(board_handler * board) {
  board->fan->close(board->f_handler);
  board->display->close(board->o_handler);
}

uint32_t argon_forty_get_temp_setpoint() {
  return ARGON_FORTY_TEMP_SETPOINT;
}

int32_t argon_forty_query(uint32_t queryid, void * result) {
  double _k ;
  int32_t _d;
  switch(queryid) {
      case BOARD_QUERY_PWM_KP:
          _k = ARGON_FORTY_PWM_KP;
          memcpy(result, &_k, sizeof(double));
          return true;
          break;
      case BOARD_QUERY_PWM_KI:
          _k = ARGON_FORTY_PWM_KI;
          memcpy(result, &_k, sizeof(double));
          return true;
          break;
      case BOARD_QUERY_PWM_KD:
          _k = ARGON_FORTY_PWM_KD;
          memcpy(result, &_k, sizeof(double));
          return true;
          break;
      case BOARD_QUERY_PWM_MIN:
          _d = ARGON_FORTY_PWM_MIN;
          memcpy(result, &_d, sizeof(int32_t));
          break;
      case BOARD_QUERY_PWM_MAX:
          _d = ARGON_FORTY_PWM_MAX;
          memcpy(result, &_d, sizeof(int32_t));
          break;
      case BOARD_QUERY_TEMP_SETPOINT:
          _d = ARGON_FORTY_TEMP_SETPOINT;
          memcpy(result, &_d, sizeof(int32_t));
          break;
      default:
          return BOARD_QUERY_UNIMPLEMENTED;
  }
  return true;
}
