#ifndef __BOARDS_H
#define __BOARDS_H

#include "board_driver.h"
#include "waveshare_poehatb.h"

enum BOARDS {
  WAVESHARE_POEHATB
};

#define NUMBER_OF_BOARDS 1


char board_list[NUMBER_OF_BOARDS][50] = {
  "Waveshare PoE hat (B)",
};

char board_id[NUMBER_OF_BOARDS][50] = {
  "wspoehatb",
};

board_interface *boards[] = {
  &waveshare_poehatb_interface,
};


#endif//__BOARDS_H
