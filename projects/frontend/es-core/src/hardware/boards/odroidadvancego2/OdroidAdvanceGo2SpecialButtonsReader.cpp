//
// Created by bkg2k on 01/11/2020.
//

#include "OdroidAdvanceGo2SpecialButtonsReader.h"
#include <utils/Log.h>
#include <SDL2/SDL.h>
#include <input/InputManager.h>

OdroidAdvanceGo2SpecialButtonsReader::OdroidAdvanceGo2SpecialButtonsReader(IBoardInterface& boardInterface)
  : mBoardInterface(boardInterface)
{
}

void OdroidAdvanceGo2SpecialButtonsReader::StartReader()
{
  LOG(LogDebug) << "[OdroidAdvanceGo2] In-game special button manager requested to start.";
  Start("OAG2Pad");
}

void OdroidAdvanceGo2SpecialButtonsReader::StopReader()
{
  LOG(LogDebug) << "[OdroidAdvanceGo2] In-game special button manager requested to stop.";
  Stop();
}

void OdroidAdvanceGo2SpecialButtonsReader::Break()
{
  LOG(LogDebug) << "[OdroidAdvanceGo2] Breaking in-game special button manager thread.";
  SDL_Event event;
  event.type = SDL_QUIT;
  SDL_PushEvent(&event);
}

void OdroidAdvanceGo2SpecialButtonsReader::Run()
{
  LOG(LogInfo) << "[OdroidAdvanceGo2] Running in-game special button manager.";

  // Allow joystick event
  InputManager::IntitializeSDL2JoystickSystem();

  // Open joystick 0 - Ignore all other joysticks
  SDL_Joystick* mainJoystick = SDL_JoystickOpen(0);
  if (mainJoystick == nullptr)
  {
    LOG(LogError) << "[OdroidAdvanceGo2] Cannot open main joystick!";
    return;
  }
  // Create device
  InputDevice device(mainJoystick,
                     SDL_JoystickInstanceID(mainJoystick),
                     0,
                     SDL_JoystickName(mainJoystick),
                     SDL_JoystickGetGUID(mainJoystick),
                     SDL_JoystickNumAxes(mainJoystick),
                     SDL_JoystickNumHats(mainJoystick),
                     SDL_JoystickNumButtons(mainJoystick));
  InputManager::LookupDeviceXmlConfiguration(device);

  // Main loop
  SDL_Event sdl;
  while(IsRunning())
  {
    // This reader runs when the application is running an emulator
    // and is blocked. Since the main SDL loop is also blocked,
    // we can safely run our own
    if (SDL_WaitEvent(&sdl) == 1)
    {
      LOG(LogDebug) << "[OdroidAdvanceGo2] SDL Event.";
      switch (sdl.type)
      {
        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
        {
          LOG(LogDebug) << "[OdroidAdvanceGo2] SDL Button Event.";
          InputEvent inputEvent(sdl.jbutton.which, InputEvent::EventType::Button, sdl.jbutton.button, sdl.jbutton.state == SDL_PRESSED ? 1 : 0);
          InputManager::LogRawEvent(inputEvent);
          InputCompactEvent compactEvent = device.ConvertToCompact(inputEvent);
          InputManager::LogCompactEvent(compactEvent);
          mBoardInterface.ProcessSpecialInputs(compactEvent);
          break;
        }
        case SDL_QUIT:
        {
          LOG(LogDebug) << "[OdroidAdvanceGo2] In-game special button manager receive quit order.";
          break;
        }
        default: break;
      }
    }
  }

  // Close joystick
  SDL_JoystickClose(mainJoystick);

  // Disable joystick events
  InputManager::FinalizeSDL2JoystickSystem();
}
