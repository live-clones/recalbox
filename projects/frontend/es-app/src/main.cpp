//EmulationStation, a graphical front-end for ROM browsing. Created by Alec "Aloshi" Lofquist.
//http://www.aloshi.com

#include <EmulationStation.h>
#include <utils/sdl2/SyncronousEventService.h>
#include <Settings.h>
#include <utils/Log.h>
#include "MainRunner.h"

bool parseArgs(int argc, char* argv[], unsigned int& width, unsigned int& height, Settings& settings)
{
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "--resolution") == 0)
    {
      if (i >= argc - 2)
      {
        LOG(LogError) << "Invalid resolution supplied.";
        return false;
      }

      char* err = nullptr;
      width = (unsigned int)strtol(argv[i + 1], &err, 10);
      height = (unsigned int)strtol(argv[i + 2], &err, 10);
      i += 2; // skip the argument value
    }
    else if (strcmp(argv[i], "--ignore-gamelist") == 0)
    {
      settings.SetIgnoreGamelist(true);
    }
    else if (strcmp(argv[i], "--draw-framerate") == 0)
    {
      settings.SetDrawFramerate(true);
    }
    else if (strcmp(argv[i], "--no-exit") == 0)
    {
      settings.SetShowExit(false);
    }
    else if (strcmp(argv[i], "--debug") == 0)
    {
      settings.SetDebug(true);
      settings.SetHideConsole( false);
      Log::setReportingLevel(LogLevel::LogDebug);
    }
    else if (strcmp(argv[i], "--windowed") == 0)
    {
      settings.SetWindowed(true);
    }
    else if (strcmp(argv[i], "--vsync") == 0)
    {
      bool vsync = (strcmp(argv[i + 1], "on") == 0 || strcmp(argv[i + 1], "1") == 0);
      settings.SetVSync(vsync);
      i++; // skip vsync value
    }
    else if (strcmp(argv[i], "--max-vram") == 0)
    {
      char* err = nullptr;
      int maxVRAM = (int)strtol(argv[i + 1], &err, 10);
      settings.SetMaxVRAM(maxVRAM);
    }
    else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
    {
      #ifdef WIN32
      // This is a bit of a hack, but otherwise output will go to nowhere
      // when the application is compiled with the "WINDOWS" subsystem (which we usually are).
      // If you're an experienced Windows programmer and know how to do this
      // the right way, please submit a pull request!
      AttachConsole(ATTACH_PARENT_PROCESS);
      freopen("CONOUT$", "wb", stdout);
      #endif
      printf("EmulationStation, a graphical front-end for ROM browsing.\n"
             "Written by Alec \"Aloshi\" Lofquist.\n"
             "Version " PROGRAM_VERSION_STRING ", built " PROGRAM_BUILT_STRING "\n\n"
             "Command line arguments:\n"
             "--resolution [width] [height]	try and force a particular resolution\n"
             "--gamelist-only			skip automatic game search, only read from gamelist.xml\n"
             "--ignore-gamelist		ignore the gamelist (useful for troubleshooting)\n"
             "--draw-framerate		display the framerate\n"
             "--no-exit			don't show the exit option in the menu\n"
             "--hide-systemview		show only gamelist view, no system view\n"
             "--debug				more logging, show console on Windows\n"
             "--windowed			not fullscreen, should be used with --resolution\n"
             "--vsync [1/on or 0/off]		turn vsync on or off (default is on)\n"
             "--max-vram [size]		Max VRAM to use in Mb before swapping. 0 for unlimited\n"
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

  // Settings instance
  Settings settings;

  // Get arguments
  unsigned int width = 0;
  unsigned int height = 0;
  if (!parseArgs(argc, argv, width, height, settings))
    return 0;

  for(int loopCount = 0;; ++loopCount)
  {
    // Start the runner
    MainRunner runner(argv[0], width, height, loopCount, env);
    MainRunner::ExitState exitState = runner.Run();

    LOG(LogInfo) << "EmulationStation cleanly shutting down.";
    switch(exitState)
    {
      case MainRunner::ExitState::Quit:
      case MainRunner::ExitState::FatalError: return 0;
      case MainRunner::ExitState::Relaunch: continue;
      case MainRunner::ExitState::NormalReboot:
      case MainRunner::ExitState::FastReboot:
      {
        LOG(LogInfo) << "Rebooting system";
        #ifndef DEBUG
        if (system("shutdown -r now") != 0)
          LOG(LogError) << "Error rebooting system";
        #endif
        return 0;
      }
      case MainRunner::ExitState::Shutdown:
      case MainRunner::ExitState::FastShutdown:
      {
        LOG(LogInfo) << "Rebooting system (fast)";
        #ifndef DEBUG
        if (system("shutdown -h now") != 0)
          LOG(LogError) << "Error shutting system down";
        #endif
        return 0;
      }
    }
  }
}

