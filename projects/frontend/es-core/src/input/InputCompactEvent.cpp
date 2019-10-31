//
// Created by bkg2k on 28/10/2019.
//

#include "InputCompactEvent.h"
#include "InputDevice.h"

bool InputCompactEvent::KeyDown() const
{
  return mInputDevice.IsKeyboard() && mInputEvent.Value() != 0;
}

bool InputCompactEvent::KeyUp() const
{
  return mInputDevice.IsKeyboard() && mInputEvent.Value() == 0;
}

bool InputCompactEvent::IsKeyboard() const
{
  return mInputDevice.IsKeyboard();
}

int InputCompactEvent::KeyCode() const
{
  return mInputDevice.IsKeyboard() ? mInputEvent.Id() : SDLK_UNKNOWN;
}
