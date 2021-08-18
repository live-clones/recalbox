//
// Created by bkg2k on 21/12/2019.
//

#include "Pad.h"
#include <input/InputDevice.h>
#include <SDL2/SDL.h>
#include <input/InputManager.h>
#include <utils/math/Misc.h>

Pad::Pad(const PadConfiguration& padConfiguration)
  : mConfiguration(nullptr),
    mPadConfiguration(padConfiguration),
    mSdlToRecalboxIndex {},
    mItemOnOff {},
    mItemValues {},
    mReady(false)
{
  memset(mItemOnOff, 0, sizeof(mItemOnOff));
  memset(mItemValues, 0, sizeof(mItemValues));
}

void Pad::Open(const OrderedDevices& orderedDevices)
{
  // Allow joystick event
  InputManager::IntitializeSDL2JoystickSystem();

  // Bitflag for assigned pads
  int Assigned = 0;
  // Reset
  for(int i = Input::sMaxInputDevices; --i >= 0; )
  {
    mSdlToRecalboxIndex[i] = -1;
    mItemOnOff[i] = 0;
  }

  // Compute SDL to Recalbox indexes
  int count = SDL_NumJoysticks();
  for(int j = 0; j < count; ++j)
  {
    // Get joystick
    SDL_Joystick* joystick = SDL_JoystickOpen(j);
    // Get global index
    SDL_JoystickID joystickIdentifier = SDL_JoystickInstanceID(joystick);
    // Get informations
    const char* name = SDL_JoystickNameForIndex(j);
    char guid[64];
    memset(guid, 0, sizeof(guid));
    SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joystick), guid, sizeof(guid));

    // Lookup
    for(int i = 0; i < orderedDevices.Count(); ++i)
    {
      const InputDevice& device = orderedDevices.Device(i);
      if (device.Index() == j) // Joystick index match?
        if ((Assigned & (1 << i)) == 0)         // Not yet assigned?
        {
          Assigned |= (1 << i);
          mSdlToRecalboxIndex[joystickIdentifier] = i;
          { LOG(LogInfo) << "[Pad2Keyboard] "<< joystickIdentifier << ':' << name << " (" << guid << ") assigned as Pad #" << i; }
          break;
        }
    }
  }

  mReady = (Assigned != 0);
  if (!mReady) { LOG(LogWarning) << "[Pad2Keyboard] No Joystick assigned!"; }
}

void Pad::Release()
{
  mReady = false;

  InputManager::FinalizeSDL2JoystickSystem();

  SDL_Event event;
  event.type = SDL_QUIT;
  SDL_PushEvent(&event);
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
    {
      switch (sdl.type)
      {
        case SDL_JOYAXISMOTION:
        {
          // Get pad mapping
          int* pindex = mSdlToRecalboxIndex.try_get(sdl.jaxis.which);
          int index = (pindex != nullptr) ? *pindex : -1;
          if ((unsigned int) index >= Input::sMaxInputDevices) break;
          const PadConfiguration::PadAllItemConfiguration& pad = mPadConfiguration.Pad(index);

          // Get actual direction
          int dir = sdl.jaxis.value;
          dir = dir < -sJoystickDeadZone ? -1 : dir > sJoystickDeadZone ? 1 : 0;
          int rawDir = sdl.jaxis.value;
          rawDir = ((rawDir >> 31) * 2) + 1; // -/0+ => -1/+1

          // Check mapping
          for (const PadConfiguration::PadItemConfiguration& item : pad.Items)
            if (item.Type == InputEvent::EventType::Axis)
              if (item.Id == sdl.jaxis.axis)
              {
                // Push raw value
                if (item.Value == rawDir)
                {
                  int current = (int)((float)(Math::absi(sdl.jaxis.value) - sJoystickDeadZone) / ((32767.f - sJoystickDeadZone) / sAxisSubRange));
                  if (current < 0) current = 0; // Deadzone = nomove
                  if ((int)mItemValues[index][sdl.jaxis.axis] != current)
                  {
                    mEventQueue.Push({ item.Item, current , false, true, (char) index });
                    mItemValues[index][sdl.jaxis.axis] = (char)current;
                  }
                }

                // Axis On?
                if ((dir == item.Value) && (mItemOnOff[index] & (1 << (int) item.Item)) == 0)
                {
                  mEventQueue.Push({ item.Item, 0, true, false, (char) index });
                  mItemOnOff[index] |= (1 << (int) item.Item);
                }
                // Axis off?
                else if ((dir == 0) && (mItemOnOff[index] & (1 << (int) item.Item)) != 0)
                {
                  mEventQueue.Push({ item.Item, 0, false, false, (char) index });
                  mItemOnOff[index] &= ~(1 << (int) item.Item);
                }
              }
          break;
        }
        case SDL_JOYHATMOTION:
        {
          // Get pad mapping
          int* pindex = mSdlToRecalboxIndex.try_get(sdl.jhat.which);
          int index = (pindex != nullptr) ? *pindex : -1;
          if ((unsigned int) index >= Input::sMaxInputDevices) break;
          const PadConfiguration::PadAllItemConfiguration& pad = mPadConfiguration.Pad(index);

          // Check mapping
          for (const PadConfiguration::PadItemConfiguration& item : pad.Items)
            if (item.Type == InputEvent::EventType::Hat)
              if (item.Id == sdl.jhat.hat)
              {
                // Hat bit(s) On?
                if (((sdl.jhat.value & item.Value) == item.Value) && (mItemOnOff[index] & (1 << (int) item.Item)) == 0)
                {
                  mEventQueue.Push({ item.Item, 1, true, false, (char) index });
                  mItemOnOff[index] |= (1 << (int) item.Item);
                }
                  // Hat bit(s) off?
                else if (((sdl.jhat.value & item.Value) != item.Value) &&
                         (mItemOnOff[index] & (1 << (int) item.Item)) != 0)
                {
                  mEventQueue.Push({ item.Item, 0, false, false, (char) index });
                  mItemOnOff[index] &= ~(1 << (int) item.Item);
                }
              }

          break;
        }
        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
        {
          // Get pad mapping
          int* pindex = mSdlToRecalboxIndex.try_get(sdl.jbutton.which);
          int index = (pindex != nullptr) ? *pindex : -1;
          if ((unsigned int) index >= Input::sMaxInputDevices) break;
          const PadConfiguration::PadAllItemConfiguration& pad = mPadConfiguration.Pad(index);

          for (const PadConfiguration::PadItemConfiguration& item : pad.Items)
            if (item.Type == InputEvent::EventType::Button)
              if (item.Id == sdl.jbutton.button)
              {
                // Button On?
                if ((sdl.jbutton.state == SDL_PRESSED) && (mItemOnOff[index] & (1 << (int) item.Item)) == 0)
                {
                  mEventQueue.Push({ item.Item, 1, true, false, (char) index });
                  mItemOnOff[index] |= (1 << (int) item.Item);
                }
                  // Button off?
                else if ((sdl.jbutton.state == SDL_RELEASED) && (mItemOnOff[index] & (1 << (int) item.Item)) != 0)
                {
                  mEventQueue.Push({ item.Item, 0, false, false, (char) index });
                  mItemOnOff[index] &= ~(1 << (int) item.Item);
                }
              }
          break;
        }
        case SDL_QUIT: return false;
        default: break;
      }
    }
    else
    {
      { LOG(LogDebug) << "[Pad2Keyboard] SDL Event Error"; }
      return false;
    }

    // Pop first of new events
    if (!mEventQueue.Empty())
    {
      event = mEventQueue.Pop();
      return true;
    }
  }
}


