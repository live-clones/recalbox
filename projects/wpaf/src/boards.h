#ifndef __BOARDS_H
#define __BOARDS_H

#include "board_driver.h"
#include "waveshare_poehatb.h"
#include "argon_forty.h"

enum BOARDS {
  WAVESHARE_POEHATB,
  ARGON_FORTY
};

char board_list[][50] = {
  "Waveshare PoE hat (B)",
  "Argon Forty (One / One M.2)"
};

char board_id[][50] = {
  "wspoehatb",
  "argonforty"
};

board_interface *boards[] = {
  &waveshare_poehatb_interface,
  &argon_forty_interface,
  NULL
};


#endif//__BOARDS_H
