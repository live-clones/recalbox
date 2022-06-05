//
// Created by bkg2k on 21/02/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <string>
#include <vector>
#include "utils/Strings.h"
#include "utils/storage/HashMap.h"

class StorageDevices
{
  private:
    //! Size info
    struct SizeInfo
    {
      long long Size; //!< Size in Kbyte
      long long Free; //!< Free in Kbyte
      SizeInfo() : Size(0), Free(0) {}
      SizeInfo(long long s, long long f) : Size(s), Free(f) {}
    };

  public:
    //! Device Property Map type
    typedef HashMap<std::string, std::string>  PropertyMap;
    //! Device Property Map type
    typedef HashMap<std::string, struct SizeInfo>  DeviceSizeInfo;

    //! Storage types
    enum class Types
    {
      Internal, //!< Default partition
      Ram,      //!< tmpfs partition
      Network,  //!< Network share
      External, //!< External device
    };

    //! Storage descriptor
    struct Device
    {
      Types       Type;          //!< Device type
      std::string DevicePath;    //!< Device path, i.e. /dev/sda1
      std::string UUID;          //!< Device UUID
      std::string PartitionName; //!< Partition name
      std::string FileSystem;    //!< Partition filesystem
      std::string DisplayName;   //!< Displayable name
      long long   Size;          //!< Size in byte
      long long   Free;          //!< Free in byte
      bool        Current;       //!< True for the current device

      // Constructor
      Device()
        : Type(Types::Internal)
        , Size(0)
        , Free(0)
        , Current(false)
      {}

      // Constructor
      Device(Types t, const std::string& p, const std::string& u, const std::string& pn, const std::string& fs, const std::string& dn, bool c, const DeviceSizeInfo& i)
        : Type(t)
        , DevicePath(p)
        , UUID(u)
        , PartitionName(pn)
        , FileSystem(fs)
        , DisplayName(dn)
        , Size(0)
        , Free(0)
        , Current(c)
      {
        SizeInfo* info = i.try_get(p);
        if (info != nullptr)
        {
          Size = ((long long)info->Size) << 10;
          Free = ((long long)info->Free) << 10;
        }
      }

      std::string HumanSize() const { return Strings::ToHumanSize(Size); }

      std::string HumanFree() const { return Strings::ToHumanSize(Free); };

      std::string PercentFree() const { return Strings::ToString((int)(((double)Free / (double)Size) * 100.0)); }
    };

    StorageDevices()
      : mBootConfiguration(Path("/boot/recalbox-boot.conf"), false)
      , mShareInRAM(false)
    {
      Initialize();
    }

    /*!
     * @brief Get storage device list
     * @return Storage list
     */
    const std::vector<Device>& GetStorageDevices() const { return mDevices; }

    /*!
     * @brief Set storage device
     * @param device Device to set as share device
     */
    void SetStorageDevice(const Device& device);

    /*!
     * @brief Get share device
     * @return Device selected as share device
     */
    std::string GetStorageDevice();

  private:
    //! Share device key
    static constexpr const char* sShareDevice = "sharedevice";
    //! Internal string
    static constexpr const char* sInMemory = "RAM";
    //! Internal string
    static constexpr const char* sInternal = "INTERNAL";
    //! Any external string
    static constexpr const char* sAnyExternal = "ANYEXTERNAL";
    //! Network string
    static constexpr const char* sNetwork = "NETWORK";

    //! Boot configuration file
    IniFile mBootConfiguration;
    //! All devices
    std::vector<Device> mDevices;
    //! Boot root device name
    std::string mBootRoot;
    //! Share in ram?
    bool mShareInRAM;

    //! Initialize all devices
    void Initialize();

    //! Get raw output of the given command
    static Strings::Vector GetCommandOutput(const std::string& command);

    //! Get raw device list from blkid command
    static Strings::Vector GetRawDeviceList();

    //! Get mounted device list from mount command
    static Strings::Vector GetMountedDeviceList();

    //! Get file system info from df command
    static DeviceSizeInfo GetFileSystemInfo();

    //! Extract properies from the given string
    static PropertyMap ExtractProperties(const std::string& properties);

    //! Analyse mounted devices - Get boot device & check is share is in ram
    void AnalyseMounts();
};



