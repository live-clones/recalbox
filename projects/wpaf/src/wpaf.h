#ifndef __WPAF_H
#define __WPAF_H

#include <stdint.h>

typedef struct {
  char * board;
  uint32_t temp_setpoint;
} __ARGS;

enum WPAF_SEMAPHORE {
  BCM2835_SEMAPHORE,
  LOGGER_SEMAPHORE,
};

#endif //__WPAF_H
