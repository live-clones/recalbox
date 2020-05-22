//
// Created by bkg2k on 22/05/2020.
//

#include <input/Sdl2Extentions.h>

int SDL_JoystickAxisEventCodeById(int, int) { return -1; }
int SDL_JoystickButtonEventCodeById(int, int) { return -1; }
int SDL_JoystickHatEventCodeById(int, int) { return -1; }
std::string SDL_JoystickDevicePathById(int) { return "/dev/js0"; }
