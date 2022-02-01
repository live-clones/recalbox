//
// Created by bkg2k on 29/09/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "Sdl2Init.h"
#include <utils/Log.h>
#include <SDL2/SDL.h>

void Sdl2Init::Initialize()
{
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) < 0)
  {
    LOG(LogError) << "[SDL2] Fatal error initializing SDL 2";
    abort();
  }
}

void Sdl2Init::Finalize()
{
  SDL_Quit();
}
