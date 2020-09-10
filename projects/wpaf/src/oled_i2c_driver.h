#ifndef __OLED_I2C_DRIVER_H
#define __OLED_I2C_DRIVER_H

#include <stdint.h>

#define IIC_CMD  0X00
#define IIC_RAM  0X40

int oled_write_register(uint32_t fd, int value);
int oled_write_memory(uint32_t fd, int value);
int oled_write_command(uint32_t fd, int command, int value);

#endif //__OLED_I2C_DRIVER_H
