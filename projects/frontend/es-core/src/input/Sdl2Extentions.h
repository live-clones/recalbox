//
// Created by bkg2k on 22/05/2020.
//

#pragma once

#include <string>
#include <SDL2/SDL.h>

/*
 * Define fake SDL2 extended methods when compiling in debug environment
 */

#ifndef SDL_JOYSTICK_IS_OVERRIDEN_BY_RECALBOX
  int SDL_JoystickAxisEventCodeById(int, int);
  int SDL_JoystickButtonEventCodeById(int, int);
  int SDL_JoystickHatEventCodeById(int, int);
  extern DECLSPEC const char *SDLCALL SDL_JoystickDevicePathById(int);
  #ifdef _RECALBOX_
    #pragma GCC error "Mandatory SDL2 Extensions unavailable!"
  #endif
#endif

