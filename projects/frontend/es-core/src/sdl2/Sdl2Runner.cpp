//
// Created by bkg2k on 02/09/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "Sdl2Runner.h"
#include <SDL.h>
#include <input/InputManager.h>

Sdl2Runner::Sdl2Runner()
{
  { LOG(LogDebug) << "[Sdl2Runner] Start SDL runner."; }
}

Sdl2Runner::~Sdl2Runner()
{
  { LOG(LogDebug) << "[Sdl2Runner] Stop SDL runner."; }
}

void Sdl2Runner::Stop()
{
  SDL_Event quit;
  quit.type = SDL_LASTEVENT;
  SDL_PushEvent(&quit);
  { LOG(LogDebug) << "[Sdl2Runner] Requested to stop."; }
}

void Sdl2Runner::Run()
{
  SDL_Event event;
  for(bool run = true; run;)
    while (SDL_WaitEvent(&event) != 0)
    {
      //{ LOG(LogDebug) << "[Sdl2Runner] SDL Button Event."; }
      // Notify?
      {
        Mutex::AutoLock locker(mLocker);
        NotifierArray* array = mRegistered.try_get(event.type);
        if (array != nullptr)
          for(int i = array->Count(); --i >= 0; )
          {
            ISdl2EventNotifier* notifier = (*array)[i];
            if (notifier->Sdl2EventIsActive())
              notifier->Sdl2EventReceived(event);
          }
      }
      if (event.type == SDL_JOYDEVICEADDED ||
          event.type == SDL_JOYDEVICEREMOVED)
        InputManager::Instance().Refresh(nullptr, false);
      // Quit this loop on SDL_QUIT
      if (event.type == SDL_LASTEVENT)
      {
        run = false;
        break;
      }
    }
}

void Sdl2Runner::Register(int eventType, ISdl2EventNotifier* notifier)
{
  if (notifier == nullptr) return;

  Mutex::AutoLock locker(mLocker);

  // Get array
  NotifierArray* array = mRegistered.try_get(eventType);
  if (array == nullptr) array = &mRegistered[eventType];

  // If the notifier already exists, exit immediately
  for (int i = array->Count(); --i >= 0; )
    if ((*array)[i] == notifier) return;

  // Register
  array->Add(notifier);
}

void Sdl2Runner::Unregister(int eventType, ISdl2EventNotifier* notifier)
{
  if (notifier == nullptr) return;

  Mutex::AutoLock locker(mLocker);

  // Get array
  NotifierArray* array = mRegistered.try_get(eventType);
  if (array == nullptr) array = &mRegistered[eventType];

  // Check if the notifier exists in the array
  for (int i = array->Count(); --i >= 0; )
    if ((*array)[i] == notifier)
    {
      array->Delete(i);
      break;
    }
}

void Sdl2Runner::UnregisterAll(ISdl2EventNotifier* notifier)
{
  if (notifier == nullptr) return;

  Mutex::AutoLock locker(mLocker);

  // For all events
  for(auto& item : mRegistered)
    // Check if the notifier exists in the array
    for (int i = item.second.Count(); --i >= 0; )
      if (item.second[i] == notifier)
      {
        item.second.Delete(i);
        break;
      }
}
