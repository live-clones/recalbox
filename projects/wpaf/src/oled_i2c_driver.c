#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

#include "config.h"
#include "oled_i2c_driver.h"

// OLED I2C low level functions
int oled_write_register(uint32_t fd, int value) {
  return oled_write_command(fd, IIC_CMD, value);
}

int oled_write_memory(uint32_t fd, int value) {
  return oled_write_command(fd, IIC_RAM, value);
}

int oled_write_command(uint32_t fd, int command, int value) {
  printf("send(0x%x) 0x%x\n", command, value);
  return wiringPiI2CWriteReg8(fd, command, value);
}

