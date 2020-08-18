#include "InputManager.h"
#include "InputDevice.h"
#include "Settings.h"
#include "utils/Log.h"
#include "pugixml/pugixml.hpp"
#include "RootFolders.h"
#include "Input.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_joystick.h"
#include "Window.h"
#include <algorithm>
#include <guis/GuiInfoPopup.h>
#include <utils/locale/LocaleHelper.h>

#define KEYBOARD_GUID_STRING { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
#define EMPTY_GUID_STRING { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

InputManager::InputManager()
  : mIndexToId {}
  , mKeyboard(nullptr, InputEvent::sKeyboardDevice, -1, "Keyboard", KEYBOARD_GUID_STRING, 0, 0, 125)
{
  // Create keyboard
  LoadDefaultKeyboardConfiguration();
}

InputManager::~InputManager()
{
  Finalize();
}

InputManager& InputManager::Instance()
{
  static InputManager _instance;
  return _instance;
}

InputDevice& InputManager::GetDeviceConfigurationFromId(SDL_JoystickID deviceId)
{
  // Already exists?
  InputDevice* device = mIdToDevices.try_get(deviceId);
  if (device != nullptr)
    return *device;

  LOG(LogError) << "Unexisting device!";
  static InputDevice sEmptyDevice(nullptr, InputEvent::sEmptyDevice, -1, "Empty Device", EMPTY_GUID_STRING, 0, 0, 0);
  return sEmptyDevice;
}

void InputManager::Finalize()
{
  if (!IsInitialized()) return;

  ClearAllConfigurations();

  SDL_JoystickEventState(SDL_DISABLE);
  SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}

void InputManager::Initialize(Window& window, bool padplugged)
{
  std::vector<InputDevice> previousList = BuildCurrentDeviceList();

  Finalize();

  SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS,
              Settings::Instance().BackgroundJoystickInput() ? "1" : "0");
  SDL_InitSubSystem(SDL_INIT_JOYSTICK);
  SDL_JoystickEventState(SDL_ENABLE);

  ClearAllConfigurations();
  LoadAllJoysticksConfiguration(previousList, window, padplugged);
}

void InputManager::LoadDefaultKeyboardConfiguration()
{
  // Load default configuration
  mKeyboard.ClearAll();
  mKeyboard.Set(InputDevice::Entry::Up, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_UP, 1));
  mKeyboard.Set(InputDevice::Entry::Down, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_DOWN, 1));
  mKeyboard.Set(InputDevice::Entry::Left, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_LEFT, 1));
  mKeyboard.Set(InputDevice::Entry::Right, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_RIGHT, 1));

  mKeyboard.Set(InputDevice::Entry::A, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_x, 1));
  mKeyboard.Set(InputDevice::Entry::B, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_z, 1));
  mKeyboard.Set(InputDevice::Entry::X, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_a, 1));
  mKeyboard.Set(InputDevice::Entry::Y, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_a, 1));
  mKeyboard.Set(InputDevice::Entry::Start, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_RETURN, 1));
  mKeyboard.Set(InputDevice::Entry::Select, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_RSHIFT, 1));

  mKeyboard.Set(InputDevice::Entry::L1, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_q, 1));
  mKeyboard.Set(InputDevice::Entry::R1, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_w, 1));

  mKeyboard.Set(InputDevice::Entry::Hotkey, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_RCTRL, 1));

  //WriteDeviceXmlConfiguration(mKeyboard);

  // Load configuration
  LookupDeviceXmlConfiguration(mKeyboard);
}

void InputManager::ClearAllConfigurations()
{
  // Close SDL devices
  for (auto& mJoystick : mIdToSdlJoysticks)
    SDL_JoystickClose(mJoystick.second);
  mIdToSdlJoysticks.clear();

  // Delete InputDevices
  mIdToDevices.clear();
}

std::vector<InputDevice> InputManager::BuildCurrentDeviceList()
{
  std::vector<InputDevice> result;
  for(const auto& item : mIdToDevices)
    result.push_back(item.second);
  std::sort(result.begin(), result.end(), [] (const InputDevice& a, const InputDevice& b)
  {
    int r = strcmp(a.RawName(), b.RawName());
    if (r < 0) return true;
    r =  memcmp(&a.RawGUID(), &b.RawGUID(), sizeof(SDL_JoystickGUID));
    if (r < 0) return true;
    if (a.AxeCount() < b.AxeCount()) return true;
    if (a.HatCount() < b.HatCount()) return true;
    return (a.ButtonCount() < b.ButtonCount());
  });

  return result;
}

