//
// Created by bkg2k on 28/10/2019.
//

#include "InputEvent.h"
#include <Log.h>

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
      LOG(LogError) << "Unknown int Input type: " << (int)type;
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
  LOG(LogError) << "Unknown string Input type: " << type;
  return EventType::Unknown;
}