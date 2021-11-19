#include "InputManager.h"
#include "InputDevice.h"
#include "utils/Log.h"
#include "pugixml/pugixml.hpp"
#include "RootFolders.h"
#include "Input.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_joystick.h"
#include "WindowManager.h"
#include "InputMapper.h"
#include "AutoMapper.h"
#include <algorithm>
#include <guis/GuiInfoPopup.h>
#include <utils/locale/LocaleHelper.h>

#define KEYBOARD_GUID_STRING { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }
#define EMPTY_GUID_STRING { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

InputManager::InputManager()
  : mIndexToId {}
  , mKeyboard(nullptr, InputEvent::sKeyboardDevice, -1, "Keyboard", KEYBOARD_GUID_STRING, 0, 0, 125)
  , mMousse(nullptr, InputEvent::sMousseDevice, 0, "Mousse", KEYBOARD_GUID_STRING, 0, 0, 5)
{
  // Create keyboard
  LoadDefaultKeyboardConfiguration();
}

InputManager::~InputManager()
{
  //Finalize(); // TODO: move to LifeCycleController
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

  { LOG(LogError) << "[Input] Unexisting device!"; }
  static InputDevice sEmptyDevice(nullptr, InputEvent::sEmptyDevice, -1, "Empty Device", EMPTY_GUID_STRING, 0, 0, 0);
  return sEmptyDevice;
}

void InputManager::IntitializeSDL2JoystickSystem()
{
  SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
  /*if (SDL_InitSubSystem(SDL_INIT_JOYSTICK) < 0)
  {
    LOG(LogError) << "[InputManager] Error initializing SDL Joystick system";
    return;
  }*/
  SDL_JoystickEventState(SDL_ENABLE);
  SDL_JoystickUpdate();
  LOG(LogInfo) << "[InputManager] Initialize SDL Joystick system";
}

void InputManager::FinalizeSDL2JoystickSystem()
{
  SDL_JoystickEventState(SDL_DISABLE);
  //SDL_QuitSubSystem(SDL_INIT_JOYSTICK);
  LOG(LogInfo) << "[InputManager] Finalize SDL Joystick system";
}

void InputManager::Finalize()
{
  ClearAllConfigurations();
  FinalizeSDL2JoystickSystem();
}

void InputManager::Initialize()
{
  ClearAllConfigurations();
  IntitializeSDL2JoystickSystem();
  LoadAllJoysticksConfiguration(std::vector<InputDevice>(), nullptr, false);
}

void InputManager::Refresh(WindowManager* window, bool padplugged)
{
  IntitializeSDL2JoystickSystem();
  std::vector<InputDevice> previousList = BuildCurrentDeviceList();
  ClearAllConfigurations();
  LoadAllJoysticksConfiguration(previousList, window, padplugged);
  LOG(LogInfo) << "[InputManager] Refresh joysticks";
}

void InputManager::LoadDefaultKeyboardConfiguration()
{
  // Load default configuration
  mKeyboard.ClearAll();
  mKeyboard.Set(InputDevice::Entry::Up, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_UP, 1));
  mKeyboard.Set(InputDevice::Entry::Down, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_DOWN, 1));
  mKeyboard.Set(InputDevice::Entry::Left, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_LEFT, 1));
  mKeyboard.Set(InputDevice::Entry::Right, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_RIGHT, 1));

  mKeyboard.Set(InputDevice::Entry::A, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_s, 1));
  mKeyboard.Set(InputDevice::Entry::B, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_a, 1));
  mKeyboard.Set(InputDevice::Entry::X, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_p, 1));
  mKeyboard.Set(InputDevice::Entry::Y, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_l, 1));
  mKeyboard.Set(InputDevice::Entry::Start, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_RETURN, 1));
  mKeyboard.Set(InputDevice::Entry::Select, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_SPACE, 1));

  mKeyboard.Set(InputDevice::Entry::L1, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_PAGEUP, 1));
  mKeyboard.Set(InputDevice::Entry::R1, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_PAGEDOWN, 1));

  mKeyboard.Set(InputDevice::Entry::Hotkey, InputEvent(InputEvent::sKeyboardDevice, InputEvent::EventType::Key, SDLK_ESCAPE, 1));

  //WriteDeviceXmlConfiguration(mKeyboard);

  mMousse.ClearAll();
  mMousse.Set(InputDevice::Entry::B, InputEvent(InputEvent::sMousseDevice, InputEvent::EventType::Button, SDL_BUTTON_LEFT, 1));
  mMousse.Set(InputDevice::Entry::A, InputEvent(InputEvent::sMousseDevice, InputEvent::EventType::Button, SDL_BUTTON_RIGHT, 1));

  // Load configuration
  LookupDeviceXmlConfiguration(mKeyboard);
  LookupDeviceXmlConfiguration(mMousse);
}