void InputManager::LoadAllJoysticksConfiguration(std::vector<InputDevice> previous, Window& window, bool padplugged)
{
  int numJoysticks = SDL_NumJoysticks();
  for (int i = 0; i < numJoysticks; i++)
    LoadJoystickConfiguration(i);

  std::vector<InputDevice> current = BuildCurrentDeviceList();

  if (padplugged)
  {
    if (current.size() > previous.size())
    {
      while (current.size() > previous.size())
      {
        // Joystick added
        int index = 0;
        while (index < (int) previous.size() && previous[index].EqualsTo(current[index]))
          index++;

        // Build the text
        std::string text = current[index].Name();
        text.append(1, ' ');
        text.append(_(" has been plugged!"));
        text.append("\n\n");
        if (current[index].IsConfigured()) text.append(_("Ready to play!"));
        else text.append(_("Not configured yet! Press a button to enter the configuration window."));
        current.erase(current.begin() + index);

        GuiInfoPopup* popup = new GuiInfoPopup(window, text, 10, GuiInfoPopup::PopupType::Pads);
        window.AddInfoPopup(popup);
      }
    }
    else
    {
      while (current.size() < previous.size())
      {
        // Joystick removed
        int index = 0;
        while (index < (int) current.size() && previous[index].EqualsTo(current[index]))
          index++;

        // Build the text
        std::string text = previous[index].Name();
        text.append(1, ' ');
        text.append(_(" has been unplugged!"));
        previous.erase(previous.begin() + index);

        GuiInfoPopup* popup = new GuiInfoPopup(window, text, 10, GuiInfoPopup::PopupType::Pads);
        window.AddInfoPopup(popup);
      }
    }
  }
}

std::string InputManager::DeviceGUIDString(SDL_Joystick* joystick)
{
  char guid[128];
  SDL_JoystickGetGUIDString(SDL_JoystickGetGUID(joystick), guid, sizeof(guid));
  return guid;
}

void InputManager::LoadJoystickConfiguration(int index)
{
  // Open joystick & add to our list
  SDL_Joystick* joy = SDL_JoystickOpen(index);
  if (joy == nullptr) return;
  SDL_JoystickID identifier = SDL_JoystickInstanceID(joy);
  mIdToSdlJoysticks[identifier] = joy;
  mIndexToId[index] = identifier;

  // Create device configuration
  InputDevice device(joy,
                     identifier,
                     index,
                     SDL_JoystickName(joy),
                     SDL_JoystickGetGUID(joy),
                     SDL_JoystickNumAxes(joy),
                     SDL_JoystickNumHats(joy),
                     SDL_JoystickNumButtons(joy));

  // Try to load from configuration file
  if (!LookupDeviceXmlConfiguration(device))
  {
    bool autoConfigured = false;
    SDL_GameController* gamepad = SDL_GameControllerOpen(index);
    if (gamepad != nullptr)
    {
      // Try to autoconfigure the gamepad
      char* sdlMapping = SDL_GameControllerMappingForGUID(SDL_JoystickGetGUID(joy));
      std::string conf(sdlMapping != nullptr ? sdlMapping : "");
      SDL_free(sdlMapping);
      autoConfigured = device.LoadAutoConfiguration(conf);
      SDL_GameControllerClose(gamepad);
      // Write configuration so that the configgen can read it
      if (autoConfigured)
        WriteDeviceXmlConfiguration(device);
    }

    if (!autoConfigured)
      LOG(LogWarning) << "Unknown joystick " << SDL_JoystickName(joy)
                      << " (GUID: " << DeviceGUIDString(joy) << ", Instance ID: " << identifier
                      << ", Device Index: " << index
                      << ", Axis: " << SDL_JoystickNumAxes(joy)
                      << ", Hats: " << SDL_JoystickNumHats(joy)
                      << ", Buttons: " << SDL_JoystickNumButtons(joy) << ")";
  }

  // Store
  mIdToDevices[identifier] = device;
  LOG(LogWarning) << "Added joystick " << SDL_JoystickName(joy)
                  << " (GUID: " << DeviceGUIDString(joy) << ", Instance ID: " << identifier
                  << ", Device Index: " << index
                  << ", Axis: " << SDL_JoystickNumAxes(joy)
                  << ", Hats: " << SDL_JoystickNumHats(joy)
                  << ", Buttons: " << SDL_JoystickNumButtons(joy) << ")";

}

