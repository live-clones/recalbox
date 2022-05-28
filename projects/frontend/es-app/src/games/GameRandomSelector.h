//
// Created by bkg2k on 27/01/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include "FileData.h"
#include <systems/SystemManager.h>
#include <systems/SystemData.h>
#include <utils/storage/Array.h>
#include <utils/storage/HashMap.h>
#include <random>

class GameRandomSelector
{
  public:
    //! Constructor
    explicit GameRandomSelector(SystemManager& systemManager, IFilter* filter);

    /*!
     * @brief Get next game
     * @return Next game
     */
    FileData* NextGame();

    //! Check if there is at least one non-empty system
    bool HasValidSystems() { return !mSystemArray.Empty(); }

    //! Initialize systemes
    void Initialize() { InitializeSystems(); }

  private:
    class SystemContent
    {
      public:
        //! Defgault constructor
        SystemContent()
          : mIndex(0)
        {
        }

        /*!
         * @brief Build game list from the given system using the given filter if not null
         * @param system System to get game from
         * @param filter Filter to apply of not null
         */
        SystemContent(const SystemData* system, IFilter* filter)
          : mIndex(0)
        {
          FileData::Filter excludes = system->Excludes();

          // Load games
          std::vector<FileData*> games = filter != nullptr ?
                                         system->MasterRoot().GetFilteredItemsRecursively(filter, false) :
                                         system->MasterRoot().GetAllItemsRecursively(false, excludes);
          mGames.AddItems(games.data(), (int)games.size());
          // Randomize list

          // Initialize randomize
          std::uniform_int_distribution<int> randomizer(0, mGames.Count() - 1);
          for(int i = (int)mGames.Count(); --i >= 0;)
          {
            int j = randomizer(sRandomGenerator);
            mGames.Swap(i, j);
          }
        }

        /*!
         * @brief Count games from the given system using the given filter if not null
         * @param system System to get game from
         * @param filter Filter to apply of not null
         */
        static int GetSystemGameCount(const SystemData* system, IFilter* filter)
        {
          FileData::Filter excludes = system->Excludes();
          // Load games
          return filter != nullptr ?
                 system->MasterRoot().CountFilteredItemsRecursively(filter, false) :
                 system->MasterRoot().CountAll(false, excludes);
        }

        /*!
         * @brief Pick next game
         * @return
         */
        FileData* NextGame()
        {
          if (--mIndex < 0) mIndex = mGames.Count() - 1;
          FileData* game = mGames[mIndex];
          { LOG(LogDebug) << "[GameRandomizer] Game selected: " << game->DisplayName() << " (" << game->FilePath().ToString() << ") in " << game->System().FullName(); }
          return game;
        }

      private:
        //! Map system => gamelist (lazy init)
        Array<FileData*> mGames;
        //! Current index
        int mIndex;
    };

    //! System manager instance
    SystemManager& mSystemManager;

    // Optionnal game filter
    IFilter* mFilter;

    //! Pre-randomized array of system
    //! System may appear one or mote times regarding the "weihgt"
    Array<const SystemData*> mSystemArray;

    //! Map system => gamelist (lazy init)
    HashMap<const SystemData*, SystemContent> mGamesMap;

    //! Random device to seed random generator
    static std::random_device sRandomDevice;
    //! Random generator
    static std::mt19937 sRandomGenerator;

    //! Current system index
    int mSystemIndex;

    /*!
     * @brief Initialize System list
     */
    void InitializeSystems();

    /*!
     * @brief Ensure the map is filled with game data and randomizer initialized, regarding the given system
     * @param system System to load games from
     */
    void EnsureSystemIsInitialized(const SystemData* system);
};



