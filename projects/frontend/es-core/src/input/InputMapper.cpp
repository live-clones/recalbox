//
// Created by bkg2k on 15/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <input/InputMapper.h>
#include <input/InputManager.h>
#include <utils/Log.h>
#include <RecalboxConf.h>

InputMapper::InputMapper(IInputChange* interface)
  : mInterface(interface)
{
  LoadConfiguration();
  Build();
  InputManager::Instance().AddNotificationInterface(this);
}

InputMapper::~InputMapper()
{
  SaveConfiguration();
  InputManager::Instance().RemoveNotificationInterface(this);
}

void InputMapper::Build()
{
  // Get connected pads
  mConnected = AvailablePads();
  // Copy to pad active list
  PadList activePads(mConnected);

  // Remove device already fetched from configuration
  // Also add non-connected pads to the phantom list
  for(Pad& device : mPads)
    if (device.IsValid())
      if ((device.Identifier = LookupAndPopDevice(activePads, device)) < 0)
      {
        mUnconnected.push_back(device);
        LOG(LogDebug) << "[PadMapping] Move to unconnected: " << device.AsString();
      }

  // Add remaining pads in unused slots
  bool assignNew = false;
  for(const Pad& connectedDevice : activePads)
    for (Pad& device : mPads)
      if (!device.IsValid())
      {
        LOG(LogDebug) << "[PadMapping] Add connected to the list: " << connectedDevice.AsString();
        device.Set(connectedDevice.Name, connectedDevice.UUID, connectedDevice.Identifier);
        assignNew = true;
        break;
      }

  // Push active pads first
  LOG(LogDebug) << "[PadMapping] Sort active first";
  SortActiveFirst(mPads);

  int index = 0;
  for(const Pad& pad : mPads)
    LOG(LogDebug) << "[PadMapping] Pad @" << ++index << " = " << pad.AsString();

  // Save config
  if (assignNew)
    SaveConfiguration();
}

void InputMapper::LoadConfiguration()
{
  // Fill from configuration
  std::string uuid, name;
  for(int i = Input::sMaxInputDevices; --i >= 0; )
    if (Strings::SplitAt(RecalboxConf::Instance().GetPad(i), ':', uuid, name, true))
    {
      mPads[i].Set(name, uuid, -1);
      LOG(LogDebug) << "[PadMapping] Load pad @" << i << " = " << mPads[i].AsString();
    }
}

void InputMapper::SaveConfiguration()
{
  for(int i = Input::sMaxInputDevices; --i >= 0;)
    if (mPads[i].IsValid())
    {
      RecalboxConf::Instance().SetPad(i, std::string(mPads[i].UUID).append(1, ':').append(mPads[i].Name));
      LOG(LogDebug) << "[PadMapping] Save pad @" << i << " = " << mPads[i].AsString();
    }
  RecalboxConf::Instance().Save();
}

InputMapper::PadList InputMapper::AvailablePads()
{
  PadList result;
  for(int i = 0; i < InputManager::Instance().DeviceCount(); ++i)
  {
    const InputDevice& device = InputManager::Instance().GetDeviceConfigurationFromIndex(i);
    result.push_back(Pad(Strings::Trim(device.Name(), " \t\r\n"), device.GUID(), device.Identifier()));
    LOG(LogDebug) << "[PadMapping] Available pad @" << i << " = " << result.back().AsString();
  }
  return result;
}

void InputMapper::SortActiveFirst(PadArray& padArray)
{
  // Seek highest non empty/phantom devices
  int higher = Input::sMaxInputDevices;
  while(--higher >= 0)
    if (padArray[higher].Identifier >= 0)
      break;

  // No devices or no empty slot?
  if ((unsigned int)higher >= Input::sMaxInputDevices) return;

  // Stack the active ones
  for(int i = 0; i < higher; ++i)
    if (padArray[i].Identifier < 0)
    {
      Pad tmp = padArray[i];
      for(int j = i; j < higher; ++j) padArray[j] = padArray[j+1];
      padArray[higher] = tmp;
      --higher;
      --i; // restart @ same position
    }
}

int InputMapper::LookupDevice(const PadList& list, const Pad& pad)
{
  for(int i = 0; i < (int)list.size(); ++i)
    if (list[i].Equals(pad))
      return i;
  return -1;
}

int InputMapper::LookupAndPopDevice(PadList& list, const Pad& pad)
{
  int foundIndex = -1;
  for(int i = 0; i < (int)list.size(); ++i)
    if (list[i].Same(pad))
    {
      foundIndex = i;
      break;
    }
  if (foundIndex >= 0)
  {
    int identifier = list[foundIndex].Identifier;
    list.erase(list.begin() + foundIndex);
    return identifier;
  }
  return -1;
}

std::string InputMapper::GetDecoratedName(int index)
{
  int count = 0;
  const Pad& pad = mPads[index];
  if (pad.Identifier >= 0)
    for(const Pad& current : mPads)
      if (current.Same(pad))
        if (current.Identifier < pad.Identifier)
          count++;

  std::string result(pad.Name);
  if (count > 0) result.append(LEGACY_STRING(" #")).append(Strings::ToString(count + 1));
  result.insert(0, pad.Identifier < 0 ? "\u26aa" : "\u26ab")
        .append(pad.Identifier < 0 ? "\u26aa" : "\u26ab");
  return result;
}

void InputMapper::Swap(int index1, int index2)
{
  // Clamp indexes
  index1 = Math::clampi(index1, 0, Input::sMaxInputDevices - 1);
  index2 = Math::clampi(index2, 0, Input::sMaxInputDevices - 1);

  // Really swap?
  if ((index1 == index2) || (index1 < 0) || (index2 < 0)) return;

  // Swap
  Pad tmp       = mPads[index1];
  mPads[index1] = mPads[index2];
  mPads[index2] = tmp;
}

void InputMapper::PadsAddedOrRemoved()
{
  Build();
  if (mInterface != nullptr)
    mInterface->PadsAddedOrRemoved();
}
