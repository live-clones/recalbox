//
// Created by bkg2k on 10/03/19.
//

#include "DemoMode.h"
#include "Log.h"


DemoMode::DemoMode(Window& window)
  : mWindow(window),
    mSettings(*Settings::getInstance()),
    mRecalboxConf(*RecalboxConf::getInstance()),
    mRandomGenerator(mRandomRevice())
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
    if ((includeSystem || systemIsInIncludeList) &&
        (allSystems[i]->getRootFolder()->countAllDisplayableItemsRecursively(false) > 0))
    {
      mDemoSystems.push_back(allSystems[i]);
      int systemDuration = mRecalboxConf.getUInt(name + ".demo.duration", (unsigned int)mDefaultDuration);
      mDurations.push_back(systemDuration);
    }
  }

  // Check if there is at least one system.
  // If not, get all system with no filtering
  if (mDemoSystems.empty())
    for (int i=(int)allSystems.size(); --i>= 0;)
    {
      mDemoSystems.push_back(allSystems[i]);
      mDurations.push_back(mDefaultDuration);
    }
}

bool DemoMode::hasDemoMode()
{
  return (mSettings.getString("ScreenSaverBehavior") == "demo");
}

bool DemoMode::getRandomSystem(SystemData*& outputSystem, int& outputDuration)
{
  if (mDemoSystems.empty()) return false;

  std::uniform_int_distribution<int> random(0, (int)mDemoSystems.size() - 1);
  for (int i=5; --i>=0; ) // Maximum 5 tries to find a new game to launch
  {
    // Select random system
    int randomPosition = random(mRandomGenerator);
    outputSystem = mDemoSystems[randomPosition];
    outputDuration = mDurations[randomPosition];
    std::string outputName = outputSystem->getName();

    if ((outputName != mLastSystem) || (mDemoSystems.size() == 1))
    {
      // Set new game path
      mLastSystem = outputName;
      break;
    }
    // No luck, try again
  }
  LOG(LogInfo) << "Randomly selected system: " << outputSystem->getName();
  //std::cout << "Randomly selected system: " << outputSystem->getName() << "\r\n";

  return true;
}

bool DemoMode::getRandomGame(FileData*& outputGame, int& outputDuration)
{
  SystemData* system = nullptr;
  if (!getRandomSystem(system, outputDuration)) return false;

  // Get filelist
  FileData::List gamelist = system->getRootFolder()->getAllDisplayableItemsRecursively(false);
  if (gamelist.empty()) return false;

  std::uniform_int_distribution<int> random(0, (int)gamelist.size() - 1);
  for (int i=5; --i>=0; ) // Maximum 5 tries to find a new game to launch
  {
    // Select game
    outputGame = gamelist[random(mRandomGenerator)];
    std::string outputPath = outputGame->getPath().generic_string();

    if ((outputPath != mLastGamePath) || (gamelist.size() == 1))
    {
      // Set new game path
      mLastGamePath = outputPath;
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

  FileData* game = nullptr;
  int duration = 0;
  bool Initialized = false;
  SystemData* system = nullptr;

  while(getRandomGame(game, duration))
  {
    // Get game's parent system
    system = game->getSystem();
    // Initialize (shutdown ES display)
    if (!Initialized)
    {
      system->demoInitialize(mWindow);
      Initialized = true;
    }
    // Run game
    if (system->demoLaunchGame(game, duration))
    {
      mWindow.doWake();
      break;
    }
  }
  // Finalize (remount ES display)
  if (Initialized)
    system->demoFinalize(mWindow);
}




