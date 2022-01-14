//
// Created by bkg2k on 14/01/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <utils/os/system/Thread.h>
#include <systems/IMountMonitorNotifications.h>
#include "utils/sdl2/SyncronousEvent.h"
#include "MountDevice.h"

class MountMonitor : private Thread, private ISynchronousEvent
{
  public:
    //! Mount info list
    typedef std::vector<DeviceMount> DeviceMountList;

    //! Constructor
    explicit MountMonitor(IMountMonitorNotifications* interface);

    //! Destructor
    ~MountMonitor() override;

    //! Get current mount point list
    const DeviceMountList& MountPoints() const { return mMountPoints; }

  private:
    //! Mount point list file
    static constexpr const char* sMountPointFile = "/proc/mounts";
    //! Recalbox Mount point root
    static constexpr const char* sRecalboxRootMountPoint = "/recalbox/share/externals";
    //! Share path
    static constexpr const char* sSharePath = "/recalbox/share";

    //! Action used in synchronous event
    enum class Action
    {
      Mount,   //!< mPendingMountPoint is a new mount point
      Unmount, //!< mPendingMountPoint is a removed mount point
    };

    //! Syncro events
    SyncronousEvent mEvent;

    //! Current path list
    DeviceMountList mMountPoints;
    //! Mount point to process
    DeviceMount mPendingMountPoint;

    //! Callback interface
    IMountMonitorNotifications* mInterface;

    /*!
     * @brief Load moint points in /media and return them as a path list
     * @return Path list
     */
    static DeviceMountList LoadMountPoints();

    /*!
     * @brief Compare old list and new list an generate add/remove events accordingly
     * @param oldList Old list (before changes)
     * @param newList New list (after changes)
     */
    void Process(DeviceMountList& oldList, const DeviceMountList& newListOriginal);

    /*!
     * @brief Get partition name from a device path
     * @param devicePath Device path
     * @return Partition name
     */
    static std::string GetPartitionLabel(const Path& devicePath);

    /*
     * Thread implementation
     */

    //! Monitor main loop
    void Run() override;

    /*
     * ISynchronousEvent implementation
     */

    //! Requested to process mountpoints
    void ReceiveSyncCallback(const SDL_Event& event) override;
};



