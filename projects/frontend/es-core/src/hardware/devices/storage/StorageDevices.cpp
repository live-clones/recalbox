//
// Created by bkg2k on 21/02/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "utils/locale/LocaleHelper.h"
#include "hardware/Board.h"
#include "RootFolders.h"
#include "StorageDevices.h"

void StorageDevices::Initialize()
{
  AnalyseMounts();
  std::string current = GetStorageDevice();

  // Get storage sizes
  DeviceSizeInfo sizeInfos = GetFileSystemInfo();

  // Ram?
  if (mShareInRAM)
  {
    current = "RAM";
    mDevices.push_back(Device(Types::Ram, "SHARE",  sInMemory, "RECALBOX", "tmpfs",_("In Memory!")+ " \u26a0", true, sizeInfos));
    LOG(LogWarning) << "[Storage] Share is stored in memory!";
  }

  // Add Internal
  mDevices.push_back(Device(Types::Internal, "SHARE",  sInternal, "RECALBOX", "exfat",_("Internal Share Partition"), current == sInternal, sizeInfos));

  // Network
  if (mBootConfiguration.HasKeyStartingWith("sharenetwork_") || mBootConfiguration.AsString("sharedevice") == "NETWORK")
  {
    mDevices.push_back(Device(Types::Internal, "", sNetwork, "", "", _("Network Share"), current == sNetwork, sizeInfos));
    LOG(LogDebug) << "[Storage] Network share configuration detected";
  }

  // Go advance does not support external storage
  if ((Board::Instance().GetBoardType() == BoardType::OdroidAdvanceGo) ||
      (Board::Instance().GetBoardType() == BoardType::OdroidAdvanceGoSuper)) return;

  // Any external
  mDevices.push_back(Device(Types::Internal, "",  sAnyExternal, "", "",_("Any External Device"), current == sAnyExternal, sizeInfos));

  // External Devices
  std::string devicePath;
  std::string propertyLine;
  for(const std::string& line : GetRawDeviceList())
    if (Strings::SplitAt(line, ':', devicePath, propertyLine, true))
    {
      // Avoid boot device partitions
      if (Strings::StartsWith(devicePath, mBootRoot)) continue;

      // Extract device properties
      PropertyMap properties = ExtractProperties(propertyLine);
      std::string uuid = "DEV " + properties.get_or_return_default("UUID");
      std::string label = properties.get_or_return_default("LABEL");
      if (label.empty()) label = "Unnamed";
      std::string filesystem = properties.get_or_return_default("TYPE");
      if (filesystem.empty()) filesystem = "fs?";
      std::string displayable = _("Device %d - %l (%f)");
      Strings::ReplaceAllIn(displayable, "%d", Path(devicePath).Filename());
      Strings::ReplaceAllIn(displayable, "%l", label);
      Strings::ReplaceAllIn(displayable, "%f", filesystem);

      // Store
      mDevices.push_back(Device(Types::External, devicePath, uuid, label, filesystem, displayable, current == uuid, sizeInfos));
      LOG(LogDebug) << "[Storage] External storage: " << devicePath << ' ' << uuid << " \"" << label << "\" " << filesystem;
    }
}

Strings::Vector StorageDevices::GetCommandOutput(const std::string& command)
{
  std::string output;
  char buffer[4096];
  FILE* pipe = popen(command.data(), "r");
  if (pipe != nullptr)
  {
    while (feof(pipe) == 0)
      if (fgets(buffer, sizeof(buffer), pipe) != nullptr)
        output.append(buffer);
    pclose(pipe);
  }
  return Strings::Split(output, '\n');
}

Strings::Vector StorageDevices::GetRawDeviceList()
{
  return GetCommandOutput("blkid");
}

Strings::Vector StorageDevices::GetMountedDeviceList()
{
  return GetCommandOutput("mount");
}

StorageDevices::DeviceSizeInfo StorageDevices::GetFileSystemInfo()
{
  DeviceSizeInfo result;
  for(const std::string& line : GetCommandOutput("df -kP"))
  {
    Strings::Vector items = Strings::Split(line, ' ', true);
    if (items.size() >= 6)
    {
      long long size = 0; Strings::ToLong(items[1], size);
      long long free = 0; Strings::ToLong(items[3], free);
      result[items[0]] = SizeInfo(size, free);
      // Special cases
      if (items[5] == RootFolders::DataRootFolder.ToString())
        result["SHARE"] = SizeInfo(size, free);
    }
  }
  return result;
}

StorageDevices::PropertyMap StorageDevices::ExtractProperties(const std::string& properties)
{
  PropertyMap map;

  std::string key;
  std::string value;
  for(const std::string& kv : Strings::SplitQuotted(properties, ' '))
    if (Strings::SplitAt(kv, '=', key, value, true))
      map[key] = Strings::Trim(value, "\"");

  return map;
}

void StorageDevices::SetStorageDevice(const StorageDevices::Device& device)
{
  mBootConfiguration.SetString(sShareDevice, device.UUID);
  mBootConfiguration.Save();
}

std::string StorageDevices::GetStorageDevice()
{
  return mBootConfiguration.AsString(sShareDevice, sInternal);
}

void StorageDevices::AnalyseMounts()
{
  mBootRoot = "/dev/bootdevice";
  for(const std::string& line : GetMountedDeviceList())
  {
    Strings::Vector items = Strings::Split(line, ' ', true);
    if (items.size() < 6)
    {
      LOG(LogError) << "[Storage] Incomplete mount line: " << line;
      continue;
    }
    if (items[2] == "/recalbox/share") mShareInRAM =  (items[4] == "tmpfs");
    else if (items[2] == "/boot") mBootRoot = Strings::Trim(items[0], "0123456789");
  }
  if (mBootRoot.empty()) mBootRoot = "/dev/boot"; // for testing purpose only :)
  LOG(LogDebug) << "[Storage] BootRoot: " << mBootRoot << " - Is In Ram: " << mShareInRAM;
}

