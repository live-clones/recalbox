//
// Created by bkg2k on 21/12/2019.
//

#include "Pad.h"
#include <input/InputDevice.h>
#include <SDL2/SDL.h>
#include <input/InputManager.h>
#include <utils/math/Misc.h>

Pad::Pad(const PadConfiguration& padConfiguration)
  : mConfiguration(nullptr)
  , mPadConfiguration(padConfiguration)
  , mSdlToRecalboxIndex {}
  , mHotkeyOnOff {}
  , mItemOnOff {}
  , mItemValues {}
  , mReady(false)
{
  memset(mItemOnOff, 0, sizeof(mItemOnOff));
  memset(mItemValues, 0, sizeof(mItemValues));
  memset(mHotkeyOnOff, 0, sizeof(mHotkeyOnOff));
}

void Pad::Open(const OrderedDevices& orderedDevices)
{
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
}

bool Pad::PopPadEvent(Pad::Event& event)
{
  if (!mEventQueue.Empty())
  {
    // Dequeue
    event = mEventQueue.Pop();

    // Process hotkey state here
    if (event.Item == PadItems::Hotkey) mHotkeyOnOff[(int)event.Pad] = event.On;
    // Convert?
    if (mHotkeyOnOff[(int)event.Pad])
      switch(event.Item)
      {
        case PadItems::Up:    event.Item = PadItems::HotKeyUp;    break;
        case PadItems::Right: event.Item = PadItems::HotKeyRight; break;
        case PadItems::Down:  event.Item = PadItems::HotKeyDown;  break;
        case PadItems::Left:  event.Item = PadItems::HotKeyLeft;  break;
        case PadItems::A:     event.Item = PadItems::HotKeyA;     break;
        case PadItems::B:     event.Item = PadItems::HotKeyB;     break;
        case PadItems::X:     event.Item = PadItems::HotKeyX;     break;
        case PadItems::Y:     event.Item = PadItems::HotKeyY;     break;
        case PadItems::L1:    event.Item = PadItems::HotKeyL1;    break;
        case PadItems::R1:    event.Item = PadItems::HotKeyR1;    break;
        case PadItems::L2:    event.Item = PadItems::HotKeyL2;    break;
        case PadItems::R2:    event.Item = PadItems::HotKeyR2;    break;
        case PadItems::L3:    event.Item = PadItems::HotKeyL3;    break;
        case PadItems::R3:    event.Item = PadItems::HotKeyR3;    break;
        case PadItems::Start: event.Item = PadItems::HotKeyStart; break;
        case PadItems::Select: // Select & Hotkey are not combinable
        case PadItems::Hotkey: break;
        case PadItems::J1Up:    event.Item = PadItems::HotKeyJ1Up;    break;
        case PadItems::J1Right: event.Item = PadItems::HotKeyJ1Right; break;
        case PadItems::J1Down:  event.Item = PadItems::HotKeyJ1Down;  break;
        case PadItems::J1Left:  event.Item = PadItems::HotKeyJ1Left;  break;
        case PadItems::J2Up:    event.Item = PadItems::HotKeyJ2Up;    break;
        case PadItems::J2Right: event.Item = PadItems::HotKeyJ2Right; break;
        case PadItems::J2Down:  event.Item = PadItems::HotKeyJ2Down;  break;
        case PadItems::J2Left:  event.Item = PadItems::HotKeyJ2Left;  break;
        case PadItems::HotKeyUp:
        case PadItems::HotKeyRight:
        case PadItems::HotKeyDown:
        case PadItems::HotKeyLeft:
        case PadItems::HotKeyA:
        case PadItems::HotKeyB:
        case PadItems::HotKeyX:
        case PadItems::HotKeyY:
        case PadItems::HotKeyL1:
        case PadItems::HotKeyR1:
        case PadItems::HotKeyL2:
        case PadItems::HotKeyR2:
        case PadItems::HotKeyL3:
        case PadItems::HotKeyR3:
        case PadItems::HotKeyStart:
        case PadItems::HotKeyJ1Up:
        case PadItems::HotKeyJ1Right:
        case PadItems::HotKeyJ1Down:
        case PadItems::HotKeyJ1Left:
        case PadItems::HotKeyJ2Up:
        case PadItems::HotKeyJ2Right:
        case PadItems::HotKeyJ2Down:
        case PadItems::HotKeyJ2Left:
        case PadItems::Invalid:
        case PadItems::__Count:
        default: break;
      }

    return true;
  }
  return false;
}

