//
// Created by bkg2k on 24/11/18.
//
#pragma once

#include <string>
#include <utils/storage/HashMap.h>
#include <systems/SystemData.h>

class GameNameMapManager
{
  private:
    //! MAME name count
    static constexpr int sMameListSize = 30444;
    //! Flashback name count
    static constexpr int sFlashbackListSize = 3;

    //! Mame names
    static const char* mMameNameToRealName[sMameListSize * 2];
    //! Mame hashed
    static int         mMameNameHashes[sMameListSize];

    //! Mame device files - TODO: use et set
    static HashMap<std::string, bool> mMameDevices;
    //! Mame bios files - TODO: use et set
    static HashMap<std::string, bool> mMameBios;

    //! Mame names
    static const char* mFlashbackNameToRealName[sFlashbackListSize * 2];
    //! Mame hashed
    static int         mFlashbackNameHashes[sFlashbackListSize];

    /*!
     * @brief Initialize all MAME hashes
     */
    static void GenerateMameNameHashes();

    /*!
     * @brief Initialize all MAME hashes
     */
    static void GenerateFlashbackNameHashes();

    /*!
     * @brief Get a MAME game name, given the file name
     * @param from Filename
     * @return Game name or nullptr
     */
    static const char* GetCleanMameName(const std::string& from);

    /*!
     * @brief Get a Flashback game name, given the file name
     * @param from Filename
     * @return Game name or nullptr
     */
    static const char* GetCleanFlashbackName(const std::string& from);

  public:

    /*!
     * @brief Chekc if the given system has internal game renaming
     * @param system System
     * @return True if the given system has internal game renaming
     */
    static bool HasRenaming(const SystemData& system);

    /*!
     * @brief Chekc if the given system has internal game filtering
     * @param system System
     * @return True if the given system has internal game filtering
     */
    static bool HasFiltering(const SystemData& system);

    /*!
     * @brief Check if the given game in the given system has to be filtered out
     * @param system System
     * @param filenameWoExt Game filename w/o extension
     * @return True if the game has to be filtered out
     */
    static bool IsFiltered(const SystemData& system, const std::string& filenameWoExt);

    /*!
     * @brief Rename game if required
     * @param system System
     * @param filenameWoExt Game filename w/o extension
     * @return Original or new game name
     */
    static std::string Rename(const SystemData& system, const std::string& filenameWoExt);
};