int InputManager::ConfiguredDeviceCount()
{
  int num = mKeyboard.IsConfigured() ? 1 : 0;
  for (auto& mInputConfig : mIdToDevices)
    if (mInputConfig.second.IsConfigured())
      num++;

  return num;
}

InputCompactEvent InputManager::ManageAxisEvent(const SDL_JoyAxisEvent& axis)
{
  // Normalize value
  int value = (axis.value < 0) ?
              (axis.value < -sJoystickDeadZone ? -1 : 0) :
              (axis.value > sJoystickDeadZone ? 1 : 0) ;

  // Check if the axis enter or exit from the dead area
  InputDevice& device = GetDeviceConfigurationFromId(axis.which);
  InputEvent event(axis.which, InputEvent::EventType::Axis, axis.axis, value);
  if (value != device.PreviousAxisValues(axis.axis))
  {
    device.SetPreviousAxisValues(axis.axis, value);
    return device.ConvertToCompact(event);
  }
  return { InputCompactEvent::Entry::Nothing, InputCompactEvent::Entry::Nothing, device, event };
}

InputCompactEvent InputManager::ManageButtonEvent(const SDL_JoyButtonEvent& button)
{
  InputDevice& device = GetDeviceConfigurationFromId(button.which);
  return device.ConvertToCompact(InputEvent(button.which, InputEvent::EventType::Button, button.button, button.state == SDL_PRESSED ? 1 : 0));
}

InputCompactEvent InputManager::ManageHatEvent(const SDL_JoyHatEvent& hat)
{
  InputDevice& device = GetDeviceConfigurationFromId(hat.which);
  return device.ConvertToCompact(InputEvent(hat.which, InputEvent::EventType::Hat, hat.hat, hat.value));
}

InputCompactEvent InputManager::ManageKeyEvent(const SDL_KeyboardEvent& key, bool down)
{
  InputEvent event = InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, key.keysym.sym, down ? 1 : 0);
  if (down)
  {
    // Ignore repeat events
    if (key.repeat != 0u) return {InputCompactEvent::Entry::Nothing, InputCompactEvent::Entry::Nothing, mKeyboard, event };

    // Quit?
    if (key.keysym.sym == SDLK_F4)
    {
      SDL_Event quit;
      quit.type = SDL_QUIT;
      SDL_PushEvent(&quit);
      return {InputCompactEvent::Entry::Nothing, InputCompactEvent::Entry::Nothing, mKeyboard, event };
    }

    if (event.Value() != 0 && Settings::Instance().Debug())
    {
      // toggle debug grid with Ctrl-G
      if (event.Id() == SDLK_g && ((SDL_GetModState() & KMOD_LCTRL) != 0))
        Settings::Instance().SetDebugGrid(!Settings::Instance().DebugGrid());
      // toggle TextComponent debug view with Ctrl-T
      else if (event.Id() == SDLK_t && ((SDL_GetModState() & KMOD_LCTRL) != 0))
        Settings::Instance().SetDebugText(!Settings::Instance().DebugText());
    }
  }
  return mKeyboard.ConvertToCompact(event);
}

InputCompactEvent InputManager::ManageSDLEvent(Window& window, const SDL_Event& ev)
{
  switch (ev.type)
  {
    case SDL_JOYAXISMOTION: return ManageAxisEvent(ev.jaxis);
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP: return ManageButtonEvent(ev.jbutton);
    case SDL_JOYHATMOTION: return ManageHatEvent(ev.jhat);
    case SDL_KEYDOWN:
    case SDL_KEYUP: return ManageKeyEvent(ev.key, ev.type == SDL_KEYDOWN);
    case SDL_JOYDEVICEADDED:
    case SDL_JOYDEVICEREMOVED:
    {
      LOG(LogInfo) << "Reinitialize because of joystick added/removed.";
      Initialize(window, true);
      break;
    }
  }

  return {InputCompactEvent::Entry::Nothing, InputCompactEvent::Entry::Nothing, mKeyboard, InputEvent() };
}

