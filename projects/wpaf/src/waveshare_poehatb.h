#ifndef __WAVESHARE_POEHATB_H
#define __WAVESHARE_POEHATB_H

#include <stdint.h>

#include "board_driver.h"
#include "ssd1306_display_controller.h"

#define WSPOEHATB_OLED_ADDRESS 0x3c
#define WSPOEHATB_FAN_ADDRESS 0x20
#define WSPOEHATB_PCF8574_IOPORT 0

#define WSPOEHATB_OLED_X 128
#define WSPOEHATB_OLED_Y 32

// default setpoint
#define WSPOEHATB_TEMP_SETPOINT 55

extern board_interface waveshare_poehatb_interface;

board_handler * wspoehatb_init();
void wspoehatb_close(board_handler *);
uint32_t wspoehatb_get_temp_setpoint();
int32_t wspoehatb_query(uint32_t queryid, void*);

#endif //__WAVESHARE_POEHATB_H
