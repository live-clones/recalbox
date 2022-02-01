#include <stdlib.h>
#include <string.h>

#include "config.h"

#include "rpi_poe_plus.h"
#include "mb_pwm_fan_controller.h"
#include "board_driver.h"
#include "fan_driver.h"
#include "null_display_controller.h"

board_interface rpi_poe_plus_interface = {
  &rpi_poe_plus_init,
  &rpi_poe_plus_close,
  &rpi_poe_plus_get_temp_setpoint,
  &rpi_poe_plus_query,
};

board_handler * rpi_poe_plus_init() {
  board_handler * board = malloc(sizeof(board_handler));
  board->fan = &mb_pwm_interface;
  board->display = &null_display_interface;

  board->f_handler = board->fan->init(0);
  board->o_handler = board->display->init(0);

  return board;
}

void rpi_poe_plus_close(board_handler * board) {
  board->fan->close(board->f_handler);
  board->display->close(board->o_handler);
}

uint32_t rpi_poe_plus_get_temp_setpoint() {
  return RPI_POE_PLUS_TEMP_SETPOINT;
}

int32_t rpi_poe_plus_query(uint32_t queryid, void * result) {
  double _k ;
  int32_t _d;
  switch(queryid) {
      case BOARD_QUERY_PWM_KP:
          _k = RPI_POE_PLUS_PWM_KP;
          memcpy(result, &_k, sizeof(double));
          break;
      case BOARD_QUERY_PWM_KI:
          _k = RPI_POE_PLUS_PWM_KI;
          memcpy(result, &_k, sizeof(double));
          break;
      case BOARD_QUERY_PWM_KD:
          _k = RPI_POE_PLUS_PWM_KD;
          memcpy(result, &_k, sizeof(double));
          break;
      case BOARD_QUERY_PWM_MIN:
          _d = RPI_POE_PLUS_PWM_MIN;
          memcpy(result, &_d, sizeof(int32_t));
          break;
      case BOARD_QUERY_PWM_MAX:
          _d = RPI_POE_PLUS_PWM_MAX;
          memcpy(result, &_d, sizeof(int32_t));
          break;
      case BOARD_QUERY_TEMP_SETPOINT:
          _d = RPI_POE_PLUS_TEMP_SETPOINT;
          memcpy(result, &_d, sizeof(int32_t));
          break;
      default:
          return BOARD_QUERY_UNIMPLEMENTED;
  }
  return true;
}
