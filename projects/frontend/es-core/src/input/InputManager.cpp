#include "InputManager.h"
#include "InputDevice.h"
#include "Settings.h"
#include "utils/Log.h"
#include "pugixml/pugixml.hpp"
#include "RootFolders.h"
#include "SDL.h"
#include "Input.h"

#define KEYBOARD_GUID_STRING { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }

InputManager::InputManager()
  : mKeyboard(InputEvent::sKeyboardDevice, -1, "Keyboard", KEYBOARD_GUID_STRING, 0, 0, 125)
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

InputDevice& InputManager::GetDeviceConfiguration(int deviceId)
{
  // Already exists?
  InputDevice* device = mDevices.try_get(deviceId);
  if (device != nullptr)
    return *device;

  LOG(LogError) << "Unexisting device!";
  return mKeyboard;
}

void InputManager::Finalize()
{
  if (!IsInitialized()) return;

  ClearAllConfigurations();

  SDL_JoystickEventState(SDL_DISABLE);
  SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
}

void InputManager::Initialize()
{
  if (IsInitialized()) return;

  Finalize();

  SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS,
              Settings::Instance().BackgroundJoystickInput() ? "1" : "0");
  SDL_InitSubSystem(SDL_INIT_JOYSTICK);
  SDL_JoystickEventState(SDL_ENABLE);

  ClearAllConfigurations();
  LoadAllJoysticksConfiguration();
}

void InputManager::LoadDefaultKeyboardConfiguration()
{
  // Load default configuration
  mKeyboard.ClearAll();
  mKeyboard.Set(InputDevice::Entry::Up, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_UP, 1));
  mKeyboard.Set(InputDevice::Entry::Down, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_DOWN, 1));
  mKeyboard.Set(InputDevice::Entry::Left, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_LEFT, 1));
  mKeyboard.Set(InputDevice::Entry::Right, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_RIGHT, 1));

  mKeyboard.Set(InputDevice::Entry::A, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_RETURN, 1));
  mKeyboard.Set(InputDevice::Entry::B, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_ESCAPE, 1));
  mKeyboard.Set(InputDevice::Entry::Start, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_F1, 1));
  mKeyboard.Set(InputDevice::Entry::Select, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_F2, 1));

  mKeyboard.Set(InputDevice::Entry::L1, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_RIGHTBRACKET, 1));
  mKeyboard.Set(InputDevice::Entry::R1, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_LEFTBRACKET, 1));

  // Load configuration
  LookupDeviceXmlConfiguration(mKeyboard);
}

void InputManager::ClearAllConfigurations()
{
  // Close SDL devices
  for (auto& mJoystick : mJoysticks)
    SDL_JoystickClose(mJoystick.second);
  mJoysticks.clear();

  // Delete InputDevices
  mDevices.clear();
}

void InputManager::LoadAllJoysticksConfiguration()
{
  int numJoysticks = SDL_NumJoysticks();
  for (int i = 0; i < numJoysticks; i++)
  {
    LoadJoystickConfiguration(i);
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
  mJoysticks[identifier] = joy;

  // Create device configuration
  InputDevice device(identifier,
                     index,
                     SDL_JoystickName(joy),
                     SDL_JoystickGetGUID(joy),
                     SDL_JoystickNumAxes(joy),
                     SDL_JoystickNumHats(joy),
                     SDL_JoystickNumButtons(joy));

  // Try to load from configuration file
  if (!LookupDeviceXmlConfiguration(device))
    LOG(LogWarning) << "Unknown joystick " << SDL_JoystickName(joy);

  // Store
  mDevices[identifier] = device;
  LOG(LogWarning) << "Added joystick " << SDL_JoystickName(joy) << " (GUID: " << DeviceGUIDString(joy) << ", instance ID: " << identifier << ", device index: " << index << ")";

}

int InputManager::ConfiguredDeviceCount()
{
  int num = mKeyboard.IsConfigured() ? 1 : 0;
  for (auto& mInputConfig : mDevices)
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
  InputDevice& device = GetDeviceConfiguration(axis.which);
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
  InputDevice& device = GetDeviceConfiguration(button.which);
  return device.ConvertToCompact(InputEvent(button.which, InputEvent::EventType::Button, button.button, button.state == SDL_PRESSED ? 1 : 0));
}

InputCompactEvent InputManager::ManageHatEvent(const SDL_JoyHatEvent& hat)
{
  InputDevice& device = GetDeviceConfiguration(hat.which);
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

InputCompactEvent InputManager::ManageSDLEvent(const SDL_Event& ev)
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
      Initialize();
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

  pugi::xml_node root = doc.child("inputList");
  if (root != nullptr)
    for (pugi::xml_node item = root.child("inputConfig"); item != nullptr; item = item.next_sibling("inputConfig"))
    {
      // check the guid
      bool guid = strcmp(device.GUID().c_str(), item.attribute("deviceGUID").value()) == 0;
      bool name = strcmp(device.Name().c_str(), item.attribute("deviceName").value()) == 0;
      if (guid || name)
      {
        if (!name) LOG(LogInfo) << "Approximative device found using guid=" << item.attribute("deviceGUID").value()
                                << " name=" << item.attribute("deviceName").value() << ")";
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
              strcmp(device.Name().c_str(), item.attribute("deviceName").value()) == 0)
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
  for(auto& item : mDevices)
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
      std::string p = " -p" + std::to_string(player + 1);
      command.append(p + "index " + std::to_string(device.Index()));
      command.append(p + "guid " + device.GUID());
      command.append(p + "name \"" + device.Name() + "\"");
      command.append(p + "nbaxes " + std::to_string(device.AxeCount()));

      #if defined(SDL_JOYSTICK_IS_OVERRIDEN_BY_RECALBOX) || defined(_RPI_)
        command.append(p + "devicepath " + SDL_JoystickDevicePathById(device.Index()));
      #endif
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

