//
// Created by bkg2k on 10/02/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <string>
#include <utils/os/fs/Path.h>

//! Device mount info
class DeviceMount
{
  public:
    /*!
     * @brief Constructor
     * @param device Device path
     * @param mountpoint Mount point
     * @param name Volume Name
     */
    DeviceMount(const Path& device, const Path& mountpoint, const std::string& name)
      : mDevice(device)
      , mMountPoint(mountpoint)
      , mName(name)
    {
    }

    /*
     * Accessors
     */

    //! Get device path
    const Path& Device() const { return mDevice; }
    //! Get mount point
    const Path& MountPoint() const { return mMountPoint; }
    //! Get volume name
    const std::string& Name() const { return mName; }

    /*
     * Operators
     */

    bool operator == (const DeviceMount& right)
    {
      return mDevice == right.mDevice;
    }

  private:
    Path        mDevice;     //!< Device (/dev/...)
    Path        mMountPoint; //!< Mount point (/recalbox/share/externals/...)
    std::string mName;       //!< Volume name
};
