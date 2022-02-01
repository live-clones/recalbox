//
// Created by bkg2k on 10/03/19.
//

#include "DemoMode.h"
#include "utils/Log.h"
#include "systems/SystemManager.h"
#include "MainRunner.h"

DemoMode::DemoMode(WindowManager& window, SystemManager& systemManager)
  : mWindow(window),
    mSystemManager(systemManager),
    mRecalboxConf(RecalboxConf::Instance()),
    mDefaultDuration(0),
    mInfoScreenDuration(0),
    mRandomGenerator(mRandomDevice()),
    mGameRandomizer(0, 1U << 30U),
    mSeed(0)
{
}

void DemoMode::init()
{
  // Default duration
  mDefaultDuration = (int) mRecalboxConf.AsUInt("global.demo.duration", 90);
  mInfoScreenDuration = (int) mRecalboxConf.AsUInt("global.demo.infoscreenduration", 6);

  // flush previous init
  mDemoSystems.clear();
  mDurations.clear();

  // Build system list filtered by user config
  const std::vector<SystemData*>& allSystems = mSystemManager.GetAllSystemList();
  bool systemListExists = !mRecalboxConf.AsString("global.demo.systemlist").empty();
  for (int i=(int)allSystems.size(); --i>= 0;)
  {
    const std::string& name = allSystems[i]->Name();
    bool includeSystem = mRecalboxConf.AsBool(name + ".demo.include");
    bool systemIsInIncludeList = !systemListExists || mRecalboxConf.isInList("global.demo.systemlist", name);
    bool hasVisibleGame = allSystems[i]->HasVisibleGame();
    if ((includeSystem || systemIsInIncludeList) && hasVisibleGame)
    {
      mDemoSystems.push_back(allSystems[i]);
      int systemDuration = (int) mRecalboxConf.AsUInt(name + ".demo.duration", (unsigned int) mDefaultDuration);
      mDurations.push_back(systemDuration);
      { LOG(LogInfo) << "[DemoMode] System selected for demo : " << allSystems[i]->Name() << " with session of " << systemDuration << "s"; }
    }
    else { LOG(LogDebug) << "[DemoMode] System NOT selected for demo : " << allSystems[i]->Name() << " - isSelected: " << (includeSystem || systemIsInIncludeList) << " - HasVisibleGame: " << (hasVisibleGame ? 1 : 0); }
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
  for(int s = (int)PlatformIds::PlatformId::PLATFORM_COUNT; --s >= 0; )
  {
    mSystemHistory[s] = -1;
    for (int g = MAX_HISTORY; --g >= 0;)
      mGameHistories[s][g] = -1;
  }

  // Second seed - must never be negative
  mSeed = ((int)(DateTime() - DateTime(1970, 1, 1)).TotalMilliseconds()) & 0x7FFFFFFF;
}

bool DemoMode::hasDemoMode()
{
  return (RecalboxConf::Instance().GetScreenSaverType() == "demo");
}

bool DemoMode::isInHistory(int item, const int history[], int maxitems)
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

  for (int i = (int)PlatformIds::PlatformId::PLATFORM_COUNT; --i >= 0; )
  {
    // Select random system
    outputSystemIndex = mSystemRandomizer(mRandomGenerator);
    outputDuration = mDurations[outputSystemIndex];

    if (!isInHistory(outputSystemIndex, mSystemHistory, (int)mDemoSystems.size() / 2) || (mDemoSystems.size() == 1))
    {
      // Save history
      insertIntoHistory(outputSystemIndex, mSystemHistory);
      break;
    }
    // No luck, try again
  }
  { LOG(LogInfo) << "[DemoMode] Randomly selected system: " << mDemoSystems[outputSystemIndex]->Name() << " ("  << outputSystemIndex << "/" << (int)mDemoSystems.size() << ")"; }

  return true;
}

bool DemoMode::getRandomGame(FileData*& outputGame, int& outputDuration)
{
  int systemIndex = 0;
  if (!getRandomSystem(systemIndex, outputDuration)) { LOG(LogError) << "[DemoMode] NO system available for demo mode!"; return false; }
  SystemData* system = mDemoSystems[systemIndex];

  // Get filelist
  FileData::List gameList = system->getGames();
  if (gameList.empty()) { LOG(LogError) << "[DemoMode] NO game available for demo mode in system " << system->Name() << " !"; return false; }

  int gamePosition = 0;
  for (int i = MAX_HISTORY; --i >= 0; )
  {
    // Select game
    gamePosition = ((mGameRandomizer(mRandomGenerator) + i + mSeed) & 0x7FFFFFFF) % (int)gameList.size();
    outputGame = gameList[gamePosition];

    if (!isInHistory(gamePosition, mGameHistories[systemIndex], (int)gameList.size() / 2) || (gameList.size() == 1))
    {
      // Save history
      insertIntoHistory(gamePosition, mGameHistories[systemIndex]);
      break;
    }
    // No luck, try again
  }
  { LOG(LogInfo) << "[DemoMode] Randomly selected game: " << outputGame->FilePath().ToString() << " ("  << gamePosition << "/" << (int)gameList.size() << ")"; }

  mSeed++;
  return true;
}

void DemoMode::runDemo()
{
  if (!hasDemoMode()) return;

  FileData* game = nullptr;
  int duration = 0;
  bool Initialized = false;
  std::string controllerConfigs;

  init();

  Path mustExit(MainRunner::sQuitNow);
  while(getRandomGame(game, duration))
  {
    // Initialize (shutdown ES display)
    if (!Initialized)
    {
      controllerConfigs = GameRunner::Instance().demoInitialize();
      Initialized = true;
    }
    // Run game
    EmulatorData emulator = mSystemManager.Emulators().GetGameEmulator(*game);
    if (GameRunner::Instance().DemoRunGame(*game, emulator, duration, mInfoScreenDuration, controllerConfigs))
    {
      mWindow.DoWake();
      break;
    }
    // Exit required?
    if (mustExit.Exists()) break;
  }
  // Finalize (remount ES display)
  if (Initialized)
    GameRunner::Instance().demoFinalize();
}
