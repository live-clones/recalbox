#include <search.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>

#include "config.h"

#include "board_driver.h"
#include "cpu_controller.h"
#include "fan_driver.h"
#include "fan_manager.h"
#include "log.h"
#include "pid_controller.h"
#include "wpaf.h"

/*
 * This is fan manager
 * can handle both CC fans and PWM fans
 */ 
void start_fan_manager(board_interface * interface, board_handler * handler, __ARGS * arguments) {
  // initialize
  handler->fan->off(handler->f_handler) ;
  switch (handler->fan->fan_capabilities()) {
    case FAN_CAP_SIMPLE:
      log_debug("Fan is simple");
      manage_simple_fan(interface, handler, arguments);
      break;
    case FAN_CAP_PWM:
      log_debug("Fan is PWM");
      manage_pwm_fan(interface, handler, arguments);
      break;
  }
}

/* handle PWM fan
 */
void manage_pwm_fan(board_interface * interface, board_handler * handler, __ARGS * arguments) {
  float temp;
  float PID;
  int32_t pwm_min, pwm_max;
  double kp, ki, kd;
  struct pid * _pid;
  uint32_t temp_setpoint = arguments->temp_setpoint != 0 ? arguments->temp_setpoint : interface->get_temp_setpoint();
  interface->query(BOARD_QUERY_PWM_KP, &kp);
  interface->query(BOARD_QUERY_PWM_KI, &ki);
  interface->query(BOARD_QUERY_PWM_KD, &kd);
  interface->query(BOARD_QUERY_PWM_MIN, &pwm_min),
  interface->query(BOARD_QUERY_PWM_MAX, &pwm_max),

  _pid = pid_controller_init(1, 
          pwm_max,
          pwm_min,
          kp,
          ki,
          kd);

  log_debug("CPU temp setpoint: %d PWM max: %d PWM min: %d", temp_setpoint, pwm_max, pwm_min);
  // cpu fan controller main loop
  while(true) {
    temp = get_cpu_temp();
    PID = -pid_controller_calculate(_pid, temp_setpoint, temp);
    PID = PID >=0 ? PID : 0 ;
    handler->fan->set_pwm(handler->f_handler, (uint8_t)PID);

    log_debug("cpu: %.2f", temp);

    log_debug("with pid_controller: %0.10f", PID);

    sleep(1);
  }
}

/* start/stop CC fan
 */
void manage_simple_fan(board_interface * interface, board_handler * handler, __ARGS * arguments) {
  float temp;
  float PID;
  struct pid * _pid;
  uint32_t temp_setpoint = arguments->temp_setpoint != 0 ? arguments->temp_setpoint : interface->get_temp_setpoint();

  _pid = pid_controller_init(1, 1, -1, 0.1, 0.01, 0.1); // PID parameters, config file ?

  log_debug("CPU temp setpoint: %d", temp_setpoint);
  handler->fan->off(handler->f_handler);
  // cpu fan controller main loop
  while(true) {
    temp = get_cpu_temp();
    PID = pid_controller_calculate(_pid, temp_setpoint, temp);
    if (PID == _pid->min) {
      handler->fan->on(handler->f_handler);
      log_debug("fan on");
    }
    if (PID == _pid->max) {
      handler->fan->off(handler->f_handler);
      log_debug("fan off");
    }

    log_debug("cpu: %.2f", temp);

    log_debug("with pid_controller: %0.2f fan_status: %x", PID, handler->fan->status(handler->f_handler));

    sleep(1);
  }
}