void InputManager::ClearAllConfigurations()
{
  // Close SDL devices
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

void InputManager::LoadAllJoysticksConfiguration(std::vector<InputDevice> previous, WindowManager* window, bool padplugged)
{
  int numJoysticks = SDL_NumJoysticks();
  LOG(LogInfo) << "[InputManager] Joystick count: " << numJoysticks;
  for (int i = 0; i < numJoysticks; i++)
    LoadJoystickConfiguration(i);

  // No info popup ?
  if (window == nullptr) return;
  if (!padplugged) return;

  std::vector<InputDevice> current = BuildCurrentDeviceList();
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

      GuiInfoPopup* popup = new GuiInfoPopup(*window, text, 10, GuiInfoPopup::PopupType::Pads);
      window->InfoPopupAdd(popup);
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

      GuiInfoPopup* popup = new GuiInfoPopup(*window, text, 10, GuiInfoPopup::PopupType::Pads);
      window->InfoPopupAdd(popup);
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
  LOG(LogInfo) << "[InputManager] Lond configuration for Joystick #: " << index;

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
    std::string autoMapping = AutoMapper(index).GetSDLMapping();
    bool autoConfigured = !autoMapping.empty();
    if (autoConfigured)
    {
      autoConfigured = device.LoadAutoConfiguration(autoMapping);
      WriteDeviceXmlConfiguration(device);
    }

    if (!autoConfigured)
    { LOG(LogWarning) << "[Input] Unknown joystick " << SDL_JoystickName(joy)
                      << " (GUID: " << DeviceGUIDString(joy) << ", Instance ID: " << identifier
                      << ", Device Index: " << index
                      << ", Axis: " << SDL_JoystickNumAxes(joy)
                      << ", Hats: " << SDL_JoystickNumHats(joy)
                      << ", Buttons: " << SDL_JoystickNumButtons(joy) << ")"; }
  }

  // Store
  mIdToDevices[identifier] = device;
  { LOG(LogInfo) << "[Input] Added joystick " << SDL_JoystickName(joy)
                    << " (GUID: " << DeviceGUIDString(joy) << ", Instance ID: " << identifier
                    << ", Device Index: " << index
                    << ", Axis: " << SDL_JoystickNumAxes(joy)
                    << ", Hats: " << SDL_JoystickNumHats(joy)
                    << ", Buttons: " << SDL_JoystickNumButtons(joy) << ")"; }

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
  }
  return mKeyboard.ConvertToCompact(event);
}

InputCompactEvent InputManager::ManageMousseButtonEvent(const SDL_MouseButtonEvent& button, bool down)
{
  InputEvent event = InputEvent((int)button.which, InputEvent::EventType::Button, button.button, down  ? 1 : 0);
  return mMousse.ConvertToCompact(event);
}

