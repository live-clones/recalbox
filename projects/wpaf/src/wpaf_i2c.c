#include <bcm2835.h>
#include <stdbool.h>
#include <stdint.h>

bool bcm2835_initialized = false;
uint16_t i2c_clock_divider = BCM2835_I2C_CLOCK_DIVIDER_2500;

bool wpaf_i2c_initialize() {
  if (bcm2835_initialized)
    return true;
  if (!bcm2835_init())
    return false;
  bcm2835_initialized = true;
  return true;
}

void wpaf_i2c_set_clock_divider(uint16_t clock_divider) {
  i2c_clock_divider = clock_divider;
}

uint8_t wpaf_i2c_write_to_addr(uint8_t slave_address, char * write_buffer, uint32_t length) {
  wpaf_i2c_initialize();
  bcm2835_i2c_begin() ;
  bcm2835_i2c_setSlaveAddress(slave_address);
  bcm2835_i2c_setClockDivider(i2c_clock_divider);
  return bcm2835_i2c_write(write_buffer, length);
}

uint8_t wpaf_i2c_write1_to_addr(uint8_t slave_address, char write_buffer) {
  wpaf_i2c_initialize();
  bcm2835_i2c_begin() ;
  bcm2835_i2c_setSlaveAddress(slave_address);
  bcm2835_i2c_setClockDivider(i2c_clock_divider);
  return bcm2835_i2c_write(&write_buffer, 1);
}

uint8_t wpaf_i2c_read_from_addr(uint8_t slave_address, char * read_buffer, uint32_t length) {
  wpaf_i2c_initialize();
  bcm2835_i2c_begin() ;
  bcm2835_i2c_setSlaveAddress(slave_address);
  bcm2835_i2c_setClockDivider(i2c_clock_divider);
  return bcm2835_i2c_read(read_buffer, length);
}

uint8_t wpaf_i2c_read1_from_addr(uint8_t slave_address) {
  char data;
  wpaf_i2c_initialize();
  bcm2835_i2c_begin() ;
  bcm2835_i2c_setSlaveAddress(slave_address);
  bcm2835_i2c_setClockDivider(i2c_clock_divider);
  bcm2835_i2c_read(&data, 1);
  return data;
}

