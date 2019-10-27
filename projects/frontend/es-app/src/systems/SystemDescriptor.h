//
// Created by bkg2k on 01/11/2019.
//
#pragma once

#include <utils/os/fs/Path.h>
#include "EmulatorList.h"

class SystemDescriptor
{
  public:
    //! Maximum platform Id (usually only one is used)
    static constexpr int sMaximumPlatformIds = 8;

    typedef PlatformIds::PlatformId PlateformIdentifiers[sMaximumPlatformIds];

  private:
    Path                    mPath;           //!< Rom path
    std::string             mName;           //!< Short name ("snes")
    std::string             mFullName;       //!< Full name ("Super Nintendo Entertainment System")
    std::string             mCommand;        //!< Emulator command
    std::string             mExtensions;     //!< Supported extensions, space separated
    std::string             mThemeFolder;    //!< Theme sub-folder

    int                     mPlateformCount; //!< Platform count
    PlateformIdentifiers    mPlatformIds;    //!< Platform identifiers
    EmulatorList            mEmulators;      //!< Emulator/core tree

  public:
    /*!
     * @brief Default constructor
     */
    SystemDescriptor()
      : mPlateformCount(0),
        mPlatformIds{}
    {
    }

    /*!
     * @brief Set basic informations
     * @param path Rom path
     * @param name Short name ("snes")
     * @param fullname Full name ("Super Nintendo Entertainment System")
     * @param command Emulator command
     * @param extensions Supported extensions, space separated
     * @param theme Theme sub-folder
     */
    void SetInformation(const std::string& path,
                        const std::string& name,
                        const std::string& fullname,
                        const std::string& command,
                        const std::string& extensions,
                        const std::string& theme)
    {
      mPath = Path(path);
      mName = name;
      mFullName = fullname;
      mCommand = command;
      mExtensions = extensions;
      mThemeFolder = theme;
    }

    void ClearPlatforms() { mPlateformCount = 0; }
    void ClearEmulators() { mEmulators.Clear(); }

    /*!
     * @brief Reset theme folder to its default state
     */
    void SetDefaultThemePath() { mThemeFolder = "default"; }

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

    /*!
     * @brief Check of the current descriptor is valid and contains all required informations
     * @return True if the descriptor is valid
     */
    bool IsValid()
    {
      return !mName.empty() && !mPath.Empty() && !mExtensions.empty() && !mCommand.empty();
    }

    /*
     * Accessors
     */

    const Path& RomPath() const { return mPath; }
    const std::string& Name() const { return mName; }
    const std::string& FullName() const { return mFullName; }
    const std::string& Command() const { return mCommand; }
    const std::string& Extension() const { return mExtensions; }
    const std::string& ThemeFolder() const { return mThemeFolder; }

    int PlatformCount() const { return mPlateformCount; }
    PlatformIds::PlatformId Platform(int index) const { return (unsigned int)index < (unsigned int)sMaximumPlatformIds ? mPlatformIds[index] : PlatformIds::PlatformId::PLATFORM_UNKNOWN; }
    bool HasPlatform(PlatformIds::PlatformId id)
    {
      for(int i = mPlateformCount; --i >= 0;)
        if (id == mPlatformIds[i])
          return true;
      return false;
    }

    const EmulatorList& EmulatorTree() const { return mEmulators; }
};