//
// Created by bkg2k on 14/01/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <fcntl.h>
#include <systems/MountMonitor.h>
#include <utils/Log.h>
#include <sys/poll.h>
#include "utils/Files.h"

MountMonitor::MountMonitor(IMountMonitorNotifications* interface)
  : mEvent(this)
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

    Path::PathList newMounts = LoadMountPoints();
    Process(mMountPoints, newMounts);
    mMountPoints = std::move(newMounts);
  }

  close(fd);
}

Path::PathList MountMonitor::LoadMountPoints()
{
  static constexpr const int sDeviceIndex = 0;
  static constexpr const int sMountPointIndex = 1;
  static constexpr const int sTypeIndex = 2;

  Path::PathList result;

  // Get all valid mount point
  for(const std::string& line : Strings::Split(Files::LoadFile(Path(sMountPointFile)), '\n')) // For every entry
  {
    Strings::Vector items = Strings::Split(line, ' ');
    const std::string& device = items[sDeviceIndex];
    const Path mountPoint(items[sMountPointIndex]);
    const std::string& type = items[sTypeIndex];

    // Physical USB device?
    if (Strings::StartsWith(device, LEGACY_STRING("/dev/"))) // starting with /dev/
    {
      if (mountPoint == sSharePath) continue;                // Don't store real share
      #ifndef DEBUG
      if (mountPoint.StartWidth(sRecalboxRootMountPoint))    // is it valid?
      #endif
      result.push_back(mountPoint);                          // so store it in the list
    }
    // Network?
    if (type == "cifs" || Strings::StartsWith(type,LEGACY_STRING("nfs")))
    {
      if (mountPoint.StartWidth(sRecalboxRootMountPoint))  // is it valid?
        result.push_back(mountPoint);                      // so store it in the list
    }
  }

  // Final result
  return result;
}

void MountMonitor::ReceiveSyncCallback(const SDL_Event& event)
{
  switch((Action)event.user.code)
  {
    case Action::Mount: if (mInterface != nullptr) mInterface->DeviceMount(mPendingMountPoint); break;
    case Action::Unmount: if (mInterface != nullptr) mInterface->DeviceUnmount(mPendingMountPoint); break;
    default: break;
  }
}

void MountMonitor::Process(Path::PathList& oldList, const Path::PathList& newListOriginal)
{
  Path::PathList newList = newListOriginal;
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
  for(const Path& mp : oldList)
  {
    { LOG(LogInfo) << "[MountMonitor] Mount point removed: " << mp.ToString(); }
    mPendingMountPoint = mp;
    mEvent.Call((int)Action::Unmount);
  }
  // Remaining points in new list are added mount points
  for(const Path& mp : newList)
  {
    { LOG(LogInfo) << "[MountMonitor] Mount point added: " << mp.ToString(); }
    mPendingMountPoint = mp;
    mEvent.Call((int)Action::Mount);
  }
}
