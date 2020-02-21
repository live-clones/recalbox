//
// Created by bkg2k on 21/12/2019.
//

#include "Pad.h"
#include <SDL2/SDL.h>


Pad::Pad(const PadConfiguration& padConfiguration, const Configuration& configuration)
  : mConfiguration(configuration),
    mPadConfiguration(padConfiguration),
    mSdlToRecalboxIndexex {},
    mItemOnOff {},
    mReady(false)
{
  Open();
}

void Pad::Open()
{
  // Allow joystick event
  SDL_Init(SDL_INIT_JOYSTICK);
  SDL_JoystickEventState(SDL_ENABLE);
  SDL_JoystickUpdate();

  // Bitflag for assigned pads
  int Assigned = 0;
  // Reset
  for(int i = Pad2Keyb::MaxPadSupported; --i >= 0; )
  {
    mSdlToRecalboxIndexex[i] = -1;
    mItemOnOff[i] = 0;
  }

  // Compute SDL to Recalbox indexes
  int count = SDL_NumJoysticks();
  for(int j = 0; j < count; ++j)
  {
    // Get joystick
    SDL_Joystick* joystick = SDL_JoystickOpen(j);
    // Get global index
    SDL_JoystickID joystickIndex = SDL_JoystickGetDeviceInstanceID(j);
    // Get informations
    const char* name = SDL_JoystickNameForIndex(j);
    char guid[64];
    SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joystick), guid, sizeof(guid));

    // SDL index too high?
    if (joystickIndex >= Pad2Keyb::MaxPadSupported) continue;

    // Lookup
    for(int i = 0; i < Pad2Keyb::MaxPadSupported; ++i)
      if (mConfiguration.Valid(i))                          // Configuration valid?
        if (strcmp(name, mConfiguration.PadName[i]) == 0)   // Name matching?
          if (strcmp(guid, mConfiguration.PadGUID[i]) == 0) // Guid matching?
            if ((Assigned & (1 << i)) == 0)                 // Not yet assigned?
            {
              mSdlToRecalboxIndexex[joystickIndex] = i;
              break;
            }
  }

  mReady = true;
}

void Pad::Release()
{
  mReady = false;
  SDL_Event event;
  event.type = SDL_QUIT;
  SDL_PushEvent(&event);
  SDL_Quit();
}

bool Pad::GetEvent(Pad::Event& event)
{
  // Pop old events first
  if (!mEventQueue.Empty()) { event = mEventQueue.Pop(); return true; }

  SDL_Event sdl;
  for(;;)
  {
    bool ok = (SDL_WaitEvent(&sdl) == 1);
    if (ok)
      switch (sdl.type)
      {
        case SDL_JOYAXISMOTION:
        {
          // Get pad mapping
          if ((unsigned int) sdl.jaxis.which >= Pad2Keyb::MaxPadSupported) break;
          int index = mSdlToRecalboxIndexex[sdl.jaxis.which]; // Get Recalbox index
          if (index < 0) break;
          const PadConfiguration::PadAllItemConfiguration& pad = mPadConfiguration.Pad(index);

          // Get actual direction
          int dir = sdl.jaxis.value;
          dir = dir < -sJoystickDeadZone ? -1 : dir > sJoystickDeadZone ? 1 : 0;

          // Check mapping
          for (const PadConfiguration::PadItemConfiguration& item : pad.Items)
            if (item.Type == PadItemTypes::Axis)
              if (item.Id == sdl.jaxis.axis)
              {
                // Axis On?
                if ((dir == item.Value) && (mItemOnOff[index] & (1 << (int) item.Item)) == 0)
                {
                  mEventQueue.Push({ item.Item, (char) index, true });
                  mItemOnOff[index] |= (1 << (int) item.Item);
                }
                  // Axis off?
                else if ((dir == 0) && (mItemOnOff[index] & (1 << (int) item.Item)) != 0)
                {
                  mEventQueue.Push({ item.Item, (char) index, false });
                  mItemOnOff[index] &= ~(1 << (int) item.Item);
                }
              }
          break;
        }
        case SDL_JOYHATMOTION:
        {
          // Get pad mapping
          if ((unsigned int) sdl.jhat.which >= Pad2Keyb::MaxPadSupported) break;
          int index = mSdlToRecalboxIndexex[sdl.jhat.which]; // Get Recalbox index
          if (index < 0) break;
          const PadConfiguration::PadAllItemConfiguration& pad = mPadConfiguration.Pad(index);

          // Check mapping
          for (const PadConfiguration::PadItemConfiguration& item : pad.Items)
            if (item.Type == PadItemTypes::Hat)
              if (item.Id == sdl.jhat.hat)
              {
                // Hat bit(s) On?
                if (((sdl.jhat.value & item.Value) == item.Value) && (mItemOnOff[index] & (1 << (int) item.Item)) == 0)
                {
                  mEventQueue.Push({ item.Item, (char) index, true });
                  mItemOnOff[index] |= (1 << (int) item.Item);
                }
                  // Hat bit(s) off?
                else if (((sdl.jhat.value & item.Value) != item.Value) &&
                         (mItemOnOff[index] & (1 << (int) item.Item)) != 0)
                {
                  mEventQueue.Push({ item.Item, (char) index, false });
                  mItemOnOff[index] &= ~(1 << (int) item.Item);
                }
              }

          break;
        }
        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
        {
          // Get pad mapping
          if ((unsigned int) sdl.jbutton.which >= Pad2Keyb::MaxPadSupported) break;
          int index = mSdlToRecalboxIndexex[sdl.jbutton.which]; // Get Recalbox index
          if (index < 0) break;
          const PadConfiguration::PadAllItemConfiguration& pad = mPadConfiguration.Pad(index);

          for (const PadConfiguration::PadItemConfiguration& item : pad.Items)
            if (item.Type == PadItemTypes::Button)
              if (item.Id == sdl.jbutton.button)
              {
                // Button On?
                if ((sdl.jbutton.state == SDL_PRESSED) && (mItemOnOff[index] & (1 << (int) item.Item)) == 0)
                {
                  mEventQueue.Push({ item.Item, (char) index, true });
                  mItemOnOff[index] |= (1 << (int) item.Item);
                }
                  // Button off?
                else if ((sdl.jbutton.state == SDL_RELEASED) && (mItemOnOff[index] & (1 << (int) item.Item)) != 0)
                {
                  mEventQueue.Push({ item.Item, (char) index, false });
                  mItemOnOff[index] &= ~(1 << (int) item.Item);
                }
              }
          break;
        }
        case SDL_QUIT: return false;
        default: break;
      }

    // Pop first of new events
    if (!mEventQueue.Empty())
    {
      event = mEventQueue.Pop();
      return true;
    }
  }
}