void Pad::PushSDL2Event(const SDL_Event& event)
{
  switch (event.type)
  {
    case SDL_JOYAXISMOTION:
    {
      // Get pad mapping
      int* pindex = mSdlToRecalboxIndex.try_get(event.jaxis.which);
      int index = (pindex != nullptr) ? *pindex : -1;
      if ((unsigned int) index >= Input::sMaxInputDevices) break;
      const PadConfiguration::PadAllItemConfiguration& pad = mPadConfiguration.Pad(index);

      // Get actual direction
      int dir = event.jaxis.value;
      dir = dir < -sJoystickDeadZone ? -1 : dir > sJoystickDeadZone ? 1 : 0;
      int rawDir = event.jaxis.value;
      rawDir = ((rawDir >> 31) * 2) + 1; // -/0+ => -1/+1

      // Check mapping
      for (const PadConfiguration::PadItemConfiguration& item : pad.Items)
        if (item.Type == InputEvent::EventType::Axis)
          if (item.Id == event.jaxis.axis)
          {
            // Push raw value
            if (item.Value == rawDir)
            {
              int current = (int)((float)(Math::absi(event.jaxis.value) - sJoystickDeadZone) / ((32767.f - sJoystickDeadZone) / sAxisSubRange));
              if (current < 0) current = 0; // Deadzone = nomove
              if ((int)mItemValues[index][event.jaxis.axis] != current)
              {
                mEventQueue.Push({ item.Item, current , false, true, (unsigned char)index });
                mItemValues[index][event.jaxis.axis] = (char)current;
              }
            }

            // Axis On?
            if ((dir == item.Value) && (mItemOnOff[index] & (1 << (int) item.Item)) == 0)
            {
              mEventQueue.Push({ item.Item, 0, true, false, (unsigned char)index });
              mItemOnOff[index] |= (1 << (int) item.Item);
            }
              // Axis off?
            else if ((dir == 0) && (mItemOnOff[index] & (1 << (int) item.Item)) != 0)
            {
              mEventQueue.Push({ item.Item, 0, false, false, (unsigned char)index });
              mItemOnOff[index] &= ~(1 << (int) item.Item);
            }
          }
      break;
    }
    case SDL_JOYHATMOTION:
    {
      // Get pad mapping
      int* pindex = mSdlToRecalboxIndex.try_get(event.jhat.which);
      int index = (pindex != nullptr) ? *pindex : -1;
      if ((unsigned int) index >= Input::sMaxInputDevices) break;
      const PadConfiguration::PadAllItemConfiguration& pad = mPadConfiguration.Pad(index);

      // Check mapping
      for (const PadConfiguration::PadItemConfiguration& item : pad.Items)
        if (item.Type == InputEvent::EventType::Hat)
          if (item.Id == event.jhat.hat)
          {
            // Hat bit(s) On?
            if (((event.jhat.value & item.Value) == item.Value) && (mItemOnOff[index] & (1 << (int) item.Item)) == 0)
            {
              mEventQueue.Push({ item.Item, 1, true, false, (unsigned char)index });
              mItemOnOff[index] |= (1 << (int) item.Item);
            }
              // Hat bit(s) off?
            else if (((event.jhat.value & item.Value) != item.Value) &&
                     (mItemOnOff[index] & (1 << (int) item.Item)) != 0)
            {
              mEventQueue.Push({ item.Item, 0, false, false, (unsigned char)index });
              mItemOnOff[index] &= ~(1 << (int) item.Item);
            }
          }

      break;
    }
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP:
    {
      // Get pad mapping
      int* pindex = mSdlToRecalboxIndex.try_get(event.jbutton.which);
      int index = (pindex != nullptr) ? *pindex : -1;
      if ((unsigned int) index >= Input::sMaxInputDevices) break;
      const PadConfiguration::PadAllItemConfiguration& pad = mPadConfiguration.Pad(index);

      for (const PadConfiguration::PadItemConfiguration& item : pad.Items)
        if (item.Type == InputEvent::EventType::Button)
          if (item.Id == event.jbutton.button)
          {
            // Button On?
            if ((event.jbutton.state == SDL_PRESSED) && (mItemOnOff[index] & (1 << (int) item.Item)) == 0)
            {
              mEventQueue.Push({ item.Item, 1, true, false, (unsigned char)index});
              mItemOnOff[index] |= (1 << (int) item.Item);
            }
              // Button off?
            else if ((event.jbutton.state == SDL_RELEASED) && (mItemOnOff[index] & (1 << (int) item.Item)) != 0)
            {
              mEventQueue.Push({ item.Item, 0, false, false, (unsigned char)index});
              mItemOnOff[index] &= ~(1 << (int) item.Item);
            }
          }
      break;
    }
    default: break;
  }
}


