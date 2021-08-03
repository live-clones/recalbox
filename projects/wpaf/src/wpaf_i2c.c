#include "../config.h"

#ifdef HAVE_LIBBCM2835
#include <bcm2835.h>
#endif
#include <semaphore.h>
#include <stdbool.h>
#include <stdint.h>

#include "log.h"
#include "wpaf.h"

bool bcm2835_initialized = false;
#ifdef HAVE_LIBBCM2835
uint16_t i2c_clock_divider = BCM2835_I2C_CLOCK_DIVIDER_2500;
#endif

extern sem_t * semaphore[10];

bool wpaf_i2c_initialize() {
  log_trace("wpaf_i2c_initialize()");
  if (bcm2835_initialized)
    return true;
#ifdef HAVE_LIBBCM2835
  if (!bcm2835_init())
    return false;
#else
  log_error("no libbcm2835 on this system");
  return false;
#endif
  bcm2835_initialized = true;
  return true;
}

#define BCM2835_LOCK()   sem_wait(semaphore[BCM2835_SEMAPHORE])
#define BCM2835_UNLOCK() sem_post(semaphore[BCM2835_SEMAPHORE])

void wpaf_i2c_set_clock_divider(uint16_t clock_divider) {
  log_trace("wpaf_i2c_set_clock_divider(%d)", clock_divider);
#ifdef HAVE_LIBBCM2835
  i2c_clock_divider = clock_divider;
#endif
}

uint8_t wpaf_i2c_write_to_addr(uint8_t slave_address, char * write_buffer, uint32_t length) {
  uint8_t __length = 0;
  log_trace("wpaf_i2c_write_to_addr(0x%x)", slave_address);
  BCM2835_LOCK();
  wpaf_i2c_initialize();
#ifdef HAVE_LIBBCM2835
  bcm2835_i2c_begin() ;
  bcm2835_i2c_setSlaveAddress(slave_address);
  bcm2835_i2c_setClockDivider(i2c_clock_divider);
  __length = bcm2835_i2c_write(write_buffer, length);
  bcm2835_i2c_end();
#endif
  BCM2835_UNLOCK();
  return __length;
}

uint8_t wpaf_i2c_write1_to_addr(uint8_t slave_address, char write_buffer) {
  uint8_t __length = 0;
  log_trace("wpaf_i2c_write1_to_addr(0x%x)", slave_address);
  BCM2835_LOCK();
  wpaf_i2c_initialize();
#ifdef HAVE_LIBBCM2835
  bcm2835_i2c_begin() ;
  bcm2835_i2c_setSlaveAddress(slave_address);
  bcm2835_i2c_setClockDivider(i2c_clock_divider);
  __length = bcm2835_i2c_write(&write_buffer, 1);
  bcm2835_i2c_end();
#endif
  BCM2835_UNLOCK();
  return __length;
}

uint8_t wpaf_i2c_read_from_addr(uint8_t slave_address, char * read_buffer, uint32_t length) {
  uint8_t __length = 0;
  log_trace("wpaf_i2c_read_from_addr(0x%x)", slave_address);
  BCM2835_LOCK();
  wpaf_i2c_initialize();
#ifdef HAVE_LIBBCM2835
  bcm2835_i2c_begin() ;
  bcm2835_i2c_setSlaveAddress(slave_address);
  bcm2835_i2c_setClockDivider(i2c_clock_divider);
  __length = bcm2835_i2c_read(read_buffer, length);
  bcm2835_i2c_end();
#endif
  BCM2835_UNLOCK();
  return __length;
}

uint8_t wpaf_i2c_read1_from_addr(uint8_t slave_address) {
  uint8_t data = 0;
  log_trace("wpaf_i2c_read1_from_addr(0x%x)", slave_address);
  BCM2835_LOCK();
  wpaf_i2c_initialize();
#ifdef HAVE_LIBBCM2835
  bcm2835_i2c_begin() ;
  bcm2835_i2c_setSlaveAddress(slave_address);
  bcm2835_i2c_setClockDivider(i2c_clock_divider);
  bcm2835_i2c_read(&data, 1);
  bcm2835_i2c_end();
#endif
  BCM2835_UNLOCK();
  return data;
}

