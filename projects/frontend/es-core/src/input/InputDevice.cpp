#include "InputDevice.h"
#include <string>
#include "utils/Log.h"
#include "utils/Strings.h"

std::string InputDevice::EntryToString(InputDevice::Entry entry)
{
  switch(entry)
  {
    case InputDevice::Entry::None: return "none";
    case InputDevice::Entry::Start: return "start";
    case InputDevice::Entry::Select: return "select";
    case InputDevice::Entry::Hotkey: return "hotkey";
    case InputDevice::Entry::A: return "a";
    case InputDevice::Entry::B: return "b";
    case InputDevice::Entry::X: return "x";
    case InputDevice::Entry::Y: return "y";
    case InputDevice::Entry::L1: return "l1";
    case InputDevice::Entry::R1: return "r1";
    case InputDevice::Entry::L2: return "l2";
    case InputDevice::Entry::R2: return "r2";
    case InputDevice::Entry::L3: return "l3";
    case InputDevice::Entry::R3: return "r3";
    case InputDevice::Entry::Up: return "up";
    case InputDevice::Entry::Right: return "right";
    case InputDevice::Entry::Down: return "down";
    case InputDevice::Entry::Left: return "left";
    case InputDevice::Entry::Joy1AxisH: return "joystick1left";
    case InputDevice::Entry::Joy1AxisV: return "joystick1up";
    case InputDevice::Entry::Joy2AxisH: return "joystick2left";
    case InputDevice::Entry::Joy2AxisV: return "joystick2up";
    case InputDevice::Entry::__Count:
    default:
    {
      LOG(LogError) << "Unknown int Input entry: " << (int)entry;
      return "error";
    }
  }
}

InputDevice::Entry InputDevice::StringToEntry(const std::string& entry)
{
  std::string lentry = Strings::ToLowerASCII(entry);
  if (lentry == "none") return InputDevice::Entry::None;
  if (lentry == "start") return InputDevice::Entry::Start;
  if (lentry == "select") return InputDevice::Entry::Select;
  if (lentry == "hotkey") return InputDevice::Entry::Hotkey;
  if (lentry == "a") return InputDevice::Entry::A;
  if (lentry == "b") return InputDevice::Entry::B;
  if (lentry == "x") return InputDevice::Entry::X;
  if (lentry == "y") return InputDevice::Entry::Y;
  if (lentry == "l1") return InputDevice::Entry::L1;
  if (lentry == "r1") return InputDevice::Entry::R1;
  if (lentry == "l2") return InputDevice::Entry::L2;
  if (lentry == "r2") return InputDevice::Entry::R2;
  if (lentry == "l3") return InputDevice::Entry::L3;
  if (lentry == "r3") return InputDevice::Entry::R3;
  if (lentry == "up") return InputDevice::Entry::Up;
  if (lentry == "right") return InputDevice::Entry::Right;
  if (lentry == "down") return InputDevice::Entry::Down;
  if (lentry == "left") return InputDevice::Entry::Left;
  if (lentry == "joystick1left") return InputDevice::Entry::Joy1AxisH;
  if (lentry == "joystick1up") return InputDevice::Entry::Joy1AxisV;
  if (lentry == "joystick2left") return InputDevice::Entry::Joy2AxisH;
  if (lentry == "joystick2up") return InputDevice::Entry::Joy2AxisV;
  else
  {
    LOG(LogError) << "Unknown string Input entry: " << entry;
    return InputDevice::Entry::None;
  }
}

InputDevice::InputDevice(int deviceId, int deviceIndex, const std::string& deviceName, const std::string& deviceGUID, int deviceNbAxes, int deviceNbHats, int deviceNbButtons)
  : mDeviceName(deviceName),
    mDeviceGUID(deviceGUID),
    mDeviceId(deviceId),
    mDeviceIndex(deviceIndex),
    mDeviceNbAxes(deviceNbAxes),
    mDeviceNbHats(deviceNbHats),
    mDeviceNbButtons(deviceNbButtons),
    mConfigurationBits(0),
    mPreviousAxisValues{}
{
}

void InputDevice::ClearAll()
{
  for(int i = (int)Entry::__Count; --i >= 0;)
    mInputEvents[i] = InputEvent();
  mConfigurationBits = 0;
}

void InputDevice::LoadFrom(const InputDevice& source)
{
  *this = source;
}

/*
std::string InputDevice::getSDLPowerLevel()
{
  SDL_Joystick* joy;
  //joy = InputManager::getInstance()->getJoystickByJoystickID(getDeviceId());
  joy = SDL_JoystickOpen(getDeviceId());
  SDL_JoystickPowerLevel power = SDL_JoystickCurrentPowerLevel(joy);
  switch(power)
  {
    case SDL_JOYSTICK_POWER_EMPTY:   return "\uF1b6";
    case SDL_JOYSTICK_POWER_FULL:    return "\uF1b7";
    case SDL_JOYSTICK_POWER_LOW:     return "\uF1b1";
    case SDL_JOYSTICK_POWER_MAX:     return "\uF1ba";
    case SDL_JOYSTICK_POWER_MEDIUM:  return "\uF1b8";
    case SDL_JOYSTICK_POWER_UNKNOWN: return "\uF1b9";
    case SDL_JOYSTICK_POWER_WIRED:   return "\uF1b4";
    default: break;
  }

  return "";
}

std::string InputDevice::getSysPowerLevel()
{
  SDL_Joystick* joy;
  //joy = InputManager::getInstance()->getJoystickByJoystickID(getDeviceId());
  joy = SDL_JoystickOpen(getDeviceId());
  (void)joy; // TO-DO: Check usefulness
  return "\uF1be";
}
*/

