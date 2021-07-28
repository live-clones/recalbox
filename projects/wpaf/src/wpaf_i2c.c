#ifdef HAVE_LIBBCM2835
#include <bcm2835.h>
#endif
#include <stdbool.h>
#include <stdint.h>

#include "log.h"

bool bcm2835_initialized = false;
#ifdef HAVE_LIBBCM2835
uint16_t i2c_clock_divider = BCM2835_I2C_CLOCK_DIVIDER_2500;
#endif

bool wpaf_i2c_initialize() {
  if (bcm2835_initialized)
    return true;
#ifdef HAVE_LIBBCM2835
  if (!bcm2835_init())
    return false;
#else
  return false;
#endif
  bcm2835_initialized = true;
  return true;
}

void wpaf_i2c_set_clock_divider(uint16_t clock_divider) {
#ifdef HAVE_LIBBCM2835
  i2c_clock_divider = clock_divider;
#endif
}

uint8_t wpaf_i2c_write_to_addr(uint8_t slave_address, char * write_buffer, uint32_t length) {
  wpaf_i2c_initialize();
#ifdef HAVE_LIBBCM2835
  bcm2835_i2c_begin() ;
  bcm2835_i2c_setSlaveAddress(slave_address);
  bcm2835_i2c_setClockDivider(i2c_clock_divider);
  return bcm2835_i2c_write(write_buffer, length);
#else
  return 0;
#endif
}

uint8_t wpaf_i2c_write1_to_addr(uint8_t slave_address, char write_buffer) {
  wpaf_i2c_initialize();
#ifdef HAVE_LIBBCM2835
  bcm2835_i2c_begin() ;
  bcm2835_i2c_setSlaveAddress(slave_address);
  bcm2835_i2c_setClockDivider(i2c_clock_divider);
  return bcm2835_i2c_write(&write_buffer, 1);
#else
  return 0;
#endif
}

uint8_t wpaf_i2c_read_from_addr(uint8_t slave_address, char * read_buffer, uint32_t length) {
  wpaf_i2c_initialize();
#ifdef HAVE_LIBBCM2835
  bcm2835_i2c_begin() ;
  bcm2835_i2c_setSlaveAddress(slave_address);
  bcm2835_i2c_setClockDivider(i2c_clock_divider);
  return bcm2835_i2c_read(read_buffer, length);
#else
  return 0;
#endif
}

uint8_t wpaf_i2c_read1_from_addr(uint8_t slave_address) {
  char data;
  wpaf_i2c_initialize();
#ifdef HAVE_LIBBCM2835
  bcm2835_i2c_begin() ;
  bcm2835_i2c_setSlaveAddress(slave_address);
  bcm2835_i2c_setClockDivider(i2c_clock_divider);
  bcm2835_i2c_read(&data, 1);
  return data;
#else
  return 0;
#endif
}

