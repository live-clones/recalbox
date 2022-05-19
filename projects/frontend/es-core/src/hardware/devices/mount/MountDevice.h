//
// Created by bkg2k on 10/02/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <string>
#include "utils/os/fs/Path.h"
#include "utils/Strings.h"
#include <sys/statvfs.h>

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
    DeviceMount(const Path& device, const Path& mountpoint, const std::string& name, const std::string& type, const std::string& options)
      : mDevice(device)
      , mMountPoint(mountpoint)
      , mName(name)
      , mType(type)
      , mTotalSize(0)
      , mFreeSize(0)
      , mReadOnly(false)
    {
      for(const std::string& option : Strings::Split(options, ','))
        if (option == "ro")
        {
          mReadOnly = true;
          break;
        }
    }

    /*
     * Tool
     */

    std::string DisplayableDeviceName() const
    {
      return std::string(mName)
             .append(" (", 2)
             .append(mDevice.ToString())
             .append(1, ')');
    }

    std::string DisplayableFreeSpace() const
    {
      return Strings::ToHumanSize(mFreeSize)
             .append(1, '/')
             .append(Strings::ToHumanSize(mTotalSize))
             .append(" (", 2)
             .append(mTotalSize == 0 ? std::string("Unknown") : Strings::ToString((mFreeSize * 100) / mTotalSize))
             .append("%)", 2);
    }

    /*!
     * @brief Update size & free
     * @return This
     */
    DeviceMount& UpdateSize()
    {
      struct statvfs fiData {};
      if ((statvfs(mMountPoint.ToChars(), &fiData)) >= 0)
      {
        mTotalSize = ((long long)fiData.f_blocks * (long long)fiData.f_bsize);
        mFreeSize = ((long long)fiData.f_bfree * (long long)fiData.f_bsize);
      }
      return *this;
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
    //! Get file system type
    const std::string& Type() const { return mType; }
    //! Total size
    long long TotalSize() const { return mTotalSize; }
    //! Free size
    long long FreeSize() const { return mFreeSize; }
    //! Get file system read-only status
    bool ReadOnly() const { return mReadOnly; }

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
    std::string mType;       //!< FS type (ntfs, ext, ...)
    long long   mTotalSize;  //!< Total size in byte
    long long   mFreeSize;   //!< Free size in byte
    bool        mReadOnly;   //!< Read only?
};
