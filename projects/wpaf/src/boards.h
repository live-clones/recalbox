#ifndef __BOARDS_H
#define __BOARDS_H

#include "board_driver.h"
#include "waveshare_poehatb.h"
#include "argon_forty.h"
#include "piboy.h"
#include "rpi_poe_plus.h"

enum BOARDS {
  WAVESHARE_POEHATB,
  ARGON_FORTY,
  PIBOY,
  RPI_POE_PLUS,
};

const char board_list[][50] = {
  "Waveshare PoE hat (B)",
  "Argon Forty (One / One M.2)",
  "Experimental PI PiBoy DMG",
  "Raspberry PI POE+",
};

const char board_id[][50] = {
  "wspoehatb",
  "argonforty",
  "piboy",
  "rpipoeplus",
};

board_interface *boards[] = {
  &waveshare_poehatb_interface,
  &argon_forty_interface,
  &piboy_interface,
  &rpi_poe_plus_interface,
  NULL
};


#endif//__BOARDS_H
