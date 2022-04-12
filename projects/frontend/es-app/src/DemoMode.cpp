//
// Created by bkg2k on 10/03/19.
//

#include "DemoMode.h"
#include "utils/Log.h"
#include "systems/SystemManager.h"
#include "MainRunner.h"

DemoMode::DemoMode(WindowManager& window, SystemManager& systemManager)
  : mWindow(window)
  , mSystemManager(systemManager)
  , mGameSelector(systemManager, nullptr)
  , mInfoScreenDuration(RecalboxConf::Instance().GetGlobalDemoInfoScreen())
{
}

bool DemoMode::hasDemoMode()
{
  return (RecalboxConf::Instance().GetScreenSaverType() == "demo");
}

void DemoMode::runDemo()
{
  if (!hasDemoMode()) return;

  bool Initialized = false;
  std::string controllerConfigs;

  mGameSelector.Initialize();

  Path mustExit(MainRunner::sQuitNow);
  for(FileData* game = mGameSelector.NextGame();
      game != nullptr;
      game = mGameSelector.NextGame())
  {
    // Initialize (shutdown ES display)
    if (!Initialized)
    {
      controllerConfigs = GameRunner::demoInitialize();
      Initialized = true;
    }

    int duration = RecalboxConf::Instance().GetSystemDemoDuration(game->System());
    // Run game
    EmulatorData emulator = mSystemManager.Emulators().GetGameEmulator(*game);
    if (GameRunner::DemoRunGame(*game, emulator, duration, mInfoScreenDuration, controllerConfigs))
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
