//
// Created by bkg2k on 01/11/2020.
//

#include "RG353XSpecialButtonsReader.h"
#include <utils/Log.h>
#include <SDL2/SDL.h>
#include <input/InputManager.h>

RG353XSpecialButtonsReader::RG353XSpecialButtonsReader(IBoardInterface& boardInterface)
  : mBoardInterface(boardInterface)
  , mDevice(nullptr)
{
}

void RG353XSpecialButtonsReader::StartReader(Sdl2Runner& sdlRunner)
{
  { LOG(LogDebug) << "[RG353X] In-game special button manager requested to start."; }

  // Create device
  if (InputManager::Instance().DeviceCount() != 0)
    mDevice = &InputManager::Instance().GetDeviceConfigurationFromIndex(0);

  // Register SDL2 events
  int Sdl2EventToRegister[] =
  {
    SDL_JOYBUTTONDOWN,
    SDL_JOYBUTTONUP,
  };
  for(int event : Sdl2EventToRegister)
    sdlRunner.Register(event, this);
}

void RG353XSpecialButtonsReader::StopReader(Sdl2Runner& sdlRunner)
{
  { LOG(LogDebug) << "[RG353X] In-game special button manager requested to stop."; }

  sdlRunner.UnregisterAll(this);
}

void RG353XSpecialButtonsReader::Sdl2EventReceived(const SDL_Event& event)
{
  switch (event.type)
  {
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP:
    {
      //{ LOG(LogDebug) << "[RG353X] SDL Button Event."; }
      InputEvent inputEvent(event.jbutton.which, InputEvent::EventType::Button, event.jbutton.button, event.jbutton.state == SDL_PRESSED ? 1 : 0);
      //InputManager::LogRawEvent(inputEvent);
      InputCompactEvent compactEvent = mDevice->ConvertToCompact(inputEvent);
      //InputManager::LogCompactEvent(compactEvent);
      mBoardInterface.ProcessSpecialInputs(compactEvent);
      break;
    }
    case SDL_QUIT:
    {
      { LOG(LogDebug) << "[RG353X] In-game special button manager receive quit order."; }
      break;
    }
    default: break;
  }
}
