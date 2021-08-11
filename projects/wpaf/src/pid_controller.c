#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "pid_controller.h"

struct pid * pid_controller_init(double dt, double max, double min, double Kp, double Kd, double Ki) {
  struct pid * _pid ;
  _pid = calloc(1, sizeof(struct pid));
  _pid->dt = dt;
  _pid->max = max;
  _pid->min = min;
  _pid->Kp = Kp;
  _pid->Kd = Kd;
  _pid->Ki = Ki;
  return _pid;
}

double pid_controller_calculate(struct pid * _pid, double setpoint, double pv) {
    // Calculate error
    double error = setpoint - pv;

    // Proportional term
    double Pout = _pid->Kp * error;

    // Integral term
    _pid->_integral += error * _pid->dt;
    double Iout = _pid->Ki * _pid->_integral;

    // Derivative term
    double derivative = (error - _pid->_pre_error) / _pid->dt;
    double Dout = _pid->Kd * derivative;

    // Calculate total output
    double output = Pout + Iout + Dout;

    // Restrict to max/min
    if( output > _pid->max )
        output = _pid->max;
    else if( output < _pid->min )
        output = _pid->min;

    // Save error to previous error
    _pid->_pre_error = error;

    return output;
}

uint32_t pid_controller_free(struct pid * _pid) {
  free(_pid);
  return true;
}
