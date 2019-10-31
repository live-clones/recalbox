#include "InputManager.h"
#include "InputDevice.h"
#include "Settings.h"
#include "Log.h"
#include "pugixml/pugixml.hpp"
#include <boost/filesystem.hpp>
#include "platform.h"
#include "RootFolders.h"
#include "SDL.h"

#define KEYBOARD_GUID_STRING "-1"

namespace fs = boost::filesystem;

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

InputDevice* InputManager::GetDeviceConfiguration(int deviceId)
{
  // Already exists?
  auto result =  mDevices.find(deviceId);
  if (result != mDevices.end())
    return result->second;

  LOG(LogError) << "Unexisting device!";
  return nullptr;
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
  if (IsInitialized())
    Finalize();

  SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS,
              Settings::getInstance()->getBool("BackgroundJoystickInput") ? "1" : "0");
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
  for (auto& mInputConfig : mDevices)
    delete mInputConfig.second;
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
  mDevices[identifier] = new InputDevice(identifier,
                                         index,
                                         SDL_JoystickName(joy),
                                         DeviceGUIDString(joy),
                                         SDL_JoystickNumAxes(joy),
                                         SDL_JoystickNumHats(joy),
                                         SDL_JoystickNumButtons(joy));
  LOG(LogWarning) << "Added joystick " << SDL_JoystickName(joy) << " (GUID: " << DeviceGUIDString(joy) << ", instance ID: " << identifier << ", device index: " << index << ")";
  // Try to load from configuration file
  if (!LookupDeviceXmlConfiguration(*mDevices[identifier])) LOG(LogWarning) << "Unknown joystick " << SDL_JoystickName(joy);
}

int InputManager::ConfiguredDeviceCount()
{
  int num = 0;
  for (auto& mInputConfig : mDevices)
    if (mInputConfig.second->IsConfigured())
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
  InputDevice* device = GetDeviceConfiguration(axis.which);
  InputEvent event(axis.which, InputEvent::EventType::Axis, axis.axis, value);
  if (value != device->PreviousAxisValues(axis.axis))
  {
    device->SetPreviousAxisValues(axis.axis, value);
    return device->ConvertToCompact(event);
  }
  return {InputCompactEvent::Entry::Nothing, InputCompactEvent::Entry::Nothing, *device, event };
}

InputCompactEvent InputManager::ManageButtonEvent(const SDL_JoyButtonEvent& button)
{
  InputDevice* device = GetDeviceConfiguration(button.which);
  return device->ConvertToCompact(InputEvent(button.which, InputEvent::EventType::Button, button.button, button.state == SDL_PRESSED ? 1 : 0));
}

