//
// Created by bkg2k on 10/03/19.
//

#include "DemoMode.h"
#include "Log.h"


DemoMode::DemoMode(Window& window)
  : mWindow(window),
    mSettings(*Settings::getInstance()),
    mRecalboxConf(*RecalboxConf::getInstance()),
    mRandomGenerator(mRandomRevice()),
    mGameRandomizer(0, 1 << 30)
{
  // Default duration
  mDefaultDuration = (int)mRecalboxConf.getUInt("global.demo.duration", 90);

  // Build system list filtered by user config
  const std::vector<SystemData*>& allSystems = SystemData::getAllSystems();
  bool systemListExists = !mRecalboxConf.get("global.demo.systemlist").empty();
  for (int i=(int)allSystems.size(); --i>= 0;)
  {
    const std::string& name = allSystems[i]->getName();
    bool includeSystem = mRecalboxConf.getBool(name + ".demo.include");
    bool systemIsInIncludeList = !systemListExists || mRecalboxConf.isInList("global.demo.systemlist", name);
    int gameCount = allSystems[i]->getRootFolder()->countAllDisplayableItemsRecursively(false);
    LOG(LogDebug) << "System elligible for Demo : " << allSystems[i]->getName() << " - isSelected: " << (includeSystem | systemIsInIncludeList) << " - gameCount: " << gameCount;
    if ((includeSystem || systemIsInIncludeList) && (gameCount > 0))
    {
      mDemoSystems.push_back(allSystems[i]);
      int systemDuration = mRecalboxConf.getUInt(name + ".demo.duration", (unsigned int)mDefaultDuration);
      mDurations.push_back(systemDuration);
      LOG(LogInfo) << "System selected for demo : " << allSystems[i]->getName() << " with session of " << systemDuration << "s";
    }
    else LOG(LogDebug) << "System NOT selected for demo : " << allSystems[i]->getName();
  }

  // Check if there is at least one system.
  // If not, get all system with no filtering
  if (mDemoSystems.empty())
    for (int i=(int)allSystems.size(); --i>= 0;)
    {
      mDemoSystems.push_back(allSystems[i]);
      mDurations.push_back(mDefaultDuration);
    }
  // Set system randomizer
  mSystemRandomizer = std::uniform_int_distribution<int>(0, (int)mDemoSystems.size() - 1);

  // Reset histories
  for(int s = (int)PlatformIds::PLATFORM_COUNT; --s >= 0; )
  {
    mSystemHistory[s] = -1;
    for (int g = MAX_HISTORY; --g >= 0;)
      mGameHistories[s][g] = -1;
  }
}

bool DemoMode::hasDemoMode()
{
  return (mSettings.getString("ScreenSaverBehavior") == "demo");
}

bool DemoMode::isInHistory(int item, int history[], int maxitems)
{
  int max = MAX_HISTORY < maxitems ? MAX_HISTORY : maxitems;
  for(int i = max; --i >= 0; )
    if (history[i] == item)
      return true;

  return false;
}

void DemoMode::insertIntoHistory(int item, int history[])
{
  memmove(&history[1], &history[0], (MAX_HISTORY - 1) * sizeof(int));
  history[0] = item;
}

bool DemoMode::getRandomSystem(int& outputSystemIndex, int& outputDuration)
{
  if (mDemoSystems.empty()) return false;

  for (int i = (int)PlatformIds::PLATFORM_COUNT; --i >= 0; )
  {
    // Select random system
    outputSystemIndex = mSystemRandomizer(mRandomGenerator);
    outputDuration = mDurations[outputSystemIndex];

    if (!isInHistory(outputSystemIndex, mSystemHistory, (int)mDemoSystems.size()) || (mDemoSystems.size() == 1))
    {
      // Save history
      insertIntoHistory(outputSystemIndex, mSystemHistory);
      break;
    }
    // No luck, try again
  }
  LOG(LogInfo) << "Randomly selected system: " << mDemoSystems[outputSystemIndex]->getName();
  //std::cout << "Randomly selected system: " << mDemoSystems[outputSystemIndex]->getName() << "\r\n";

  return true;
}

bool DemoMode::getRandomGame(FileData*& outputGame, int& outputDuration)
{
  int systemIndex = 0;
  if (!getRandomSystem(systemIndex, outputDuration)) return false;
  SystemData* system = mDemoSystems[systemIndex];

  // Get filelist
  FileData::List gameList = system->getRootFolder()->getAllDisplayableItemsRecursively(false);
  if (gameList.empty()) return false;

  for (int i = MAX_HISTORY; --i >= 0; )
  {
    // Select game
    int gamePosition = mGameRandomizer(mRandomGenerator) % (int)gameList.size();
    outputGame = gameList[gamePosition];

    if (!isInHistory(gamePosition, mGameHistories[systemIndex], (int)gameList.size()) || (gameList.size() == 1))
    {
      // Save history
      insertIntoHistory(gamePosition, mGameHistories[systemIndex]);
      break;
    }
    // No luck, try again
  }
  LOG(LogInfo) << "Randomly selected game: " << outputGame->getPath().generic_string();
  //std::cout << "Randomly selected game: " << outputGame->getPath().generic_string() << "\r\n";;

  return true;
}

void DemoMode::runDemo()
{
  if (!hasDemoMode()) return;

  SystemData* system = nullptr;
  FileData* game = nullptr;
  int duration = 0;
  bool Initialized = false;
  std::string controllerConfigs;

  while(getRandomGame(game, duration))
  {
    // Get game's parent system
    system = game->getSystem();
    // Initialize (shutdown ES display)
    if (!Initialized)
    {
      controllerConfigs = system->demoInitialize(mWindow);
      Initialized = true;
    }
    // Run game
    if (system->demoLaunchGame(game, duration, controllerConfigs))
    {
      mWindow.doWake();
      break;
    }
  }
  // Finalize (remount ES display)
  if (Initialized)
    system->demoFinalize(mWindow);
}




