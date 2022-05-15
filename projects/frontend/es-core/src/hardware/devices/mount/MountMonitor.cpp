//
// Created by bkg2k on 14/01/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <fcntl.h>
#include "MountMonitor.h"
#include "utils/Log.h"
#include <sys/poll.h>
#include "utils/Files.h"
#include <blkid/blkid.h>

MountMonitor::MountMonitor(IMountMonitorNotifications* interface)
  : mEvent(this)
  , mShareMountPoint(Path::Empty, Path::Empty, Strings::Empty, Strings::Empty, Strings::Empty)
  , mShareRomsMountPoint(Path::Empty, Path::Empty, Strings::Empty, Strings::Empty, Strings::Empty)
  , mPendingMountPoint(Path::Empty, Path::Empty, Strings::Empty, Strings::Empty, Strings::Empty)
  , mInterface(interface)
{
  // Initialize mount points
  mMountPoints = LoadMountPoints();

  { LOG(LogInfo) << "[MountMonitor] Mount monitoring starts"; }
  Start("MountMon");
}

MountMonitor::~MountMonitor()
{
  Stop();
  { LOG(LogInfo) << "[MountMonitor] Mount monitoring stops"; }
}

void MountMonitor::Run()
{
  int fd = open(sMountPointFile, O_RDONLY);
  if (fd < 0) { LOG(LogError) << "[MountMonitor] Fatal error monitoring mount devices!"; return; }

  while(IsRunning())
  {
    struct pollfd poller { .fd = fd, .events = POLLPRI, .revents = 0 };
    if (poll(&poller, 1, 500) != 1 || (poller.revents & POLLPRI) == 0) continue;

    DeviceMountList newMounts = LoadMountPoints();
    Process(mMountPoints, newMounts);
    mMountPoints = std::move(newMounts);
  }

  close(fd);
}

MountMonitor::DeviceMountList MountMonitor::LoadMountPoints()
{
  static constexpr const int sDeviceIndex = 0;
  static constexpr const int sMountPointIndex = 1;
  static constexpr const int sTypeIndex = 2;
  static constexpr const int sOptionsIndex = 3;

  { LOG(LogDebug) << "[MountMonitor] Loading available mount points"; }
  DeviceMountList result;

  // Device actually mounted on /recalbox/share
  Path shareDevice;

  // Get all valid mount point
  for(const std::string& line : Strings::Split(Files::LoadFile(Path(sMountPointFile)), '\n')) // For every entry
  {
    Strings::Vector items = Strings::Split(line, ' ');
    const Path device(items[sDeviceIndex]);
    const Path mountPoint(items[sMountPointIndex]);
    const std::string& type = items[sTypeIndex];
    const std::string& options = items[sOptionsIndex];

    //{ LOG(LogDebug) << "[MountMonitor] Pre-Candidate: " << device.ToString() << " mounted to " << mountPoint.ToString() << " (" << GetPartitionLabel(device) << ')';  }

    // Physical USB device?
    if (device.StartWidth(std::string(LEGACY_STRING("/dev/")))) // starting with /dev/
    {
      if (mountPoint == sSharePath)
      {
        mShareMountPoint = DeviceMount(device, mountPoint, "Share", type, options); // so store it in the list
        shareDevice = device; // Store share device
        { LOG(LogDebug) << "[MountMonitor] share: " << device.ToString() << " mounted to " << mountPoint.ToString() << " (" << GetPartitionLabel(device) << ')';  }
        continue; // Don't store real share
      }
      else
      {
        #ifndef DEBUG
        if (mountPoint.StartWidth(sRecalboxRootMountPoint))    // is it valid?
        #endif
        result.push_back(DeviceMount(device, mountPoint, GetPartitionLabel(device), type, options)); // so store it in the list
        { LOG(LogDebug) << "[MountMonitor] Candidate: " << device.ToString() << " mounted to " << mountPoint.ToString() << " (" << GetPartitionLabel(device) << ')';  }
      }
    }
    // Network?
    if (type == "cifs" || Strings::StartsWith(type,LEGACY_STRING("nfs")))
    {
      if (mountPoint.StartWidth(sRecalboxRootMountPoint)) // is it valid?
      {
        result.push_back(DeviceMount(device, mountPoint, "Network " + type, type, options)); // so store it in the list
        { LOG(LogDebug) << "[MountMonitor] Candidate: " << device.ToString() << " mounted to " << mountPoint.ToString() << " (" << GetPartitionLabel(device) << ')';  }
      }
      else if (mountPoint.StartWidth(sShareRomsPath))
      {
        mShareRomsMountPoint = DeviceMount(device, mountPoint, "Share's Roms", type, options); // so store it in the list
        { LOG(LogDebug) << "[MountMonitor] share/roms: " << device.ToString() << " mounted to " << mountPoint.ToString();  }
      }
    }
  }

  // Seek & destroy any Mount device that match the share device
  for(int i = (int)result.size(); --i >= 0;)
    if (result[i].Device() == shareDevice)
      result.erase(result.begin() + i);

  // Final result
  return result;
}

