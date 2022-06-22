//
// Created by bkg2k on 22/06/22.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include "games/FileData.h"

/*!
 * @brief Adapt some methods from FileData to handle special cases
 * as for EasyRPG
 */
class GameAdapter
{
  public:
    explicit GameAdapter(const FileData& filedata)
      : mGame(filedata)
    {
    }

    //! Direct accessor
    const class FileData& FileData() const { return mGame; }

    //! Get scraping name
    std::string ScrapingName() const;

    //! Get displayable name if the game has no name
    std::string DisplayName() const;

    //! Get rom size
    long long RomSize();

    /*!
     * @brief Get the raw display filename if a rom from the given system
     * @param system System the rom belongs to
     * @param rompath Rom path
     * @return Name or empty string
     */
    static std::string RawDisplayName(SystemData& system, const Path& rompath) ;

  private:
    //! Easy RPG system name
    static const std::string sEasyRPGSystemName;
    //! Easy RPG special name
    static const std::string sEasyRPGGameName;

    //! Underlying FileData structure
    const class FileData& mGame;
};
