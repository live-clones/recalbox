#ifndef __PID_CONTROLLER_H
#define __PID_CONTROLLER_H

#include <stdint.h>

struct pid {
  double dt;
  double max;
  double min;
  double Kp;
  double Kd;
  double Ki;
  double _pre_error;
  double _integral;
};


struct pid * pid_controller_init(double dt, double max, double min, double Kp, double Kd, double Ki);
double pid_controller_calculate(struct pid * _pid, double setpoint, double pv) ;
uint32_t pid_controller_free(struct pid * _pid) ;

#endif // __PID_CONTROLLER_H
