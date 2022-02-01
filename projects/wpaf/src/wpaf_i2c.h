#ifndef WPAF_I2C_H
#define WPAF_I2C_H
#include <stdbool.h>
#include <stdint.h>

bool wpaf_i2c_initialize() ;
void wpaf_i2c_set_clock_divider(uint16_t clock_divider) ;
uint8_t wpaf_i2c_write_to_addr(uint8_t slave_address, char * write_buffer, uint32_t length) ;
uint8_t wpaf_i2c_write1_to_addr(uint8_t slave_address, char write_buffer) ;
uint8_t wpaf_i2c_read_from_addr(uint8_t slave_address, char * read_buffer, uint32_t length) ;
char wpaf_i2c_read1_from_addr(uint8_t slave_address);

#endif // WPAF_I2C_H