InputCompactEvent InputManager::ManageSDLEvent(WindowManager* window, const SDL_Event& ev)
{
  switch (ev.type)
  {
    case SDL_JOYAXISMOTION: return ManageAxisEvent(ev.jaxis);
    case SDL_JOYBUTTONDOWN:
    case SDL_JOYBUTTONUP: return ManageButtonEvent(ev.jbutton);
    case SDL_JOYHATMOTION: return ManageHatEvent(ev.jhat);
    case SDL_KEYDOWN:
    case SDL_KEYUP: return ManageKeyEvent(ev.key, ev.type == SDL_KEYDOWN);
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP: return ManageMousseButtonEvent(ev.button, ev.type == SDL_MOUSEBUTTONDOWN);
    case SDL_JOYDEVICEADDED:
    case SDL_JOYDEVICEREMOVED:
    {
      { LOG(LogInfo) << "[Input] Reinitialize because of joystick added/removed."; }
      Refresh(window, true);
      for(int i = mNotificationInterfaces.Count(); --i >= 0; )
        mNotificationInterfaces[i]->PadsAddedOrRemoved();
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
    { LOG(LogError) << "[Input] Error parsing input config: " << res.description(); }
    return false;
  }

  { LOG(LogDebug) << "[Input] Looking for configuration for " << device.Name() << " (UUID: " << device.GUID()
                  << ") - Axis: " << device.AxeCount()
                  << " - Hats: " << device.HatCount()
                  << " - Buttons: " << device.ButtonCount(); }

  pugi::xml_node root = doc.child("inputList");
  if (root != nullptr)
    for (pugi::xml_node item = root.child("inputConfig"); item != nullptr; item = item.next_sibling("inputConfig"))
    {
      // check the guid
      bool guid    = (strcmp(device.GUID().c_str(), item.attribute("deviceGUID").value()) == 0) || device.IsKeyboard();
      bool name    = strcmp(device.Name().c_str(), item.attribute("deviceName").value()) == 0;
      bool axes    = (device.AxeCount() == item.attribute("deviceNbAxes").as_int()) || device.IsKeyboard();
      bool hats    = (device.HatCount() == item.attribute("deviceNbHats").as_int()) || device.IsKeyboard();
      bool buttons = (device.ButtonCount() == item.attribute("deviceNbButtons").as_int()) || device.IsKeyboard();
      if (guid && name && axes && hats && buttons)
      {
        int loaded = device.LoadFromXml(item);
        { LOG(LogDebug) << "[Input] Loaded " << item.attribute("deviceGUID").value()
                        << " (UUID: " << item.attribute("deviceName").value()
                        << ") - Axis: " << item.attribute("deviceNbAxes").as_int()
                        << " - Hats: " << item.attribute("deviceNbHats").as_int()
                        << " - Buttons: " << item.attribute("deviceNbButtons").as_int()
                        << " : " << loaded << " config. entries."; }
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
    if (!result) { LOG(LogError) << "[Input] Error parsing input config: " << result.description(); }
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

OrderedDevices InputManager::GetMappedDeviceList(const InputMapper& mapper)
{
  OrderedDevices devices;
  for (int player = 0; player < Input::sMaxInputDevices; ++player)
  {
    const InputMapper::Pad& pad = mapper.PadAt(player);
    if (pad.IsConnected())
      devices.SetDevice(player, mIdToDevices[pad.Identifier]);
  }

  return devices;
}

std::string InputManager::GetMappedDeviceListConfiguration(const InputMapper& mapper)
{
  std::string command;
  for (int player = 0; player < Input::sMaxInputDevices; ++player)
  {
    const InputMapper::Pad& pad = mapper.PadAt(player);
    if (pad.IsConnected())
    {
      const InputDevice& device = mIdToDevices[pad.Identifier];
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
  { LOG(LogInfo) << "[Input] Configure emulators command : " << command; }
  return command;
}

void InputManager::LogRawEvent(const InputEvent& event)
{
  { LOG(LogDebug) << "[Input] Raw Event: " << event.ToString(); }
}

void InputManager::LogCompactEvent(const InputCompactEvent& event)
{
  { LOG(LogDebug) << "[Input] Compact Event: " << event.ToString(); }
}

void InputManager::AddNotificationInterface(IInputChange* interface)
{
  if (!mNotificationInterfaces.Contains(interface))
    mNotificationInterfaces.Add(interface);
}

void InputManager::RemoveNotificationInterface(IInputChange* interface)
{
  int index = mNotificationInterfaces.IndexOf(interface);
  if (index >= 0)
    mNotificationInterfaces.Delete(index);
}


