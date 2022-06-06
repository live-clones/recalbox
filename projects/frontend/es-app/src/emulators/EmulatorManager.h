//
// Created by Bkg2k on 18/02/2020.
//
#pragma once

#include <utils/cplusplus/INoCopy.h>
#include <utils/storage/HashMap.h>
#include <systems/SystemData.h>
#include <games/FileData.h>
#include "EmulatorList.h"
#include <emulators/EmulatorData.h>

class EmulatorManager : public INoCopy
{
  private:
    //! System to emulator/core list
    HashMap<std::string, const EmulatorList*> mSystemEmulators;

    /*!
     * @brief Get an unique key from the given system
     * @param system System
     * @return Resulting key
     */
    static std::string KeyFrom(const SystemData& system);

    /*!
     * @brief Check if the given emulator and core exists for the given system
     * @param system System to check against emulator/code
     * @param emulator Emulator name to check
     * @param core Core name to check
     * @return True if both emulator/core exist for the given system, false otherwise
     */
    bool CheckEmulatorAndCore(const SystemData& system, const std::string& emulator, const std::string& core) const;

    /*!
     * @brief Try to guess either the emulator or the core name for the given system, using the following algorithm:
     * 1. Emulator is non empty, core is empty:
     *   If the emulator exists for the given system *and* there is only one core, populate the core
     * 2. Emulator is empty, core is non empty:
     *   Look in all emulators/cores trying to fond the core. If found, populate the emulator
     * @param system System from witch to guess emulator/core
     * @param emulator Emulator name or empty
     * @param core Core name or empty
     * @return True if either core or emulator has been guessed, false otherwise
     */
    bool GuessEmulatorAndCore(const SystemData& system, std::string& emulator, std::string& core) const;

    /*!
     * @brief Get default emulator/core for the given system
     * @param system System to get default emulator/core from
     * @param emulator Filled in Emulator name
     * @param core Filled in Core name
     * @return True if emulator and core have been filled, false otherwise (error, non-existing system, ...)
     */
    bool GetSystemDefaultEmulator(const SystemData& system, std::string& emulator, std::string& core) const;

    /*!
     * @brief Try to override emulator/core from values from recalbox.conf file
     * @param system System to extract emulator/core from
     * @param emulator Emulator name to override if '<system>.emulator=value' exists in the configuration file
     * @param core Core name to override if '<system>.core=value' exists in the configuration file
     */
    void GetEmulatorFromConfigFile(const SystemData& system, std::string& emulator, std::string& core) const;

    /*!
     * @brief Try to get emulator/core override from the given game
     * @param game Game to extract overrides from metadata
     * @param emulator Emulator name to override with the one from game's metadata if non empty
     * @param core Core name to override with the one from game's metadata if non empty
     */
    void GetEmulatorFromGamelist(const FileData& game, std::string& emulator, std::string& core) const;

    /*!
     * @brief Try to get emulator/core override from overrides files (.recalbox.conf) in the rom path
     * @param game Game from whitch to get rom path
     * @param emulator Emulator name to override with the one(s) from override files if they exists
     * @param core Core name to override with the one(s) from override files if they exists
     */
    void GetEmulatorFromOverride(const FileData& game, std::string& emulator, std::string& core) const;

    /*!
     * @brief Add a new emulator list for the given system
     * @param system System
     * @param list Emulator (and core) list
     */
    void AddEmulatorList(const SystemData& system)
    {
      std::string key = KeyFrom(system);
      if (mSystemEmulators.contains(key))
      {
        { LOG(LogError) << "[Emulator] Fatal error: You cannot define 2 systems with the same fullname and the same platforms! ABORTING."; }
        exit(1);
      }
      mSystemEmulators.insert(key, &system.Emulators());
    }

    /*!
     * @brief Patch name if required (example: libretro-duckstation renamed libretro-swanstation in 7.2.1)
     * @param emulator emulator name
     * @param core core name
     */
    static void PatchNames(std::string& emulator, std::string& core);

    //! Class SystemManager needs to Add emulator lists
    friend class SystemManager;

  public:

    /*!
     * @brief Get default emulator/core for the given system
     * @param system System to get default emulator/core from
     * @param emulator Filled in Emulator name
     * @param core Filled in Core name
     * @return True if emulator and core have been filled, false otherwise (error, non-existing system, ...)
     */
    bool GetDefaultEmulator(const SystemData& system, std::string& emulator, std::string& core) const;

    /*!
     * @brief Get final emulator/core names used to run the given game
     * this method explore all config files and override to find out final names
     * @param game Game to get emulator/core for
     * @param emulator Emulator name
     * @param core Core name
     * @return True if emulator/core have been filled properly, false otherwise (error, non-existing system, ...)
     */
    bool GetGameEmulator(const FileData& game, std::string& emulator, std::string& core) const;

    /*!
     * @brief Get final emulator/core names used to run the given game
     * this method explore all config files and override to find out final names
     * @param game Game to get emulator/core for
     * @return Emulator/Core holder. May be invalid.
     */
    EmulatorData GetGameEmulator(const FileData& game) const;

    /*!
     * @brief Get emulator list for the given system. The first item is the default emulator
     * @param system System to get emulator list from
     * @return Emulator list
     */
    Strings::Vector GetEmulators(const SystemData& system) const;

    /*!
     * @brief Get core list for the given system and the given emulator
     * @param system System to get emulator and core from
     * @param emulator Emulator ro get core list from
     * @return Core list
     */
    Strings::Vector GetCores(const SystemData& system, const std::string& emulator) const;
};
