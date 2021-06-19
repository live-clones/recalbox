//
// Created by bkg2k on 01/11/2019.
//
#pragma once

#include <systems/PlatformId.h>
#include <utils/os/fs/Path.h>
#include "EmulatorList.h"

class SystemDescriptor
{
  public:
    //! Maximum platform Id (usually only one is used)
    static constexpr int sMaximumPlatformIds = 8;
    //! Platform array
    typedef PlatformIds::PlatformId PlateformIdentifiers[sMaximumPlatformIds];

    enum class SystemType
    {
      Unknown , //!< ?!
      Arcade  , //!< Arcade system
      Console , //!< Home console
      Handheld, //!< Handheld console
      Computer, //!< Computer
      Virtual , //!< Virtual system
      Engine  , //!< Game Engine
      Port    , //!< Port
    };

    //! Device requirement
    enum class DeviceRequirement
    {
      Unknown    , //!< ?!
      Required   , //!< Can't play whithout it!
      Recommended, //!< Most games require it
      Optional   , //!< Some games may require it
      None       , //!< Useless
    };

    /*!
     * @brief Default constructor
     */
    SystemDescriptor()
      : mPlatformIds{}
      , mPlateformCount(0)
      , mScreenScraperID(0)
      , mType(SystemType::Unknown)
      , mPad(DeviceRequirement::Unknown)
      , mKeyboard(DeviceRequirement::Unknown)
      , mMouse(DeviceRequirement::Unknown)
      , mLightgun(false)
      , mReadOnly(false)
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
                                           const std::string& fullname,
                                           const std::string& platforms,
                                           bool readonly)
    {
      mGUID = guid;
      mName = name;
      mFullName = fullname;
      mReadOnly = readonly;

      // Platform list
      for (const auto &platform : Strings::Split(platforms, ' '))
      {
        PlatformIds::PlatformId platformId = PlatformIds::getPlatformId(platform);
        if (platformId == PlatformIds::PlatformId::PLATFORM_IGNORE) { ClearPlatforms().AddPlatformIdentifiers(platformId); break; }
        if (!AddPlatformIdentifiers(platformId)) { LOG(LogError) << "[System] Platform count for system " << Name() << " full. " << platform << " ignored!"; }
      }

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
                                               const std::string& command)
    {
      mPath = Path(path);
      mExtensions = extensions;
      mThemeFolder = themefolder;
      mCommand = command;
      return *this;
    }

    /*!
     * @brief Set scraper information
     * @param screenscraperId ScreenScrapper system identifier
     * @return This
     */
    SystemDescriptor& SetScraperInformation(int screenscraperId)
    {
      mScreenScraperID = screenscraperId;
      return *this;
    }

    SystemDescriptor& SetPropertiesInformation(const std::string& systemtype,
                                               const std::string& pad,
                                               const std::string& keyboard,
                                               const std::string& mouse,
                                               bool lightgun)
    {
      mType = ConvertSystemType(systemtype);
      mPad = ConvertDeviceRequirement(pad);
      mKeyboard = ConvertDeviceRequirement(keyboard);
      mMouse = ConvertDeviceRequirement(mouse);
      mLightgun = lightgun;
      return *this;
    }

    //! Clear all platform entries
    SystemDescriptor& ClearPlatforms() { mPlateformCount = 0; return *this; }
    //! Clear all emulator entries
    SystemDescriptor& ClearEmulators() { mEmulators.Clear(); return *this; }

    /*!
     * @brief Set all platform identifiers
     * @param platformIdentifiers Platform identifiers
     */
    bool AddPlatformIdentifiers(PlatformIds::PlatformId platformIdentifier)
    {
      if (mPlateformCount < sMaximumPlatformIds)
      {
        mPlatformIds[mPlateformCount++] = platformIdentifier;
        return true;
      }
      return false;
    }

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

    const std::string& GUID() const { return mGUID; }
    const std::string& Name() const { return mName; }
    const std::string& FullName() const { return mFullName; }

    const Path& RomPath() const { return mPath; }
    const std::string& Extension() const { return mExtensions; }
    const std::string& ThemeFolder() const { return mThemeFolder; }
    const std::string& Command() const { return mCommand.empty() ? mDefaultCommand : mCommand; }

    int ScreenScaperID() const { return mScreenScraperID; }

    SystemType Type() const { return mType; }
    DeviceRequirement PadRequirement() const { return mPad; }
    DeviceRequirement KeyboardRequirement() const { return mKeyboard; }
    DeviceRequirement MouseRequirement() const { return mMouse; }
    bool LightGun() const { return mLightgun; }

    bool IsPort() const { return Strings::Contains(mPath.ToString(), "/ports/"); }
    bool IsReadOnly() const { return mReadOnly; }

    int PlatformCount() const { return mPlateformCount; }
    PlatformIds::PlatformId Platform(int index) const { return (unsigned int)index < (unsigned int)sMaximumPlatformIds ? mPlatformIds[index] : PlatformIds::PlatformId::PLATFORM_UNKNOWN; }
    bool HasPlatform(PlatformIds::PlatformId id) const
    {
      for(int i = mPlateformCount; --i >= 0;)
        if (id == mPlatformIds[i])
          return true;
      return false;
    }

    const EmulatorList& EmulatorTree() const { return mEmulators; }

  private:
    static std::string      mDefaultCommand;  //!< Default command

    // System
    std::string             mGUID;            //!< System GUID
    std::string             mName;            //!< Short name ("snes")
    std::string             mFullName;        //!< Full name ("Super Nintendo Entertainment System")
    PlateformIdentifiers    mPlatformIds;     //!< Platform identifiers
    int                     mPlateformCount;  //!< Platform count
    // Descriptor
    Path                    mPath;            //!< Rom path
    std::string             mThemeFolder;     //!< Theme sub-folder
    std::string             mExtensions;      //!< Supported extensions, space separated
    std::string             mCommand;         //!< Emulator command
    // Scrapper
    int                     mScreenScraperID; //!< ScreenScraper ID
    // Properties
    SystemType              mType;            //!< System type
    DeviceRequirement       mPad;             //!< Pad state
    DeviceRequirement       mKeyboard;        //!< Pad state
    DeviceRequirement       mMouse;           //!< Pad state
    bool                    mLightgun;        //!< Support lightgun?

    EmulatorList            mEmulators;      //!< Emulator/core tree

    bool                    mReadOnly;       //!< This system is a port and is readonly

    static SystemType ConvertSystemType(const std::string& systemtype)
    {
      SystemType result = SystemType::Unknown;
      if      (systemtype == "arcade"  ) result = SystemType::Arcade;
      else if (systemtype == "console" ) result = SystemType::Console;
      else if (systemtype == "handheld") result = SystemType::Handheld;
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
