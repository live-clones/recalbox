#include "InputDevice.h"
#include <string>
#include <utils/Log.h>
#include <utils/Strings.h>
#include <utils/storage/HashMap.h>

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
    case InputDevice::Entry::VolumeUp: return "vol+";
    case InputDevice::Entry::VolumeDown: return "vol-";
    case InputDevice::Entry::BrightnessUp: return "lum+";
    case InputDevice::Entry::BrightnessDown: return "lum-";
    case InputDevice::Entry::__Count:
    default:
    {
      { LOG(LogError) << "[InputDevice] Unknown int Input entry: " << (int)entry; }
      return "error";
    }
  }
}

InputDevice::Entry InputDevice::StringToEntry(const std::string& entry)
{
  static HashMap<std::string, InputDevice::Entry> sStringToEntry
  {
    { "none", InputDevice::Entry::None },
    { "start", InputDevice::Entry::Start },
    { "select", InputDevice::Entry::Select },
    { "hotkey", InputDevice::Entry::Hotkey },
    { "a", InputDevice::Entry::A },
    { "b", InputDevice::Entry::B },
    { "x", InputDevice::Entry::X },
    { "y", InputDevice::Entry::Y },
    { "l1", InputDevice::Entry::L1 },
    { "r1", InputDevice::Entry::R1 },
    { "l2", InputDevice::Entry::L2 },
    { "r2", InputDevice::Entry::R2 },
    { "l3", InputDevice::Entry::L3 },
    { "r3", InputDevice::Entry::R3 },
    { "up", InputDevice::Entry::Up },
    { "right", InputDevice::Entry::Right },
    { "down", InputDevice::Entry::Down },
    { "left", InputDevice::Entry::Left },
    { "joystick1left", InputDevice::Entry::Joy1AxisH },
    { "joystick1up", InputDevice::Entry::Joy1AxisV },
    { "joystick2left", InputDevice::Entry::Joy2AxisH },
    { "joystick2up", InputDevice::Entry::Joy2AxisV },
    { "vol+", InputDevice::Entry::VolumeUp },
    { "vol-", InputDevice::Entry::VolumeDown },
    { "lum+", InputDevice::Entry::BrightnessUp },
    { "lum-", InputDevice::Entry::BrightnessDown },
  };
  InputDevice::Entry* result = sStringToEntry.try_get(Strings::ToLowerASCII(entry));
  if (result != nullptr) return *result;
  { LOG(LogError) << "[InputDevice] Unknown string Input entry: " << entry; }
  return InputDevice::Entry::None;
}