InputCompactEvent InputManager::ManageHatEvent(const SDL_JoyHatEvent& hat)
{
  InputDevice* device = GetDeviceConfiguration(hat.which);
  return device->ConvertToCompact(InputEvent(hat.which, InputEvent::EventType::Hat, hat.hat, hat.value));
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

    if (event.Value() != 0 && Settings::getInstance()->getBool("Debug"))
    {
      // toggle debug grid with Ctrl-G
      if (event.Id() == SDLK_g && ((SDL_GetModState() & KMOD_LCTRL) != 0))
        Settings::getInstance()->setBool("DebugGrid", !Settings::getInstance()->getBool("DebugGrid"));
      // toggle TextComponent debug view with Ctrl-T
      else if (event.Id() == SDLK_t && ((SDL_GetModState() & KMOD_LCTRL) != 0))
        Settings::getInstance()->setBool("DebugText", !Settings::getInstance()->getBool("DebugText"));
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

std::string InputManager::ConfigurationPath()
{
  std::string path = RootFolders::DataRootFolder + "/system/.emulationstation/es_input.cfg";
  return path;
}

bool InputManager::LookupDeviceXmlConfiguration(InputDevice& device)
{
  std::string path = ConfigurationPath();
  if (!fs::exists(path)) return false;

  pugi::xml_document doc;
  pugi::xml_parse_result res = doc.load_file(path.c_str());
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
  std::string path = ConfigurationPath();
  pugi::xml_document doc;
  if (fs::exists(path))
  {
    pugi::xml_parse_result result = doc.load_file(path.c_str());
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
  doc.save_file(path.c_str());
}

void InputManager::FillConfiguredDevicelist(std::vector<InputDevice*>& list)
{
  for(auto& item : mDevices)
  {
    InputDevice& device = *item.second;
    if (device.IsConfigured())
      list.push_back(&device);
  }
}

InputDevice* InputManager::LookupDevice(InputDeviceList& list, const std::string& guid, const std::string& name)
{
  for(auto device : list)
  {
    if (device->IsConfigured())
      if (guid == device->GUID() && name == device->Name())
      {
        list.erase(std::find(list.begin(), list.end(), device)); // ...
        return device;
      }
  }
  return nullptr;
}

InputDevice* InputManager::LookupDevice(InputDeviceList& list, const std::string& name)
{
  for(auto device : list)
  {
    if (device->IsConfigured())
      if (name == device->Name())
      {
        list.erase(std::find(list.begin(), list.end(), device)); // ...
        return device;
      }
  }
  return nullptr;
}

InputDevice* InputManager::LookupDevice(InputDeviceList& list)
{
  if (!list.empty())
  {
    InputDevice* device = list[0];
    list.erase(list.begin()); // ...
    return device;
  }
  return nullptr;
}

std::string InputManager::GenerateConfiggenConfiguration()
{
  InputDeviceList list;
  FillConfiguredDevicelist(list);

  InputDevice* devices[InputEvent::sMaxPlayers];
  for(int i = InputEvent::sMaxPlayers; --i >= 0;)
    devices[i] = nullptr;

  // First loop, search for GUID + NAME. High Priority
  for (int player = 0; player < InputEvent::sMaxPlayers; ++player)
  {
    std::string sstm("INPUT P"); sstm += std::to_string(player + 1);
    std::string playerConfigName = Settings::getInstance()->getString(sstm + "NAME");
    std::string playerConfigGuid = Settings::getInstance()->getString(sstm + "GUID");

    InputDevice* device = LookupDevice(list, playerConfigGuid, playerConfigName);
    if (device != nullptr)
    {
      devices[player] = device;
      LOG(LogInfo) << "Saved " << device->Name() << " for player " << player;
      break;
    }
  }
  // Second loop, search for NAME. High Priority
  for (int player = 0; player < InputEvent::sMaxPlayers; ++player)
    if (devices[player] == nullptr)
    {
      std::string sstm("INPUT P"); sstm += std::to_string(player + 1);
      std::string playerConfigName = Settings::getInstance()->getString(sstm + "NAME");

      InputDevice* device = LookupDevice(list, playerConfigName);
      if (device != nullptr)
      {
        devices[player] = device;
        LOG(LogInfo) << "Saved " << device->Name() << " for player " << player;
        break;
      }
    }
  // Last loop, search for free controllers for remaining players.
  for (int player = 0; player < InputEvent::sMaxPlayers; ++player)
    if (devices[player] == nullptr)
    {
      LOG(LogInfo) << "No config for player " << player;
      InputDevice* device = LookupDevice(list);
      if (device != nullptr)
      {
        devices[player] = device;
        LOG(LogInfo) << "So i set " << device->Name() << " for player " << player;
        break;
      }
    }
  // Shrink configuration so that there is no hole
  for (int player = 0, i = 0; player < InputEvent::sMaxPlayers; ++player)
  {
    devices[i] = devices[player];
    if (devices[i] != nullptr) ++i;
  }

  std::string command;
  for (int player = 0; player < InputEvent::sMaxPlayers; ++player)
  {
    InputDevice* device = devices[player];
    if (device != nullptr)
    {
      std::string p = " -p" + std::to_string(player + 1);
      command.append(p + "index " + std::to_string(device->Index()));
      command.append(p + "guid " + device->GUID());
      command.append(p + "name \"" + device->Name() + "\"");
      command.append(p + "nbaxes " + std::to_string(device->AxeCount()));

      #ifdef SDL_JOYSTICK_IS_OVERRIDEN_BY_RECALBOX
        command.append(p + "nbaxes " + SDL_JoystickDevicePathById(device->Index()));
      #endif
    }
  }
  LOG(LogInfo) << "Configure emulators command : " << command;
  return command;
}