Path InputManager::ConfigurationPath()
{
  return RootFolders::DataRootFolder / "system/.emulationstation/es_input.cfg";
}

bool InputManager::LookupDeviceXmlConfiguration(InputDevice& device)
{
  Path path = ConfigurationPath();
  if (!path.Exists()) return false;

  pugi::xml_document doc;
  pugi::xml_parse_result res = doc.load_file(path.ToChars());
  if (!res)
  {
    LOG(LogError) << "Error parsing input config: " << res.description();
    return false;
  }

  LOG(LogDebug) << "Looking for configuration for " << device.Name() << " (UUID: " << device.GUID()
                << ") - Axis: " << device.AxeCount()
                << " - Hats: " << device.HatCount()
                << " - Buttons: " << device.ButtonCount();

  pugi::xml_node root = doc.child("inputList");
  if (root != nullptr)
    for (pugi::xml_node item = root.child("inputConfig"); item != nullptr; item = item.next_sibling("inputConfig"))
    {
      LOG(LogDebug) << "Trying " << item.attribute("deviceGUID").value()
                    << " (UUID: " << item.attribute("deviceName").value()
                    << ") - Axis: " << item.attribute("deviceNbAxes").as_int()
                    << " - Hats: " << item.attribute("deviceNbHats").as_int()
                    << " - Buttons: " << item.attribute("deviceNbButtons").as_int();
      // check the guid
      bool guid    = (strcmp(device.GUID().c_str(), item.attribute("deviceGUID").value()) == 0) || device.IsKeyboard();
      bool name    = strcmp(device.Name().c_str(), item.attribute("deviceName").value()) == 0;
      bool axes    = (device.AxeCount() == item.attribute("deviceNbAxes").as_int()) || device.IsKeyboard();
      bool hats    = (device.HatCount() == item.attribute("deviceNbHats").as_int()) || device.IsKeyboard();
      bool buttons = (device.ButtonCount() == item.attribute("deviceNbButtons").as_int()) || device.IsKeyboard();
      if (guid && name && axes && hats && buttons)
      {
        int loaded = device.LoadFromXml(item);
        LOG(LogInfo) << "Loaded " << loaded << " configuration entries for device " << device.Name();
        return true;
      }
    }
  return false;
}

void InputManager::WriteDeviceXmlConfiguration(InputDevice& device)
{
  Path path = ConfigurationPath();
  pugi::xml_document doc;
  if (path.Exists())
  {
    pugi::xml_parse_result result = doc.load_file(path.ToChars());
    if (!result)
    {
      LOG(LogError) << "Error parsing input config: " << result.description();
    }
    else
    {
      // successfully loaded, delete the old entry if it exists
      pugi::xml_node root = doc.child("inputList");
      if (root != nullptr)
        for (pugi::xml_node item = root.child("inputConfig"); item != nullptr; item = item.next_sibling("inputConfig"))
          if (strcmp(device.GUID().c_str(), item.attribute("deviceGUID").value()) == 0 &&
              strcmp(device.Name().c_str(), item.attribute("deviceName").value()) == 0 &&
              device.AxeCount() == item.attribute("deviceNbAxes").as_int() &&
              device.HatCount() == item.attribute("deviceNbHats").as_int() &&
              device.ButtonCount() == item.attribute("deviceNbButtons").as_int())
          {
            root.remove_child(item);
            break;
          }
    }
  }

  pugi::xml_node root = doc.child("inputList");
  if (!root) root = doc.append_child("inputList");

  device.SaveToXml(root);
  doc.save_file(path.ToChars());
}

void InputManager::FillConfiguredDevicelist(InputDeviceList& list)
{
  for(auto& item : mIdToDevices)
    if (item.second.IsConfigured())
      list.Add(item.second);
}

