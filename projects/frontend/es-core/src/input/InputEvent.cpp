//
// Created by bkg2k on 28/10/2019.
//

#include <input/InputEvent.h>
#include <utils/Log.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_joystick.h>

std::string InputEvent::TypeToString(EventType type)
{
  switch(type)
  {
    case EventType::Axis:		return "axis";
    case EventType::Button:	return "button";
    case EventType::Hat:		return "hat";
    case EventType::Key:		return "key";
    case EventType::Unknown:
    default:
    {
      { LOG(LogError) << "[InputEvent] Unknown int Input type: " << (int)type; }
      return "error";
    }
  }
}

InputEvent::EventType InputEvent::StringToType(const std::string& type)
{
  if (type == "axis")	  return EventType::Axis;
  if (type == "button")	return EventType::Button;
  if (type == "hat")    return EventType::Hat;
  if (type == "key")    return EventType::Key;
  { LOG(LogError) << "[InputEvent] Unknown string Input type: " << type; }
  return EventType::Unknown;
}

void InputEvent::StoreSDLCode(int deviceIndex)
{
  #ifdef SDL_JOYSTICK_IS_OVERRIDEN_BY_RECALBOX
    switch (mType)
    {
    case EventType::Axis: mCode = SDL_JoystickAxisEventCodeById(deviceIndex, mId); { LOG(LogDebug) << "[InputEvent] AXIS"; }; break;
      case EventType::Button: mCode = SDL_JoystickButtonEventCodeById(deviceIndex, mId); { LOG(LogDebug) << "[InputEvent] BUTTON"; } break;
      case EventType::Hat: mCode = SDL_JoystickHatEventCodeById(deviceIndex, mId); { LOG(LogDebug) << "[InputEvent] HAT"; } break;
      case EventType::Key:
      case EventType::Unknown: mCode = -2;
      default: break;
    }
  #else
    mCode = -1337;
    #ifdef _RECALBOX_PRODUCTION_BUILD_
      #pragma GCC error "SDL_JOYSTICK_IS_OVERRIDEN_BY_RECALBOX undefined in production build!"
    #endif
  #endif
  { LOG(LogDebug) << "[InputEvent] Device Index: " << deviceIndex << "Device ID: " << mDeviceIdentifier << "ID: " << mId << "Event: " << ToString() << " - Code: " << mCode; }
}

std::string InputEvent::ToString() const
{
  switch (mType)
  {
    case EventType::Button:  return std::string("Button ").append(Strings::ToString(mId));
    case EventType::Axis:    return std::string("Axis ").append(Strings::ToString(mId)).append(1, (mValue > 0 ? '+' : '-'));
    case EventType::Hat:     return std::string("Hat ").append(Strings::ToString(mId)).append(1, ' ').append(HatDir(mValue));
    case EventType::Key:     return std::string("Key ").append(SDL_GetKeyName((SDL_Keycode) mId));
    case EventType::Unknown:
    default: break;
  }
  return std::string("Input to string error");
}

const char* InputEvent::HatDir(int val)
{
  if ((val & SDL_HAT_UP) != 0)    return "up";
  if ((val & SDL_HAT_DOWN) != 0)  return "down";
  if ((val & SDL_HAT_LEFT) != 0)  return "left";
  if ((val & SDL_HAT_RIGHT) != 0) return "right";
  return "neutral?";
}
