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

  for(int i = 2; --i >= 0; )
  {
    unsigned int flags = (i != 0) ? mActivatedEntryFlags : mDeactivatedEntryFlags;
    std::string& result = (i != 0) ? pressed : released; 
    if ((flags & (int) Entry::Start            ) != 0) result.append(1, ',').append("Start");
    if ((flags & (int) Entry::Select           ) != 0) result.append(1, ',').append("Select");
    if ((flags & (int) Entry::Hotkey           ) != 0) result.append(1, ',').append("Hotkey");
    if ((flags & (int) Entry::A                ) != 0) result.append(1, ',').append("A");
    if ((flags & (int) Entry::B                ) != 0) result.append(1, ',').append("B");
    if ((flags & (int) Entry::X                ) != 0) result.append(1, ',').append("X");
    if ((flags & (int) Entry::Y                ) != 0) result.append(1, ',').append("Y");
    if ((flags & (int) Entry::L1               ) != 0) result.append(1, ',').append("L1");
    if ((flags & (int) Entry::R1               ) != 0) result.append(1, ',').append("R1");
    if ((flags & (int) Entry::L2               ) != 0) result.append(1, ',').append("L2");
    if ((flags & (int) Entry::R2               ) != 0) result.append(1, ',').append("R2");
    if ((flags & (int) Entry::L3               ) != 0) result.append(1, ',').append("L3");
    if ((flags & (int) Entry::R3               ) != 0) result.append(1, ',').append("R3");
    if ((flags & (int) Entry::Up               ) != 0) result.append(1, ',').append("Up");
    if ((flags & (int) Entry::Right            ) != 0) result.append(1, ',').append("Right");
    if ((flags & (int) Entry::Down             ) != 0) result.append(1, ',').append("Down");
    if ((flags & (int) Entry::Left             ) != 0) result.append(1, ',').append("Left");
    if ((flags & (int) Entry::J1Up             ) != 0) result.append(1, ',').append("J1Up");
    if ((flags & (int) Entry::J1Right          ) != 0) result.append(1, ',').append("J1Right");
    if ((flags & (int) Entry::J1Down           ) != 0) result.append(1, ',').append("J1Down");
    if ((flags & (int) Entry::J1Left           ) != 0) result.append(1, ',').append("J1Left");
    if ((flags & (int) Entry::J2Up             ) != 0) result.append(1, ',').append("J2Up");
    if ((flags & (int) Entry::J2Right          ) != 0) result.append(1, ',').append("J2Right");
    if ((flags & (int) Entry::J2Down           ) != 0) result.append(1, ',').append("J2Down");
    if ((flags & (int) Entry::J2Left           ) != 0) result.append(1, ',').append("J2Left");
    if ((flags & (int) Entry::VolUp            ) != 0) result.append(1, ',').append("VolUp");
    if ((flags & (int) Entry::VolDown          ) != 0) result.append(1, ',').append("VolDown");
    if ((flags & (int) Entry::LumUp            ) != 0) result.append(1, ',').append("LumUp");
    if ((flags & (int) Entry::LumDown          ) != 0) result.append(1, ',').append("LumDown");
    if ((flags & (int) Entry::NeedConfiguration) != 0) result.append(1, ',').append("*NEED CONF*");
  }

  return std::string("Pressed: ").append(pressed).append(" - Released: ").append(released);
}
