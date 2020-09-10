#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "config.h"

#include "pcf8574_fan_controller.h"
#include "cpu_controller.h"

#define HYST_LOW  40 
#define HYST_HIGH 50

#define FAN_DISABLED 0
#define FAN_ENABLED 1

#define FAN_ADDRESS 0x20


void start_fan_manager(uint32_t fan_controller_id) {

  float temp;
  bool fan_status = false;
  fan_interface * fan;
  fan_handler * handler;

  // initialize
  switch(fan_controller_id) {
    case PCF8574_CONTROLLER:
      fan = &pcf8574_interface;
      break;
  }
  handler = fan->init(FAN_ADDRESS) ;
  fan->off(handler) ;

  // cpu fan controller main loop
  while(true) {
    temp = get_cpu_temp();
    if (temp > HYST_HIGH && !fan_status) {
      fan->on(handler);
      printf("fan on\n");
      fan_status = true;
    }
    if (temp < HYST_LOW && fan_status) {
      fan->off(handler);
      printf("fan off\n");
      fan_status = false;
    }
    printf("cpu: %.2f\n", temp);
    sleep(1);
  }
}