void InputDevice::Set(Entry input, InputEvent event)
{
  mInputEvents[(int)input] = event;
  mConfigurationBits |= 1 << (int)input;
}

void InputDevice::Unset(Entry input)
{
  mInputEvents[(int)input] = InputEvent();
  mConfigurationBits &= ~(1 << (int)input);
}

bool InputDevice::GetEntryConfiguration(Entry input, InputEvent& result) const
{
  if ((mConfigurationBits & (1 << (int)input)) != 0)
  {
    result = mInputEvents[(int)input];
    return true;
  }
  return false;
}

bool InputDevice::IsMatching(Entry input, InputEvent event) const
{
  if ((mConfigurationBits & (1 << (int)input)) != 0)
  {
    const InputEvent& comp = mInputEvents[(int) input];
    if (comp.Type() == event.Type() && comp.Id() == event.Id())
    {
      switch(comp.Type())
      {
        case InputEvent::EventType::Button:
        case InputEvent::EventType::Key: return true;
        case InputEvent::EventType::Hat: return (event.Value() == 0 || ((event.Value() & comp.Value()) != 0));
        case InputEvent::EventType::Axis: return event.Value() == 0 || comp.Value() == event.Value();
        case InputEvent::EventType::Unknown:
        default: break;
      }
    }
  }
  return false;
}

InputDevice::Entry InputDevice::GetMatchedEntry(InputEvent input) const
{
  std::vector<std::string> maps;

  for (int i = (int)Entry::__Count; --i >= 0; )
    if ((mConfigurationBits & (1 << (int)i)) != 0)
    {
      const InputEvent& chk = mInputEvents[i];

      if(chk.Device() == input.Device() && chk.Type() == input.Type() && chk.Id() == input.Id())
      {
        if(chk.Type() == InputEvent::EventType::Hat)
        {
          if (input.Value() == 0 || ((input.Value() & chk.Value()) != 0))
            return (Entry) i;
        }
        else if(input.Type() == InputEvent::EventType::Axis)
        {
          if(input.Value() == 0 || chk.Value() == input.Value())
            return (Entry)i;
        }
        else
        {
          return (Entry)i;
        }
      }
    }

  return Entry::None;
}

int InputDevice::LoadFromXml(pugi::xml_node root)
{
  ClearAll();

  int loaded = 0;
  for (pugi::xml_node input = root.child("input"); input != nullptr; input = input.next_sibling("input"))
  {
    std::string name = input.attribute("name").as_string();
    std::string type = input.attribute("type").as_string();
    InputEvent::EventType typeEnum = InputEvent::StringToType(type);

    if(typeEnum == InputEvent::EventType::Unknown)
    {
      LOG(LogError) << "InputConfig load error - input of type \"" << type << "\" is invalid! Skipping input \"" << name << "\".\n";
      continue;
    }

    int id = input.attribute("id").as_int();
    int value = input.attribute("value").as_int();

    if(value == 0)
      LOG(LogWarning) << "WARNING: InputConfig value is 0 for " << type << " " << id << "!\n";

    // Adapt seamlessly old l1/r1
    if (name == "pageup") name = "l1";
    if (name == "pagedown") name = "r1";
    Entry entry = StringToEntry(name);
    if (entry != Entry::None)
    {
      Set(entry, InputEvent(mDeviceId, typeEnum, id, value));
      loaded++;
    }
    else LOG(LogError) << "Unknown Joystick configuration entry: " << name << " of type " << type << "!\n";
  }
  return loaded;
}

void InputDevice::SaveToXml(pugi::xml_node parent) const
{
  pugi::xml_node cfg = parent.append_child("inputConfig");

  if(mDeviceId == InputEvent::sKeyboardDevice)
  {
    cfg.append_attribute("type") = "keyboard";
    cfg.append_attribute("deviceName") = "Keyboard";
  }else{
    cfg.append_attribute("type") = "joystick";
    cfg.append_attribute("deviceName") = mDeviceName.c_str();
  }

  cfg.append_attribute("deviceGUID") = mDeviceGUID.c_str();
    cfg.append_attribute("deviceNbAxes") = mDeviceNbAxes;
    cfg.append_attribute("deviceNbHats") = mDeviceNbHats;
    cfg.append_attribute("deviceNbButtons") = mDeviceNbButtons;

  for (int i = (int)Entry::__Count; --i >= 0; )
  {
    if ((mConfigurationBits & (1 << (int)i)) == 0) continue;
    const InputEvent& entry = mInputEvents[i];

    pugi::xml_node input = cfg.append_child("input");
    input.append_attribute("name") = EntryToString((Entry)i).c_str();
    input.append_attribute("type") = InputEvent::TypeToString(entry.Type()).c_str();
    input.append_attribute("id").set_value(entry.Id());
    input.append_attribute("value").set_value(entry.Value());
    if(entry.Code() != -1) {
      input.append_attribute("code").set_value(entry.Code());
    }
  }
}

