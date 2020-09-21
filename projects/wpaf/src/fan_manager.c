#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "config.h"

#include "cpu_controller.h"
#include "fan_driver.h"

#define HYST_LOW  40 
#define HYST_HIGH 50

extern int verbose_flag;

/*
 * This is sample fan manager
 * works for CC fans (aka on/off)
 * need to implement need functions for PWM (modulated speed fan)
 */ 
void start_fan_manager(fan_interface * fan, fan_handler * handler) {

  float temp;
  bool fan_status = false;

  // initialize
  fan->off(handler) ;

  // cpu fan controller main loop
  while(true) {
    temp = get_cpu_temp();
    if (temp > HYST_HIGH && !fan_status) {
      fan->on(handler);
      if (verbose_flag)
        printf("fan on\n");
      fan_status = true;
    }
    if (temp < HYST_LOW && fan_status) {
      fan->off(handler);
      if (verbose_flag)
        printf("fan off\n");
      fan_status = false;
    }
    if (verbose_flag)
      printf("cpu: %.2f\n", temp);
    sleep(1);
  }
}

