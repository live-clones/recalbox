//
// Created by bkg2k on 14/01/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include "utils/os/system/Thread.h"
#include "IMountMonitorNotifications.h"
#include "utils/sdl2/SyncronousEvent.h"
#include "MountDevice.h"

class MountMonitor : private Thread, private ISynchronousEvent
{
  public:
    //! Recalbox Mount point root
    static constexpr const char* sRecalboxRootMountPoint = "/recalbox/share/externals";
    //! Share path
    static constexpr const char* sSharePath = "/recalbox/share";
    //! Share roms path
    static constexpr const char* sShareRomsPath = "/recalbox/share/roms";

    //! Mount info list
    typedef std::vector<DeviceMount> DeviceMountList;
    //! Mount info list
    typedef std::vector<DeviceMount*> DeviceMountReferences;

    //! Constructor
    explicit MountMonitor(IMountMonitorNotifications* interface);

    //! Destructor
    ~MountMonitor() override;

    //! Get current mount point list
    const DeviceMountList& MountPoints() const { return mMountPoints; }

    /*!
     * @brief Get size of a registered mount point
     * @param path Mount point path
     * @return DeviceMount class with free & total size updated or nullptr if path is unknown
     */
    const DeviceMount* SizeOf(const Path& path);

    /*!
     * @brief Update all mount points and return a reference list
     * @return Mount point reference list
     */
    DeviceMountReferences AllMountPoints();

  private:
    //! Mount point list file
    static constexpr const char* sMountPointFile = "/proc/mounts";

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
    //! Share special mount point
    DeviceMount mShareMountPoint;
    //! Share/roms special mount point
    DeviceMount mShareRomsMountPoint;
    //! Mount point to process
    DeviceMount mPendingMountPoint;

    //! Callback interface
    IMountMonitorNotifications* mInterface;

    /*!
     * @brief Load moint points in /media and return them as a path list
     * @return Path list
     */
    DeviceMountList LoadMountPoints();

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