bool InputManager::LookupDevice(InputDeviceList& list, const std::string& guid, const std::string& name, InputDevice& output)
{
  for(int i = list.Count(); --i>= 0; )
  {
    const InputDevice& device = list[i];
    if (device.IsConfigured())
      if (guid == device.GUID() && name == device.Name())
      {
        output = device;
        list.Delete(i);
        return true;
      }
  }
  return false;
}

bool InputManager::LookupDevice(InputDeviceList& list, const std::string& name, InputDevice& output)
{
  for(int i = list.Count(); --i>= 0; )
  {
    const InputDevice& device = list[i];
    if (device.IsConfigured())
      if (name == device.Name())
      {
        output = device;
        list.Delete(i);
        return true;
      }
  }
  return false;
}

bool InputManager::LookupDevice(InputDeviceList& list, InputDevice& output)
{
  if (!list.Empty())
  {
    output = list[0];
    list.Delete(0);
    return true;
  }
  return false;
}

OrderedDevices InputManager::GenerateConfiguration()
{
  InputDevice device;
  OrderedDevices devices;
  InputDeviceList list;
  FillConfiguredDevicelist(list);

  // First loop, search for GUID + NAME. High Priority
  for (int player = 0; player < Input::sMaxInputDevices; ++player)
  {
    std::string playerConfigName = Settings::Instance().InputName(player);
    std::string playerConfigGuid = Settings::Instance().InputGuid(player);

    if (LookupDevice(list, playerConfigGuid, playerConfigName, device))
    {
      devices.SetDevice(player, device);
      LOG(LogInfo) << "Set " << device.Name() << " for player " << player << " (configured Name/Guid)";
    }
  }
  // Second loop, search for NAME. High Priority
  for (int player = 0; player < Input::sMaxInputDevices; ++player)
    if (!devices.IsConfigured(player))
    {
      std::string playerConfigName = Settings::Instance().InputName(player);

      if (LookupDevice(list, playerConfigName, device))
      {
        devices.SetDevice(player, device);
        LOG(LogInfo) << "Set " << device.Name() << " for player " << player << " (configured Name only)";
      }
    }
  // Last loop, search for free controllers for remaining players.
  for (int player = 0; player < Input::sMaxInputDevices; ++player)
    if (!devices.IsConfigured(player))
    {
      if (LookupDevice(list, device))
      {
        devices.SetDevice(player, device);
        LOG(LogInfo) << "Set " << device.Name() << " for player " << player << " (default)";
      }
    }

  // Shrink configuration so that there is no hole
  devices.Shrink();

  return devices;
}

std::string InputManager::GenerateConfiggenConfiguration(const OrderedDevices& devices)
{
  std::string command;
  for (int player = 0; player < Input::sMaxInputDevices; ++player)
  {
    if (devices.IsConfigured(player))
    {
      const InputDevice& device = devices.Device(player);
      std::string p(" -p"); p.append(Strings::ToString(player + 1));
      command.append(p).append("index ").append(Strings::ToString(device.Index()))
             .append(p).append("guid ").append(device.GUID())
             .append(p).append("name \"").append(device.Name() + "\"")
             .append(p).append("nbaxes ").append(Strings::ToString(device.AxeCount()))
             .append(p).append("nbhats ").append(Strings::ToString(device.HatCount()))
             .append(p).append("nbbuttons ").append(Strings::ToString(device.ButtonCount()))
             #ifdef SDL_JOYSTICK_IS_OVERRIDEN_BY_RECALBOX
               .append(p).append("devicepath ").append(SDL_JoystickDevicePathById(device.Index()))
             #else
               #ifdef _RECALBOX_PRODUCTION_BUILD_
                 #pragma GCC error "SDL_JOYSTICK_IS_OVERRIDEN_BY_RECALBOX undefined in production build!"
               #endif
             #endif
             ;
    }
  }
  LOG(LogInfo) << "Configure emulators command : " << command;
  return command;
}

void InputManager::LogRawEvent(const InputEvent& event)
{
  LOG(LogDebug) << "Raw Event: " << event.ToString();
}

void InputManager::LogCompactEvent(const InputCompactEvent& event)
{
  LOG(LogDebug) << "Compact Event: " << event.ToString();
}