InputCompactEvent InputDevice::ConvertToCompact(const InputEvent& event)
{
  InputCompactEvent::Entry on  = InputCompactEvent::Entry::Nothing;
  InputCompactEvent::Entry off = InputCompactEvent::Entry::Nothing;

  // Need configuration?
  if (!IsConfigured())
      off = (on |= InputCompactEvent::Entry::NeedConfiguration);

  // Normal processing
  for (int i = (int)Entry::__Count; --i >= 0; )
    if ((mConfigurationBits & (1 << (int)i)) != 0)
    {
      const InputEvent& current = mInputEvents[i];
      InputCompactEvent::Entry targetEntry = ConvertEntry((Entry)i);

      if(current.Device() == event.Device() && current.Type() == event.Type() && current.Id() == event.Id())
      switch(event.Type())
      {
        case InputEvent::EventType::Hat:
        {
          // Hat bits are compared strictly because any position might be configured.
          // Also any non-equal entry relying on the same hat will be set to off
          // That is, a center position will set any hat mapping to off
          if (event.Value() == current.Value()) on |= targetEntry;
          else off |= targetEntry;
          break;
        }
        case InputEvent::EventType::Axis:
        {
          // For axis, we must distinguish target axis from other binary targets.
          // Axis to axis are converted to negative/center/positive.
          // Axis to binaries are converted to on only for the right sign. off otherwise
          // That is, axis on triggers will be properly converted to on/off buttons.
          if (current.Type() == InputEvent::EventType::Axis)
          {
            InputCompactEvent::Entry targetOpposite = (InputCompactEvent::Entry)((int)targetEntry << 1);
            // Since configured event are negatives, if we got a positive value in current config, that means
            // the joystick is inverted.
            // Note: values are already normalized to -1/0/+1
            int value = (current.Value() > 0) ? -event.Value() : event.Value();
            if (value < 0) on |= targetEntry;
            else if (value > 0) on |= targetOpposite;
            else off |= targetEntry | targetOpposite; // Reset both directions
          }
          else
          {
            // If the config is negative, invert the value so that we just compare to positive range
            int value = (current.Value() < 0) ? -event.Value() : event.Value();
            if (value > 0) on |= targetEntry;
            else off|= targetEntry;
          }
          break;
        }
        case InputEvent::EventType::Key:
        case InputEvent::EventType::Button:
        {
          // Buttons and keys are simple on/off
          if (event.Value() != 0) on |= targetEntry;
          else off |= targetEntry;
          break;
        }
        case InputEvent::EventType::Unknown:
        default:
        {
          LOG(LogError) << "Abnormal InputEvent::EventType: " << (int)event.Type();
        }
      }
    }

  return {on, off, *this, event};
}

InputCompactEvent::Entry InputDevice::ConvertEntry(InputDevice::Entry entry)
{
  switch(entry)
  {
    case Entry::Start: return InputCompactEvent::Entry::Start;
    case Entry::Select: return InputCompactEvent::Entry::Select;
    case Entry::Hotkey: return InputCompactEvent::Entry::Hotkey;
    case Entry::A: return InputCompactEvent::Entry::A;
    case Entry::B: return InputCompactEvent::Entry::B;
    case Entry::X: return InputCompactEvent::Entry::X;
    case Entry::Y: return InputCompactEvent::Entry::Y;
    case Entry::L1: return InputCompactEvent::Entry::L1;
    case Entry::R1: return InputCompactEvent::Entry::R1;
    case Entry::L2: return InputCompactEvent::Entry::L2;
    case Entry::R2: return InputCompactEvent::Entry::R2;
    case Entry::L3: return InputCompactEvent::Entry::L3;
    case Entry::R3: return InputCompactEvent::Entry::R3;
    case Entry::Up: return InputCompactEvent::Entry::Up;
    case Entry::Right: return InputCompactEvent::Entry::Right;
    case Entry::Down: return InputCompactEvent::Entry::Down;
    case Entry::Left: return InputCompactEvent::Entry::Left;
    case Entry::Joy1AxisH: return InputCompactEvent::Entry::J1Left;
    case Entry::Joy1AxisV: return InputCompactEvent::Entry::J1Up;
    case Entry::Joy2AxisH: return InputCompactEvent::Entry::J2Left;
    case Entry::Joy2AxisV: return InputCompactEvent::Entry::J2Up;
    case Entry::None:
    case Entry::__Count:
    default: break;
  }
  LOG(LogError) << "Unknown entry! " << (int)entry;
  return InputCompactEvent::Entry::Nothing;
}

