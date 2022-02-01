//
// Created by bkg2k on 28/10/2019.
//

#include "InputCompactEvent.h"
#include "InputDevice.h"
#include "SDL2/SDL.h"

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

std::string InputCompactEvent::ToString() const
{
  std::string pressed;
  std::string released;

  if ((mActivatedEntryFlags & (int)Entry::Start  ) != 0) pressed.append(1, ',').append("Start");
  if ((mActivatedEntryFlags & (int)Entry::Select ) != 0) pressed.append(1, ',').append("Select");
  if ((mActivatedEntryFlags & (int)Entry::Hotkey ) != 0) pressed.append(1, ',').append("Hotkey");
  if ((mActivatedEntryFlags & (int)Entry::A      ) != 0) pressed.append(1, ',').append("A");
  if ((mActivatedEntryFlags & (int)Entry::B      ) != 0) pressed.append(1, ',').append("B");
  if ((mActivatedEntryFlags & (int)Entry::X      ) != 0) pressed.append(1, ',').append("X");
  if ((mActivatedEntryFlags & (int)Entry::Y      ) != 0) pressed.append(1, ',').append("Y");
  if ((mActivatedEntryFlags & (int)Entry::L1     ) != 0) pressed.append(1, ',').append("L1");
  if ((mActivatedEntryFlags & (int)Entry::R1     ) != 0) pressed.append(1, ',').append("R1");
  if ((mActivatedEntryFlags & (int)Entry::L2     ) != 0) pressed.append(1, ',').append("L2");
  if ((mActivatedEntryFlags & (int)Entry::R2     ) != 0) pressed.append(1, ',').append("R2");
  if ((mActivatedEntryFlags & (int)Entry::L3     ) != 0) pressed.append(1, ',').append("L3");
  if ((mActivatedEntryFlags & (int)Entry::R3     ) != 0) pressed.append(1, ',').append("R3");
  if ((mActivatedEntryFlags & (int)Entry::Up     ) != 0) pressed.append(1, ',').append("Up");
  if ((mActivatedEntryFlags & (int)Entry::Right  ) != 0) pressed.append(1, ',').append("Right");
  if ((mActivatedEntryFlags & (int)Entry::Down   ) != 0) pressed.append(1, ',').append("Down");
  if ((mActivatedEntryFlags & (int)Entry::Left   ) != 0) pressed.append(1, ',').append("Left");
  if ((mActivatedEntryFlags & (int)Entry::J1Up   ) != 0) pressed.append(1, ',').append("J1Up");
  if ((mActivatedEntryFlags & (int)Entry::J1Right) != 0) pressed.append(1, ',').append("J1Right");
  if ((mActivatedEntryFlags & (int)Entry::J1Down ) != 0) pressed.append(1, ',').append("J1Down");
  if ((mActivatedEntryFlags & (int)Entry::J1Left ) != 0) pressed.append(1, ',').append("J1Left");
  if ((mActivatedEntryFlags & (int)Entry::J2Up   ) != 0) pressed.append(1, ',').append("J2Up");
  if ((mActivatedEntryFlags & (int)Entry::J2Right) != 0) pressed.append(1, ',').append("J2Right");
  if ((mActivatedEntryFlags & (int)Entry::J2Down ) != 0) pressed.append(1, ',').append("J2Down");
  if ((mActivatedEntryFlags & (int)Entry::J2Left ) != 0) pressed.append(1, ',').append("J2Left");
  if ((mActivatedEntryFlags & (int)Entry::NeedConfiguration) != 0) pressed.append(1, ',').append("*NEED CONF*");

  if ((mDeactivatedEntryFlags & (int)Entry::Start  ) != 0) released.append(1, ',').append("Start");
  if ((mDeactivatedEntryFlags & (int)Entry::Select ) != 0) released.append(1, ',').append("Select");
  if ((mDeactivatedEntryFlags & (int)Entry::Hotkey ) != 0) released.append(1, ',').append("Hotkey");
  if ((mDeactivatedEntryFlags & (int)Entry::A      ) != 0) released.append(1, ',').append("A");
  if ((mDeactivatedEntryFlags & (int)Entry::B      ) != 0) released.append(1, ',').append("B");
  if ((mDeactivatedEntryFlags & (int)Entry::X      ) != 0) released.append(1, ',').append("X");
  if ((mDeactivatedEntryFlags & (int)Entry::Y      ) != 0) released.append(1, ',').append("Y");
  if ((mDeactivatedEntryFlags & (int)Entry::L1     ) != 0) released.append(1, ',').append("L1");
  if ((mDeactivatedEntryFlags & (int)Entry::R1     ) != 0) released.append(1, ',').append("R1");
  if ((mDeactivatedEntryFlags & (int)Entry::L2     ) != 0) released.append(1, ',').append("L2");
  if ((mDeactivatedEntryFlags & (int)Entry::R2     ) != 0) released.append(1, ',').append("R2");
  if ((mDeactivatedEntryFlags & (int)Entry::L3     ) != 0) released.append(1, ',').append("L3");
  if ((mDeactivatedEntryFlags & (int)Entry::R3     ) != 0) released.append(1, ',').append("R3");
  if ((mDeactivatedEntryFlags & (int)Entry::Up     ) != 0) released.append(1, ',').append("Up");
  if ((mDeactivatedEntryFlags & (int)Entry::Right  ) != 0) released.append(1, ',').append("Right");
  if ((mDeactivatedEntryFlags & (int)Entry::Down   ) != 0) released.append(1, ',').append("Down");
  if ((mDeactivatedEntryFlags & (int)Entry::Left   ) != 0) released.append(1, ',').append("Left");
  if ((mDeactivatedEntryFlags & (int)Entry::J1Up   ) != 0) released.append(1, ',').append("J1Up");
  if ((mDeactivatedEntryFlags & (int)Entry::J1Right) != 0) released.append(1, ',').append("J1Right");
  if ((mDeactivatedEntryFlags & (int)Entry::J1Down ) != 0) released.append(1, ',').append("J1Down");
  if ((mDeactivatedEntryFlags & (int)Entry::J1Left ) != 0) released.append(1, ',').append("J1Left");
  if ((mDeactivatedEntryFlags & (int)Entry::J2Up   ) != 0) released.append(1, ',').append("J2Up");
  if ((mDeactivatedEntryFlags & (int)Entry::J2Right) != 0) released.append(1, ',').append("J2Right");
  if ((mDeactivatedEntryFlags & (int)Entry::J2Down ) != 0) released.append(1, ',').append("J2Down");
  if ((mDeactivatedEntryFlags & (int)Entry::J2Left ) != 0) released.append(1, ',').append("J2Left");
  if ((mDeactivatedEntryFlags & (int)Entry::NeedConfiguration) != 0) released.append(1, ',').append("*NEED CONF*");

  return std::string("Pressed: ").append(pressed).append(" - Released: ").append(released);
}
