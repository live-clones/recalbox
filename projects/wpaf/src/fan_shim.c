#ifdef HAVE_LIBBCM2835
#include <bcm2835.h>
#endif
#include <stdlib.h>
#include <string.h>

#include "config.h"

#include "fan_shim.h"
#include "log.h"
#include "rpi_pwm_fan_controller.h"
#include "board_driver.h"
#include "fan_driver.h"
#include "null_display_controller.h"

board_interface fan_shim_interface = {
  &fan_shim_init,
  &fan_shim_close,
  &fan_shim_get_temp_setpoint,
  &fan_shim_query,
};

board_handler * fan_shim_init() {
  board_handler * board = malloc(sizeof(board_handler));
  board->fan = &rpi_pwm_fan_interface;
  board->display = &null_display_interface;


  board->f_handler = board->fan->init(NULL);
  board->o_handler = board->display->init(NULL);

#ifdef HAVE_LIBBCM2835
  rpi_pwm_fan_set_port(board->f_handler, RPI_V2_GPIO_P1_12);
#else
  log_error("Can't set port, not build with libbcm2835");
#endif

  return board;
}

void fan_shim_close(board_handler * board) {
  board->fan->close(board->f_handler);
  board->display->close(board->o_handler);
}

uint32_t fan_shim_get_temp_setpoint() {
  return FAN_SHIM_TEMP_SETPOINT;
}

int32_t fan_shim_query(uint32_t queryid, void * result) {
  double _k;
  int32_t _d;
  switch(queryid) {
      case BOARD_QUERY_PWM_KP:
          _k = FAN_SHIM_PWM_KP;
          memcpy(result, &_k, sizeof(double));
          break;
      case BOARD_QUERY_PWM_KI:
          _k = FAN_SHIM_PWM_KI;
          memcpy(result, &_k, sizeof(double));
          break;
      case BOARD_QUERY_PWM_KD:
          _k = FAN_SHIM_PWM_KD;
          memcpy(result, &_k, sizeof(double));
          break;
      case BOARD_QUERY_PWM_MIN:
          _d = FAN_SHIM_PWM_MIN;
          memcpy(result, &_d, sizeof(int32_t));
          break;
      case BOARD_QUERY_PWM_MAX:
          _d = FAN_SHIM_PWM_MAX;
          memcpy(result, &_d, sizeof(int32_t));
          break;
      case BOARD_QUERY_TEMP_SETPOINT:
          _d = FAN_SHIM_TEMP_SETPOINT;
          memcpy(result, &_d, sizeof(int32_t));
          break;
      default:
          return BOARD_QUERY_UNIMPLEMENTED;
  }
  return true;
}