std::string MountMonitor::GetPartitionLabel(const Path& devicePath)
{
  blkid_probe pr = blkid_new_probe_from_filename(devicePath.ToChars());
  if (pr == nullptr)
  {
    { LOG(LogError) << "[MountMonitor] Can't get BLKID structure from " << devicePath.ToString(); }
    return "UNKNOWN";
  }
  const char* string = nullptr;
  blkid_do_probe(pr);
  blkid_probe_lookup_value(pr, "LABEL", &string, nullptr);
  if (string == nullptr)
  {
    { LOG(LogWarning) << "[MountMonitor] Can't get LABEL from " << devicePath.ToString(); }
    blkid_probe_lookup_value(pr, "UUID", &string, nullptr);
    if (string == nullptr)
    {
      { LOG(LogWarning) << "[MountMonitor] Can't get UUID from " << devicePath.ToString(); }
      string = "UNKNOWN";
    }
  }
  std::string result(string); // Store result before destroying blkid structures!
  blkid_free_probe(pr);
  { LOG(LogDebug) << "[MountMonitor] " << devicePath.ToString() << " name is: " << result; }
  return result;
}

void MountMonitor::ReceiveSyncCallback(const SDL_Event& event)
{
  switch((Action)event.user.code)
  {
    case Action::Mount: if (mInterface != nullptr) mInterface->NotifyDeviceMount(mPendingMountPoint); break;
    case Action::Unmount: if (mInterface != nullptr) mInterface->NotifyDeviceUnmount(mPendingMountPoint); break;
    default: break;
  }
}

void MountMonitor::Process(DeviceMountList& oldList, const DeviceMountList& newListOriginal)
{
  DeviceMountList newList = newListOriginal;
  // Kill dups in both list
  for(int i = (int)oldList.size(); --i >= 0;)
    for(int j = (int)newList.size(); --j >= 0;)
      if (oldList[i] == newList[j])
      {
        oldList.erase(oldList.begin() + i);
        newList.erase(newList.begin() + j);
        break;
      }
  // Remaining points in old list are removed mount points
  for(const DeviceMount& mp : oldList)
  {
    { LOG(LogInfo) << "[MountMonitor] Mount point removed: " << mp.MountPoint().ToString(); }
    mPendingMountPoint = mp;
    mEvent.Call((int)Action::Unmount);
  }
  // Remaining points in new list are added mount points
  for(const DeviceMount& mp : newList)
  {
    { LOG(LogInfo) << "[MountMonitor] Mount point added: " << mp.MountPoint().ToString(); }
    mPendingMountPoint = mp;
    mEvent.Call((int)Action::Mount);
  }
}

const DeviceMount* MountMonitor::SizeOf(const Path& path)
{
  // In mount point list?
  for(DeviceMount& device : mMountPoints)
    if (!device.MountPoint().IsEmpty())
      if (path.StartWidth(device.MountPoint()))
        return &device.UpdateSize();
  // Share/roms?
  if (!mShareRomsMountPoint.MountPoint().IsEmpty())
    if (path.StartWidth(mShareRomsMountPoint.MountPoint()))
      return &mShareRomsMountPoint.UpdateSize();
  // Share?
  if (!mShareMountPoint.MountPoint().IsEmpty())
    if (path.StartWidth(mShareMountPoint.MountPoint()))
      return &mShareMountPoint.UpdateSize();
  // Nothing...
  return nullptr;
}

MountMonitor::DeviceMountReferences MountMonitor::AllMountPoints()
{
  DeviceMountReferences references;
  // Add share
  references.push_back(&mShareMountPoint);
  // Add rom mount point
  if (!mShareRomsMountPoint.MountPoint().IsEmpty())
    references.push_back(&mShareRomsMountPoint);
  // Add all mountpoints
  for(DeviceMount& device : mMountPoints)
    references.push_back(&device.UpdateSize());
  return references;
}
