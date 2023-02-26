//
// Created by bkg2k on 28/10/2019.
//

#include <input/InputEvent.h>
#include <utils/Log.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_joystick.h>

String InputEvent::TypeToString(EventType type)
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

String InputEvent::ToString() const
{
  switch (mType)
  {
    case EventType::Button:  return String("Button ").Append(mId).Append(' ').Append(mValue != 0 ? "pressed" : "released");
    case EventType::Axis:    return String("Axis ").Append(mId).Append((mValue > 0 ? '+' : '-'));
    case EventType::Hat:     return String("Hat ").Append(mId).Append(' ').Append(HatDir(mValue));
    case EventType::Key:     return String("Key ").Append(SDL_GetKeyName((SDL_Keycode)mId));
    case EventType::Unknown:
    default: break;
  }
  return String("Input to string error");
}

String InputEvent::HatDir(int val)
{
  String result;
  if ((val & SDL_HAT_UP) != 0)    result.Append("up");
  if ((val & SDL_HAT_DOWN) != 0)  result.Append(result.empty() ? "" : "+").Append("down");
  if ((val & SDL_HAT_LEFT) != 0)  result.Append(result.empty() ? "" : "+").Append("left");
  if ((val & SDL_HAT_RIGHT) != 0) result.Append(result.empty() ? "" : "+").Append("right");
  if (result.empty()) result = "center";
  return result;
}
