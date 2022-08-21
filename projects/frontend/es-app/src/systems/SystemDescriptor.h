//
// Created by bkg2k on 01/11/2019.
//
#pragma once

#include <systems/PlatformId.h>
#include <utils/os/fs/Path.h>
#include <emulators/EmulatorList.h>

class SystemDescriptor
{
  public:
    enum class SystemType
    {
      Unknown , //!< ?!
      Arcade  , //!< Arcade system
      Console , //!< Home console
      Handheld, //!< Handheld console
      Fantasy,  //!< Fantasy console (no real hardware)
      Computer, //!< Computer
      Virtual , //!< Virtual system
      Engine  , //!< Game Engine
      Port    , //!< Port
    };

    //! Device requirement
    enum class DeviceRequirement
    {
      Unknown    , //!< ?!
      Required   , //!< Can't play without it!
      Recommended, //!< Most games require it
      Optional   , //!< Some games may require it
      None       , //!< Useless
    };

    /*!
     * @brief Default constructor
     */
    SystemDescriptor()
      : mIcon(0)
      , mScreenScraperID(0)
      , mReleaseDate(0)
      , mManufacturer()
      , mType(SystemType::Unknown)
      , mPad(DeviceRequirement::Unknown)
      , mKeyboard(DeviceRequirement::Unknown)
      , mMouse(DeviceRequirement::Unknown)
      , mLightgun(false)
      , mCrtInterlaced(false)
      , mCrtMultiRegion(false)
      , mPort(false)
      , mReadOnly(false)
      , mIgnoredFiles()
    {
    }

    /*!
     * @brief Set system informations
     * @param guid GUID
     * @param name Short name ("snes")
     * @param fullname Full name ("Super Nintendo Entertainment System")
     * @param readonly Read only system
     * @return This
     */
    SystemDescriptor& SetSystemInformation(const std::string& guid,
                                           const std::string& name,
                                           const std::string& fullname)
    {
      mGUID = guid;
      mName = name;
      mFullName = fullname;

      return *this;
    }

    /*!
     * @brief Set descriptor info
     * @param path Rom path
     * @param extensions supported extensions
     * @param themefolder Theme sub folder
     * @param command Command (may be empty to take the default global command)
     * @return This
     */
    SystemDescriptor& SetDescriptorInformation(const std::string& path,
                                               const std::string& extensions,
                                               const std::string& themefolder,
                                               const std::string& command,
                                               const std::string& icon,
                                               bool port,
                                               bool readonly)
    {
      mPath = Path(path);
      mExtensions = extensions;
      mThemeFolder = themefolder;
      mCommand = command;
      mPort = port;
      mReadOnly = readonly;
      if (!Strings::HexToInt(icon, 0, mIcon)) mIcon = 0;

      return *this;
    }

    /*!
     * @brief Set scraper information
     * @param screenscraperId ScreenScraper system identifier
     * @return This
     */
    SystemDescriptor& SetScraperInformation(int screenscraperId)
    {
      mScreenScraperID = screenscraperId;
      return *this;
    }

    /*!
     * @brief Set property information
     * @param systemtype System type
     * @param pad Pad requirement
     * @param keyboard Keyboard requirement
     * @param mouse Mouser requirement
     * @param releasedate Release data
     * @param manufacturer Manufacturer
     * @param lightgun Has lightgun support?
     * @param multiresolution Support CRT Multi resolution?
     * @param multiregion Support multi region?
     * @param ignoredfiles Ignored files
     * @return This
     */
    SystemDescriptor& SetPropertiesInformation(const std::string& systemtype,
                                               const std::string& pad,
                                               const std::string& keyboard,
                                               const std::string& mouse,
                                               const std::string& releasedate,
                                               const std::string& manufacturer,
                                               bool lightgun,
                                               bool multiresolution,
                                               bool multiregion,
                                               const std::string& ignoredfiles)
    {
      mType = ConvertSystemType(systemtype);
      mPad = ConvertDeviceRequirement(pad);
      mKeyboard = ConvertDeviceRequirement(keyboard);
      mMouse = ConvertDeviceRequirement(mouse);
      if (!Strings::ToInt(Strings::Replace(releasedate, "-", ""), mReleaseDate))
        mReleaseDate = 0;
      mManufacturer = manufacturer;
      mLightgun = lightgun;
      mCrtInterlaced = multiresolution;
      mCrtMultiRegion = multiregion;
      mIgnoredFiles = ignoredfiles;
      return *this;
    }