InputDevice::InputDevice(SDL_Joystick* device, SDL_JoystickID deviceId, int deviceIndex, const String& deviceName, const SDL_JoystickGUID& deviceGUID, int deviceNbAxes, int deviceNbHats, int deviceNbButtons)
  : mDeviceName(deviceName),
    mDeviceGUID(deviceGUID),
    mDeviceSDL(device),
    mDeviceId(deviceId),
    mDeviceIndex(deviceIndex),
    mDeviceNbAxes(deviceNbAxes),
    mDeviceNbHats(deviceNbHats),
    mDeviceNbButtons(deviceNbButtons),
    mConfigurationBits(0),
    mPreviousHatsValues{},
    mPreviousAxisValues{},
    mConfiguring(false)
{
  memset(mPreviousAxisValues, 0, sizeof(mPreviousAxisValues));
  memset(mPreviousHatsValues, 0, sizeof(mPreviousHatsValues));
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

String InputDevice::NameExtented() const
{
  String result(mDeviceName);
  std::string powerLevel = PowerLevel();
  if (!powerLevel.empty())
    result.Append(' ').Append(powerLevel);
  return result;
}

String InputDevice::PowerLevel() const
{
  if (mDeviceSDL != nullptr)
  {
    SDL_JoystickPowerLevel power = SDL_JoystickCurrentPowerLevel(mDeviceSDL);
    switch (power)
    {
      case SDL_JOYSTICK_POWER_UNKNOWN: return "";
      case SDL_JOYSTICK_POWER_EMPTY:   return "\uF1b5";
      case SDL_JOYSTICK_POWER_LOW:     return "\uF1b1";
      case SDL_JOYSTICK_POWER_MEDIUM:  return "\uF1b8";
      case SDL_JOYSTICK_POWER_FULL:    return "\uF1b7";
      case SDL_JOYSTICK_POWER_MAX:     return "\uF1ba";
      case SDL_JOYSTICK_POWER_WIRED:   return "\uF1b4";
      default: break;
    }
  }
  return "";
}

/*std::string InputDevice::getSysPowerLevel()
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

bool InputDevice::LoadAutoConfiguration(const std::string& configuration)
{
  bool result = false;

  { LOG(LogInfo) << "[InputDevice] Autoconfiguration from " << configuration; }
  Strings::Vector mappingList = Strings::Split(configuration, ',');
  for(const std::string& mapping : mappingList)
  {
    size_t pos = mapping.find(':');
    if (pos == std::string::npos) continue;
    std::string key = mapping.substr(0, pos);
    std::string value = mapping.substr(pos + 1);

    static HashMap<std::string, Entry> mapper
    ({
       { "dpup",          Entry::Up },
       { "dpright",       Entry::Right },
       { "dpdown",        Entry::Down },
       { "dpleft",        Entry::Left },
       { "a",             Entry::B }, // A <=> B and X <=> Y to match ES buttons.
       { "b",             Entry::A },
       { "x",             Entry::Y },
       { "y",             Entry::X },
       { "back",          Entry::Select },
       { "start",         Entry::Start },
       { "guide",         Entry::Hotkey },
       { "leftshoulder",  Entry::L1 },
       { "rightshoulder", Entry::R1 },
       { "lefttrigger",   Entry::L2 },
       { "righttrigger",  Entry::R2 },
       { "leftstick",     Entry::L3 },
       { "rightstick",    Entry::R3 },
       { "leftx",         Entry::Joy1AxisH },
       { "lefty",         Entry::Joy1AxisV },
       { "rightx",        Entry::Joy2AxisH },
       { "righty",        Entry::Joy2AxisV },
    });

    Entry* entry = mapper.try_get(key);
    if (entry == nullptr || value.length() < 2)
    {
      { LOG(LogError) << "[InputDevice] Unknown mapping: " << mapping; }
      continue;
    }

    // Default values
    InputEvent::EventType typeEnum = InputEvent::EventType::Unknown;
    int id = -1;
    int val = 0;
    int code = -1;
    int sign = 0;
    int parserIndex = 0;

    // Sign?
    switch(value[parserIndex])
    {
      case '-': { sign = -1; ++parserIndex; break; }
      case '+': { sign = +1; ++parserIndex; break; }
      default: break;
    }

    bool parsed = false;
    switch(value[parserIndex])
    {
      case 'a':
      {
        if (Strings::ToInt(value, parserIndex + 1, id))
        {
          typeEnum = InputEvent::EventType::Axis;
          if (sign == 0) val = -1; // non-signed axes are affected to joysticks: always left or up
          else val = sign;         // Otherwise, take the sign as-is
          #ifdef SDL_JOYSTICK_IS_OVERRIDEN_BY_RECALBOX
            code = SDL_JoystickAxisEventCodeById(mDeviceIndex, id);
          #else
            #ifdef _RECALBOX_PRODUCTION_BUILD_
              #pragma GCC error "SDL_JOYSTICK_IS_OVERRIDEN_BY_RECALBOX undefined in production build!"
            #endif
          #endif
          parsed = true;
          { LOG(LogInfo) << "[InputDevice] Auto-Assign " << EntryToString(*entry) << " to Axis " << id << " (Code: " << code << ')'; }
        }
        break;
      }
      case 'b':
      {
        if (Strings::ToInt(value, parserIndex + 1, id))
        {
          typeEnum = InputEvent::EventType::Button;
          val = 1;
          #ifdef SDL_JOYSTICK_IS_OVERRIDEN_BY_RECALBOX
          code = SDL_JoystickButtonEventCodeById(mDeviceIndex, id);
          #else
            #ifdef _RECALBOX_PRODUCTION_BUILD_
              #pragma GCC error "SDL_JOYSTICK_IS_OVERRIDEN_BY_RECALBOX undefined in production build!"
            #endif
          #endif
          parsed = true;
          { LOG(LogInfo) << "[InputDevice] Auto-Assign " << EntryToString(*entry) << " to Button " << id << " (Code: " << code << ')'; }
        }
        break;
      }
      case 'h':
      {
        if (Strings::ToInt(value, parserIndex + 1, '.', id))
          if (Strings::ToInt(value, parserIndex + 3, val))
          {
            typeEnum = InputEvent::EventType::Hat;
            #ifdef SDL_JOYSTICK_IS_OVERRIDEN_BY_RECALBOX
            code = SDL_JoystickHatEventCodeById(mDeviceIndex, id);
            #else
              #ifdef _RECALBOX_PRODUCTION_BUILD_
                #pragma GCC error "SDL_JOYSTICK_IS_OVERRIDEN_BY_RECALBOX undefined in production build!"
              #endif
            #endif
            parsed = true;
            { LOG(LogInfo) << "[InputDevice] Auto-Assign " << EntryToString(*entry) << " to Hat " << id << '/' << val << " (Code: " << code << ')'; }
          }
        break;
      }
      default:
      {
      }
    }
    if (!parsed)
    {
      { LOG(LogError) << "[InputDevice] Error parsing mapping: " << mapping; }
      continue;
    }

    Set(*entry, InputEvent(mDeviceId, typeEnum, id, val, code));
    result = true; // At least one mapping has been done
  }

  // Manage pads with only one analog joystick and no dpad
  if (mInputEvents[(int)Entry::Up].Type() == InputEvent::EventType::Unknown)
    if (mInputEvents[(int)Entry::Right].Type() == InputEvent::EventType::Unknown)
      if (mInputEvents[(int)Entry::Down].Type() == InputEvent::EventType::Unknown)
        if (mInputEvents[(int)Entry::Left].Type() == InputEvent::EventType::Unknown)
          if (mInputEvents[(int)Entry::Joy1AxisH].Type() != InputEvent::EventType::Unknown)
            if (mInputEvents[(int)Entry::Joy1AxisV].Type() != InputEvent::EventType::Unknown)
            {
              const InputEvent& V = mInputEvents[(int)Entry::Joy1AxisV];
              const InputEvent& H = mInputEvents[(int)Entry::Joy1AxisH];

              // Add DPAD configuration
              mInputEvents[(int)Entry::Up] = V;
              mInputEvents[(int)Entry::Down] = InputEvent(mDeviceId, V.Type(), V.Id(), -V.Value(), V.Code());
              mInputEvents[(int)Entry::Left] = H;
              mInputEvents[(int)Entry::Right] = InputEvent(mDeviceId, H.Type(), H.Id(), -H.Value(), H.Code());
              mConfigurationBits |= (1 << (int)Entry::Up) | (1 << (int)Entry::Right) |
                                    (1 << (int)Entry::Down) | (1 << (int)Entry::Left);

              // Remove left analog configuration
              mInputEvents[(int)Entry::Joy1AxisV] = mInputEvents[(int)Entry::Joy1AxisH] = InputEvent();
              mConfigurationBits &= ~((1 << (int)Entry::Joy1AxisV) | (1 << (int)Entry::Joy1AxisH));

              { LOG(LogInfo) << "[InputDevice] No DPAD detected. First analog joystick moved to DPAD"; }
            }

  // Still no HK? Use select
  if (mInputEvents[(int)Entry::Hotkey].Type() == InputEvent::EventType::Unknown)
  {
    { LOG(LogInfo) << "[InputDevice] Not hotkey button guessed. Using SELECT as Hotkey"; }
    mInputEvents[(int) Entry::Hotkey] = mInputEvents[(int) Entry::Select];
  }

  return result;
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
      { LOG(LogError) << "[InputDevice] InputConfig load error - input of type \"" << type << "\" is invalid! Skipping input \"" << name << "\".\n"; }
      continue;
    }

    int id = input.attribute("id").as_int();
    int value = input.attribute("value").as_int();

    if(value == 0)
    { LOG(LogWarning) << "[InputDevice] WARNING: InputConfig value is 0 for " << type << " " << id << "!\n"; }

    // Adapt seamlessly old l1/r1
    if (name == "pageup") name = "l1";
    if (name == "pagedown") name = "r1";
    Entry entry = StringToEntry(name);
    if (entry != Entry::None)
    {
      Set(entry, InputEvent(mDeviceId, typeEnum, id, value));
      loaded++;
    }
    else { LOG(LogError) << "[InputDevice] Unknown Joystick configuration entry: " << name << " of type " << type << "!\n"; }
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
    cfg.append_attribute("deviceName") = mDeviceName;
  }

  cfg.append_attribute("deviceGUID") = GUID().c_str();
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
    input.append_attribute("code").set_value(entry.Code());
  }
}

void InputDevice::ConvertButtonToOnOff(int button, bool pressed, InputCompactEvent::Entry& on,
                                          InputCompactEvent::Entry& off)
{
  for (int i = (int)Entry::__Count; --i >= 0; )
    if ((mConfigurationBits & (1 << (int)i)) != 0)
      if (const InputEvent& config = mInputEvents[i]; config.Type() == InputEvent::EventType::Button)
        if (config.Id() == button)
          (pressed ? on : off) |= ConvertEntry((Entry)i);
}

void InputDevice::ConvertKeyToOnOff(int key, bool pressed, InputCompactEvent::Entry& on,
                                          InputCompactEvent::Entry& off)
{
  for (int i = (int)Entry::__Count; --i >= 0; )
    if ((mConfigurationBits & (1 << (int)i)) != 0)
      if (const InputEvent& config = mInputEvents[i]; config.Type() == InputEvent::EventType::Key)
        if (config.Id() == key)
          (pressed ? on : off) |= ConvertEntry((Entry)i);
}

void InputDevice::ConvertHatToOnOff(int hat, int bits, InputCompactEvent::Entry& on, InputCompactEvent::Entry& off)
{
  int previousBits = mPreviousHatsValues[hat];
  for (int i = (int)Entry::__Count; --i >= 0; )
    if ((mConfigurationBits & (1 << (int)i)) != 0)
      if (const InputEvent& config = mInputEvents[i]; config.Type() == InputEvent::EventType::Hat && config.Id() == hat)
      {
        // Hat bits are matching, set the target direction
        if ((bits & config.Value()) == config.Value())
          on |= ConvertEntry((Entry)i);
        // Otherwise, if previous value matched,
        else if ((previousBits & config.Value()) == config.Value()) // Previous value match bits?
            off |= ConvertEntry((Entry)i);
      }

  // Record previous value
  mPreviousHatsValues[hat] = bits;
}

void InputDevice::ConvertAxisToOnOff(int axis, int value, InputCompactEvent::Entry& on, InputCompactEvent::Entry& off)
{
  for (int i = (int)Entry::__Count; --i >= 0; )
    if ((mConfigurationBits & (1 << (int)i)) != 0)
      if (const InputEvent& config = mInputEvents[i]; config.Type() == InputEvent::EventType::Axis && config.Id() == axis)
      {
        // For axis, we must distinguish target axis from other binary targets.
        // Axis to axis are converted to negative/center/positive.
        // Axis to binaries are converted to on only for the right sign. off otherwise
        // That is, axis on triggers will be properly converted to on/off buttons.
        if (InputCompactEvent::Entry targetEntry = ConvertEntry((Entry)i); targetEntry >= InputCompactEvent::Entry::J1Left)
        {
          InputCompactEvent::Entry targetOpposite = (InputCompactEvent::Entry)((int)targetEntry << 1);
          // Since configured event are negatives, if we got a positive value in current config, that means
          // the joystick is inverted.
          // Note: values are already normalized to -1/0/+1
          value = (config.Value() > 0) ? -value : value;
          int previousValue = (config.Value() > 0) ? -mPreviousAxisValues[axis] : mPreviousAxisValues[axis];
          if (previousValue != value)
          switch(value)
          {
            case -1: // configured axis direction?
            {
              on |= targetEntry;
              if (previousValue > 0) off |= targetOpposite; // In case joystick has been quickly moved from one direction to another
              break;
            }
            case 1: // Opposite of configured axis direction?
            {
              on |= targetOpposite;
              if (previousValue < 0) off |= targetEntry; // In case joystick has been quickly moved from one direction to another
              break;
            }
            default: // Axis centered again
            {
              off |= previousValue < 0 ? targetEntry : targetOpposite;
              break;
            }
          }
        }
        else // Axis has a binary on/off
        {
          // If the config is negative, invert the value so that we just compare to positive range
          value = (config.Value() < 0) ? -value : value;
          if (value > 0) on |= targetEntry;
          else off |= targetEntry;
        }
      }

  // Record previous value
  mPreviousAxisValues[axis] = value;
}


InputCompactEvent InputDevice::ConvertToCompact(const InputEvent& event)
{
  //{ LOG(LogError) << "[InputEvent] " << event.ToString(); }
  InputCompactEvent::Entry on  = InputCompactEvent::Entry::Nothing;
  InputCompactEvent::Entry off = InputCompactEvent::Entry::Nothing;

  // Need configuration?
  if (!IsConfigured()) off = (on |= InputCompactEvent::Entry::NeedConfiguration);

  switch(event.Type())
  {
    case InputEvent::EventType::Axis: ConvertAxisToOnOff(event.Id(), event.Value(), on, off); break;
    case InputEvent::EventType::Hat: ConvertHatToOnOff(event.Id(), event.Value(), on, off); break;
    case InputEvent::EventType::Button: ConvertButtonToOnOff(event.Id(), event.Value() != 0, on, off); break;
    case InputEvent::EventType::Key: ConvertKeyToOnOff(event.Id(), event.Value() != 0, on, off); break;
    case InputEvent::EventType::Unknown:
    default: { LOG(LogError) << "[InputDevice] Abnormal InputEvent::EventType: " << (int)event.Type(); break; }
  }

  InputCompactEvent result = { on, off, *this, event };
  //{ LOG(LogError) << "[CompactInputEvent] " << result.ToString(); }
  return result;
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
    case Entry::VolumeUp: return InputCompactEvent::Entry::VolUp;
    case Entry::VolumeDown: return InputCompactEvent::Entry::VolDown;
    case Entry::BrightnessUp: return InputCompactEvent::Entry::LumUp;
    case Entry::BrightnessDown: return InputCompactEvent::Entry::LumDown;
    case Entry::None:
    case Entry::__Count:
    default: break;
  }
  { LOG(LogError) << "[InputDevice] Unknown entry! " << (int)entry; }
  return InputCompactEvent::Entry::Nothing;
}

bool InputDevice::EqualsTo(const InputDevice& to) const
{
  return (mDeviceName == to.mDeviceName) &&
         (memcmp(&mDeviceGUID, &to.mDeviceGUID, sizeof(SDL_JoystickGUID)) == 0) &&
         (mDeviceNbAxes == to.mDeviceNbAxes) &&
         (mDeviceNbHats == to.mDeviceNbHats) &&
         (mDeviceNbButtons == to.mDeviceNbButtons)
         ;
}

