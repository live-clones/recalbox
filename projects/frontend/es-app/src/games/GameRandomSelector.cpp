//
// Created by bkg2k on 27/01/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "GameRandomSelector.h"
#include <systems/SystemManager.h>
#include <cmath>

std::random_device GameRandomSelector::sRandomDevice;
std::mt19937 GameRandomSelector::sRandomGenerator(sRandomDevice());

GameRandomSelector::GameRandomSelector(SystemManager& systemManager, IFilter* filter, SystemData* systemData)
  : mSystemManager(systemManager)
  , mFilter(filter)
  , mSystemIndex(0)
  , mSystemData(systemData)

{

  InitializeSystems();
}

void GameRandomSelector::InitializeSystems()
{
  // Fill system array with weighted systems using a log(games)
  // A system with 50 games will appear 4 times in the list
  // A system with 5000 games will appear 9 times in the list
  for(const SystemData* system : mSystemManager.GetVisibleSystemList())
  {
    // for systemView video
    if(mSystemData != nullptr && mSystemData != system)
      continue;

    // Demo system?
    if (mSystemData == nullptr && !RecalboxConf::Instance().isInList("global.demo.systemlist", system->Name()) &&
        !RecalboxConf::Instance().GetSystemDemoInclude(*system)) continue;
    // Has game?
    int gameCount = SystemContent::GetSystemGameCount(system, mFilter);
    if (gameCount <= 0) continue;

    // Get system weight
    int weightedGameCount = (int)ceil(log((double)gameCount));
    { LOG(LogDebug) << "[GameRandomizer] System " << system->FullName() << " added with a weight of " << weightedGameCount << " for " << gameCount << " games."; }
    // ... and copy the system as many times as it's weighted
    for(; --weightedGameCount >= 0; ) mSystemArray.Add(system);
  }

  // Randomize the array
  std::uniform_int_distribution<int> randomizer(0, mSystemArray.Count() - 1);
  for(int i = mSystemArray.Count(); --i >= 0;)
  {
    int j = randomizer(sRandomGenerator);
    mSystemArray.Swap(i, j);
  }
}

void GameRandomSelector::EnsureSystemIsInitialized(const SystemData* system)
{
  // Not initialized?
  if (!mGamesMap.contains(system))
    mGamesMap[system] = SystemContent(system, mFilter);
}

FileData* GameRandomSelector::NextGame()
{
  // Move system index
  if (--mSystemIndex < 0) mSystemIndex = mSystemArray.Count() - 1;
  // Get system
  const SystemData* system = mSystemArray[mSystemIndex];
  // Lazy initialize
  EnsureSystemIsInitialized(system);
  // Get next games
  FileData* game = mGamesMap[system].NextGame();
  return game;
}

FileData* GameRandomSelector::NextGame(int& duration)
{
  FileData* result = NextGame();
  if (result != nullptr)
    duration = mGamesMap[&result->System()].Duration();

  return result;
}