    /*!
     * @brief Set property information for virtual systems
     * @param systemtype System type (virtual most of the time, but arcade for all arcade)
     * @param pad
     * @param keyboard
     * @param mouse
     * @return
     */
    SystemDescriptor& SetVirtualPropertiesInformation(SystemType systemtype,
                                                      DeviceRequirement keyboard,
                                                      DeviceRequirement mouse,
                                                      DeviceRequirement pad)
    {
      mType = systemtype;
      mPad = pad;
      mKeyboard = keyboard;
      mMouse = mouse;
      mReleaseDate = 0;
      mManufacturer = "Virtual";
      mLightgun = false;
      mCrtInterlaced = false;
      mCrtMultiRegion = false;
      mIgnoredFiles.clear();
      return *this;
    }

    //! Clear all emulator entries
    SystemDescriptor& ClearEmulators() { mEmulators.Clear(); return *this; }

    /*!
     * @brief Set the whole emulator tree
     * @param emulatorList Emulator/core tree
     */
    void SetEmulatorList(const EmulatorList& emulatorList)
    {
      mEmulators = emulatorList;
    }

    static void SetDefaultCommand(const std::string& command) { mDefaultCommand = command; }

    /*!
     * @brief Check of the current descriptor is valid and contains all required informations
     * @return True if the descriptor is valid
     */
    bool IsValid()
    {
      return !mGUID.empty() && !mName.empty() && !mPath.IsEmpty() && !mExtensions.empty() && !Command().empty();
    }

    /*
     * Accessors
     */

    [[nodiscard]] const std::string& GUID() const { return mGUID; }
    [[nodiscard]] const std::string& Name() const { return mName; }
    [[nodiscard]] const std::string& FullName() const { return mFullName; }

    [[nodiscard]] const Path& RomPath() const { return mPath; }
    [[nodiscard]] const std::string& Extension() const { return mExtensions; }
    [[nodiscard]] const std::string& ThemeFolder() const { return mThemeFolder; }
    [[nodiscard]] const std::string& Command() const { return mCommand.empty() ? mDefaultCommand : mCommand; }
    [[nodiscard]] unsigned int Icon() const { return (unsigned int)mIcon; }
    [[nodiscard]] std::string IconPrefix() const;

    [[nodiscard]] int ScreenScaperID() const { return mScreenScraperID; }

    [[nodiscard]] int ReleaseDate() const { return mReleaseDate; }
    [[nodiscard]] const std::string& Manufacturer() const { return mManufacturer; }

    [[nodiscard]] SystemType Type() const { return mType; }
    [[nodiscard]] DeviceRequirement PadRequirement() const { return mPad; }
    [[nodiscard]] DeviceRequirement KeyboardRequirement() const { return mKeyboard; }
    [[nodiscard]] DeviceRequirement MouseRequirement() const { return mMouse; }
    [[nodiscard]] bool LightGun() const { return mLightgun; }
    [[nodiscard]] bool CrtHighResolution() const { return mCrtInterlaced; }
    [[nodiscard]] bool CrtMultiRegion() const { return mCrtMultiRegion; }
    [[nodiscard]] const std::string& IgnoredFiles() const { return mIgnoredFiles; }

