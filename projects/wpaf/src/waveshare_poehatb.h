#ifndef __WAVESHARE_POEHATB_H
#define __WAVESHARE_POEHATB_H

#include "board_driver.h"

#define WSPOEHATB_OLED_ADDRESS 0x3c
#define WSPOEHATB_FAN_ADDRESS 0x20

#define WSPOEHATB_OLED_X 128
#define WSPOEHATB_OLED_Y 32

extern board_interface waveshare_poehatb_interface;

board_handler * wspoehatb_init();
void wspoehatb_close(board_handler *);

#endif //__WAVESHARE_POEHATB_H
