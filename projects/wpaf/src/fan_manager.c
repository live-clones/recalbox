#include <search.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>

#include "config.h"

#include "cpu_controller.h"
#include "fan_driver.h"
#include "fan_manager.h"
#include "log.h"
#include "pid_controller.h"

#define HYST_LOW  55
#define HYST_HIGH 60
#define HYSTERESIS 2
#define FAN_END_OF_LIST -1

/*
 * This is fan manager
 * can handle both CC fans and PWM fans
 */ 
void start_fan_manager(fan_interface * fan, fan_handler * handler) {
  // initialize
  fan->off(handler) ;

  switch (fan->fan_capabilities()) {
    case FAN_CAP_SIMPLE:
      log_debug("Fan is simple");
      manage_simple_fan(fan, handler);
      break;
    case FAN_CAP_PWM:
      log_debug("Fan is PWM");
      manage_pwm_fan(fan, handler);
      break;
  }
}

/* handle PWM fan
 */
void manage_pwm_fan(fan_interface * fan, fan_handler * handler) {
  float temp;
  float PID;
  struct pid * _pid = pid_controller_init(1, 3, -3, 0.01, 0.001, 0.01);

  // cpu fan controller main loop
  while(true) {
    temp = get_cpu_temp();
    PID = -pid_controller_calculate(_pid, 52, temp); // 52 should be read from config file
    PID = PID >=0 ? PID : 0 ;
    fan->set_pwm(handler, (uint8_t)PID);

    log_debug("cpu: %.2f", temp);

    log_debug("with pid_controller: %0.10f", PID);

    sleep(1);
  }
}

/* start/stop CC fan
 */
void manage_simple_fan(fan_interface * fan, fan_handler * handler) {
  float temp;
  float PID;
  struct pid * _pid = pid_controller_init(1, 1, -1, 0.1, 0.01, 0.1); // PID parameters, config file ?

  fan->off(handler);
  // cpu fan controller main loop
  while(true) {
    temp = get_cpu_temp();
    PID = pid_controller_calculate(_pid, 55, temp) ; // 55 should be read from config file
    if (PID == _pid->min) {
      fan->on(handler);
      log_debug("fan on");
    }
    if (PID == _pid->max) {
      fan->off(handler);
      log_debug("fan off");
    }

    log_debug("cpu: %.2f", temp);

    log_debug("with pid_controller: %0.2f fan_status: %x", PID, fan->status(handler));

    sleep(1);
  }
}