    [[nodiscard]] bool HasNetPlayCores() const
    {
      for(int i = mEmulators.Count(); --i >= 0; )
        for(int j = mEmulators.EmulatorAt(i).CoreCount(); --j >= 0; )
          if (mEmulators.EmulatorAt(i).CoreNetplay(j))
            return true;
      return false;
    }

    [[nodiscard]] bool IsSoftpatching(const std::string& emulatorName, const std::string& coreName) const
    {
      for(int i = mEmulators.Count(); --i >= 0; )
      {
        if(mEmulators.EmulatorAt(i).Name() != emulatorName) continue;
        for (int j = mEmulators.EmulatorAt(i).CoreCount(); --j >= 0;)
          if (mEmulators.EmulatorAt(i).CoreNameAt(j) == coreName && mEmulators.EmulatorAt(i).CoreSoftpatching(j))
            return true;
      }
      return false;
    }

    [[nodiscard]] bool IsPort() const { return mPort; }
    [[nodiscard]] bool IsReadOnly() const { return mReadOnly; }

    [[nodiscard]] const EmulatorList& EmulatorTree() const { return mEmulators; }

  private:
    static std::string      mDefaultCommand;  //!< Default command

    // System
    std::string             mGUID;            //!< System GUID
    std::string             mName;            //!< Short name ("snes")
    std::string             mFullName;        //!< Full name ("Super Nintendo Entertainment System")
    // Descriptor
    Path                    mPath;            //!< Rom path
    std::string             mThemeFolder;     //!< Theme sub-folder
    std::string             mExtensions;      //!< Supported extensions, space separated
    std::string             mCommand;         //!< Emulator command
    int                     mIcon;            //!< Icon unicode char
    // Scraper
    int                     mScreenScraperID; //!< ScreenScraper ID
    // Properties
    int                     mReleaseDate;     //!< Release date in numeric format yyyymm
    std::string             mManufacturer;    //!< Manufacturer ("Nintendo")
    SystemType              mType;            //!< System type
    DeviceRequirement       mPad;             //!< Pad state
    DeviceRequirement       mKeyboard;        //!< Pad state
    DeviceRequirement       mMouse;           //!< Pad state
    bool                    mLightgun;        //!< Support lightgun ?
    bool                    mCrtInterlaced;   //!< Support 480i/p ?
    bool                    mCrtMultiRegion;  //!< PAL/NTSC ?

    EmulatorList            mEmulators;       //!< Emulator/core tree

    bool                    mPort;            //!< This system is a port
    bool                    mReadOnly;        //!< This system is a port and is readonly
    std::string             mIgnoredFiles;    //!< Ignored files list

    static SystemType ConvertSystemType(const std::string& systemtype)
    {
      SystemType result = SystemType::Unknown;
      if      (systemtype == "arcade"  ) result = SystemType::Arcade;
      else if (systemtype == "console" ) result = SystemType::Console;
      else if (systemtype == "handheld") result = SystemType::Handheld;
      else if (systemtype == "fantasy" ) result = SystemType::Fantasy;
      else if (systemtype == "computer") result = SystemType::Computer;
      else if (systemtype == "virtual" ) result = SystemType::Virtual;
      else if (systemtype == "engine"  ) result = SystemType::Engine;
      else if (systemtype == "port"    ) result = SystemType::Port;
      else { LOG(LogError) << "[SystemDescriptor] Unknown system type " << systemtype << " !"; }
      return result;
    }

    static DeviceRequirement ConvertDeviceRequirement(const std::string& requirement)
    {
      DeviceRequirement result = DeviceRequirement::Unknown;
      if      (requirement == "no"         ) result = DeviceRequirement::None;
      else if (requirement == "optional"   ) result = DeviceRequirement::Optional;
      else if (requirement == "recommended") result = DeviceRequirement::Recommended;
      else if (requirement == "mandatory"  ) result = DeviceRequirement::Required;
      else { LOG(LogError) << "[SystemDescriptor] Unknown device requirement " << requirement << " !"; }
      return result;
    }
};
