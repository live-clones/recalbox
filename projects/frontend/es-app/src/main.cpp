//EmulationStation, a graphical front-end for ROM browsing. Created by Alec "Aloshi" Lofquist.
//http://www.aloshi.com

#include <EmulationStation.h>
#include <utils/sdl2/SyncronousEventService.h>
#include <utils/Log.h>
#include "MainRunner.h"

bool parseArgs(int argc, char* argv[], unsigned int& width, unsigned int& height, bool& windowed)
{
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "--resolution") == 0)
    {
      if (i >= argc - 2)
      {
        { LOG(LogError) << "[Params] Invalid resolution supplied."; }
        return false;
      }

      char* err = nullptr;
      width = (unsigned int)strtol(argv[i + 1], &err, 10);
      height = (unsigned int)strtol(argv[i + 2], &err, 10);
      i += 2; // skip the argument value
    }
    else if (strcmp(argv[i], "--draw-framerate") == 0)
    {
      RecalboxConf::Instance().SetGlobalShowFPS(true);
    }
    else if (strcmp(argv[i], "--debug") == 0)
    {
      Log::setReportingLevel(LogLevel::LogDebug);
    }
    else if (strcmp(argv[i], "--windowed") == 0)
    {
      windowed = true;
    }
    else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
    {
      printf("EmulationStation, a graphical front-end for ROM browsing.\n"
             "Written by Alec \"Aloshi\" Lofquist.\n"
             "Version " PROGRAM_VERSION_STRING ", built " PROGRAM_BUILT_STRING "\n\n"
             "Command line arguments:\n"
             "--resolution [width] [height]	try and force a particular resolution\n"
             "--draw-framerate		display the framerate\n"
             "--debug				more logging, show console on Windows\n"
             "--windowed			not fullscreen, should be used with --resolution\n"
             "--help, -h			summon a sentient, angry tuba\n\n"
             "More information available in README.md.\n");
      return false; //exit after printing help
    }
  }

  return true;
}

int main(int argc, char* argv[], char** env)
{
  Log::open();

  // Get arguments
  unsigned int width = 0;
  unsigned int height = 0;
  bool windowed = false;
  if (!parseArgs(argc, argv, width, height, windowed))
    return 0;

  for(int loopCount = 0;; ++loopCount)
  {
    // Start the runner
    MainRunner runner(argv[0], width, height, windowed, loopCount, env);
    MainRunner::ExitState exitState = runner.Run();

    { LOG(LogInfo) << "[Main] EmulationStation cleanly shutting down."; }
    switch(exitState)
    {
      case MainRunner::ExitState::Quit:
      case MainRunner::ExitState::FatalError:
      {
        { LOG(LogInfo) << "[Main] Regular Quit (or Fatal Error)"; }
        return 0;
      }
      case MainRunner::ExitState::Relaunch:
      case MainRunner::ExitState::RelaunchNoUpdate: continue;
      case MainRunner::ExitState::NormalReboot:
      case MainRunner::ExitState::FastReboot:
      {
        { LOG(LogInfo) << "[Main] Rebooting system"; }
        #ifndef DEBUG
        if (system("reboot") != 0)
          { LOG(LogError) << "[Main] Error rebooting system"; }
        #else
        { LOG(LogError) << "[Main] Fake reboot performed!"; }
        #endif
        return 0;
      }
      case MainRunner::ExitState::Shutdown:
      case MainRunner::ExitState::FastShutdown:
      {
        { LOG(LogInfo) << "[Main] Rebooting system (fast)"; }
        #ifndef DEBUG
        if (system("shutdown -h now") != 0)
          { LOG(LogError) << "[Main] Error shutting system down"; }
        #else
        { LOG(LogError) << "[Main] Fake shutdown performed!"; }
        #endif
        return 0;
      }
    }
  }
}

