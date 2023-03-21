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

  mGameSelector.Initialize();

  Path shouldStop(MainRunner::sStopDemo);
  shouldStop.Delete();
  { LOG(LogDebug) << "[DemoMode] Starting demo mode"; }

  for(FileData* game = mGameSelector.NextGame();
      game != nullptr;
      game = mGameSelector.NextGame())
  {
    // Initialize (shutdown ES display)
    if (!Initialized)
    {
      GameRunner::SubSystemPrepareForRun();
      Initialized = true;
    }

    int duration = RecalboxConf::Instance().GetSystemDemoDuration(game->System());
    // Run game
    EmulatorData emulator = mSystemManager.Emulators().GetGameEmulator(*game);
    if (GameRunner::Instance().DemoRunGame(*game, emulator, duration, mInfoScreenDuration) || shouldStop.Exists())
    {
      { LOG(LogDebug) << "[DemoMode] Stopping demo mode"; }
      mWindow.DoWake();
      break;
    }
  }
  // Finalize (remount ES display)
  if (Initialized)
    GameRunner::Instance().SubSystemRestore();
}
