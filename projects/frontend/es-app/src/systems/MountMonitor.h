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

class MountMonitor : private Thread, private ISynchronousEvent
{
  public:
    //! Constructor
    explicit MountMonitor(IMountMonitorNotifications* interface);

    //! Destructor
    ~MountMonitor() override;

    //! Get current mount point list
    const Path::PathList& MountPoints() const { return mMountPoints; }

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
    Path::PathList mMountPoints;
    //! Mount point to process
    Path mPendingMountPoint;

    //! Callback interface
    IMountMonitorNotifications* mInterface;

    /*!
     * @brief Load moint points in /media and return them as a path list
     * @return Path list
     */
    static Path::PathList LoadMountPoints();

    /*!
     * @brief Compare old list and new list an generate add/remove events accordingly
     * @param oldList Old list (before changes)
     * @param newList New list (after changes)
     */
    void Process(Path::PathList& oldList, const Path::PathList& newListOriginal);

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



