//EmulationStation, a graphical front-end for ROM browsing. Created by Alec "Aloshi" Lofquist.
//http://www.aloshi.com

#include <SDL.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <RootFolders.h>
#include <VideoEngine.h>

#include "AudioManager.h"
#include "CommandThread.h"
#include "EmulationStation.h"
#include "FileSorts.h"
#include "Locale.h"
#include "Log.h"
#include "NetPlayThread.h"
#include "NetworkThread.h"
#include "platform.h"
#include "RecalboxConf.h"
#include "Renderer.h"
#include "resources/Font.h"
#include "ScraperCmdLine.h"
#include "Settings.h"
#include "SystemData.h"
#include "views/ViewController.h"
#include "VolumeControl.h"
#include "Window.h"
#include "DemoMode.h"
#include "guis/GuiDetectDevice.h"
#include "guis/GuiInfoPopup.h"
#include "guis/GuiMsgBox.h"
#include "guis/GuiMsgBoxScroll.h"
#include "recalbox/RecalboxSystem.h"
#include "recalbox/RecalboxUpgrade.h"
#include "recalbox/RecalboxSystem.h"
#include "datetime/SystemDateTimeInterface.h"

#ifdef WIN32
  #include <Windows.h>
#endif

namespace fs = boost::filesystem;

bool scrape_cmdline = false;

void playSound(const std::string& name)
{
  std::string selectedTheme = Settings::getInstance()->getString("ThemeSet");
  std::string loadingMusic = RootFolders::DataRootFolder + "/system/.emulationstation/themes/" + selectedTheme +
                             "/fx/" + name + ".ogg";
  if (boost::filesystem::exists(loadingMusic))
  {
    Music::get(loadingMusic)->play(false, nullptr);
  }
}

bool parseArgs(int argc, char* argv[], unsigned int* width, unsigned int* height)
{
  for (int i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "--resolution") == 0)
    {
      if (i >= argc - 2)
      {
        std::cerr << "Invalid resolution supplied.";
        return false;
      }

      char* err;
      *width = (unsigned int)strtol(argv[i + 1], &err, 10);
      *height = (unsigned int)strtol(argv[i + 2], &err, 10);
      i += 2; // skip the argument value
    }
    else if (strcmp(argv[i], "--ignore-gamelist") == 0)
    {
      Settings::getInstance()->setBool("IgnoreGamelist", true);
    }
    else if (strcmp(argv[i], "--draw-framerate") == 0)
    {
      Settings::getInstance()->setBool("DrawFramerate", true);
    }
    else if (strcmp(argv[i], "--no-exit") == 0)
    {
      Settings::getInstance()->setBool("ShowExit", false);
    }
    else if (strcmp(argv[i], "--debug") == 0)
    {
      Settings::getInstance()->setBool("Debug", true);
      Settings::getInstance()->setBool("HideConsole", false);
      Log::setReportingLevel(LogLevel::LogDebug);
    }
    else if (strcmp(argv[i], "--windowed") == 0)
    {
      Settings::getInstance()->setBool("Windowed", true);
    }
    else if (strcmp(argv[i], "--vsync") == 0)
    {
      bool vsync = (strcmp(argv[i + 1], "on") == 0 || strcmp(argv[i + 1], "1") == 0);
      Settings::getInstance()->setBool("VSync", vsync);
      i++; // skip vsync value
    }
    else if (strcmp(argv[i], "--scrape") == 0)
    {
      scrape_cmdline = true;
    }
    else if (strcmp(argv[i], "--max-vram") == 0)
    {
      char* err;
      int maxVRAM = (int)strtol(argv[i + 1], &err, 10);
      Settings::getInstance()->setInt("MaxVRAM", maxVRAM);
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
      std::cout << "EmulationStation, a graphical front-end for ROM browsing.\n"
                   "Written by Alec \"Aloshi\" Lofquist.\n"
                   "Version " << PROGRAM_VERSION_STRING << ", built " << PROGRAM_BUILT_STRING << "\n\n"
                                                                                                 "Command line arguments:\n"
                                                                                                 "--resolution [width] [height]	try and force a particular resolution\n"
                                                                                                 "--gamelist-only			skip automatic game search, only read from gamelist.xml\n"
                                                                                                 "--ignore-gamelist		ignore the gamelist (useful for troubleshooting)\n"
                                                                                                 "--draw-framerate		display the framerate\n"
                                                                                                 "--no-exit			don't show the exit option in the menu\n"
                                                                                                 "--hide-systemview		show only gamelist view, no system view\n"
                                                                                                 "--debug				more logging, show console on Windows\n"
                                                                                                 "--scrape			scrape using command line interface\n"
                                                                                                 "--windowed			not fullscreen, should be used with --resolution\n"
                                                                                                 "--vsync [1/on or 0/off]		turn vsync on or off (default is on)\n"
                                                                                                 "--max-vram [size]		Max VRAM to use in Mb before swapping. 0 for unlimited\n"
                                                                                                 "--help, -h			summon a sentient, angry tuba\n\n"
                                                                                                 "More information available in README.md.\n";
      return false; //exit after printing help
    }
  }

  return true;
}

bool verifyHomeFolderExists()
{
  //make sure the config directory exists
  std::string home = RootFolders::DataRootFolder;
  std::string configDir = home + "/system/.emulationstation";
  if (!fs::exists(configDir))
  {
    std::cout << "Creating config directory \"" << configDir << "\"\n";
    fs::create_directory(configDir);
    if (!fs::exists(configDir))
    {
      std::cerr << "Config directory could not be created!\n";
      return false;
    }
  }

  return true;
}

// Returns true if everything is OK, 
bool loadSystemConfigFile(const char** errorString)
{
  *errorString = nullptr;

  if (!SystemData::loadConfig())
  {
    LOG(LogError) << "Error while parsing systems configuration file!";
    *errorString = "IT LOOKS LIKE YOUR SYSTEMS CONFIGURATION FILE HAS NOT BEEN SET UP OR IS INVALID. YOU'LL NEED TO DO THIS BY HAND, UNFORTUNATELY.\n\n"
                   "VISIT EMULATIONSTATION.ORG FOR MORE INFORMATION.";
    return false;
  }

  if (SystemData::sSystemVector.empty())
  {
    LOG(LogError)
      << "No systems found! Does at least one system have a game present? (check that extensions match!)\n(Also, make sure you've updated your es_systems.cfg for XML!)";
    *errorString = "WE CAN'T FIND ANY SYSTEMS!\n"
                   "CHECK THAT YOUR PATHS ARE CORRECT IN THE SYSTEMS CONFIGURATION FILE, AND "
                   "YOUR GAME DIRECTORY HAS AT LEAST ONE GAME WITH THE CORRECT EXTENSION.\n"
                   "\n"
                   "VISIT RECALBOX.FR FOR MORE INFORMATION.";
    return false;
  }

  return true;
}

//called on exit, assuming we get far enough to have the log initialized
void onExit()
{
  Log::close();
}

int setLocale(char* argv1)
{
  char path_save[PATH_MAX];
  char abs_exe_path[PATH_MAX];
  char* p;

  bool error = false;
  if (!(p = strrchr(argv1, '/')))
  {
    char* res = getcwd(abs_exe_path, sizeof(abs_exe_path));
    error = (res == nullptr);
  }
  else
  {
    *p = '\0';
    if (getcwd(path_save, sizeof(path_save)) == nullptr)
      error = true;
    if (chdir(argv1) != 0)
      error = true;
    if (getcwd(abs_exe_path, sizeof(abs_exe_path)) == nullptr)
      error = true;
    if (chdir(path_save) != 0)
      error = true;
  }
  if (error)
  {
    LOG(LogError) << "Error getting path";
  }

  boost::locale::localization_backend_manager my = boost::locale::localization_backend_manager::global();
  // Get global backend

  my.select("std");
  boost::locale::localization_backend_manager::global(my);
  // set this backend globally

  boost::locale::generator gen;

  std::string localeDir = abs_exe_path;
  localeDir += "/locale/lang";
  LOG(LogInfo) << "Setting local directory to " << localeDir;
  // Specify location of dictionaries
  gen.add_messages_path(localeDir);
  gen.add_messages_path("/usr/share/locale");
  gen.add_messages_domain("emulationstation2");

  // Generate locales and imbue them to iostream
  std::locale::global(gen(""));
  std::cout.imbue(std::locale());
  LOG(LogInfo) << "Locals set...";
  return 0;
}

int main(int argc, char* argv[])
{
  unsigned int width = 0;
  unsigned int height = 0;

  if (!parseArgs(argc, argv, &width, &height))
    return 0;

  // start the logger
  Log::open();
  LOG(LogInfo) << "EmulationStation - v" << PROGRAM_VERSION_STRING << ", built " << PROGRAM_BUILT_STRING;

  //always close the log on exit
  atexit(&onExit);

  // Initialize system datetime interface
  SystemDateTimeImplementation::Initialize();

  // only show the console on Windows if HideConsole is false
  #ifdef WIN32
  // MSVC has a "SubSystem" option, with two primary options: "WINDOWS" and "CONSOLE".
  // In "WINDOWS" mode, no console is automatically created for us.  This is good,
  // because we can choose to only create the console window if the user explicitly
  // asks for it, preventing it from flashing open and then closing.
  // In "CONSOLE" mode, a console is always automatically created for us before we
  // enter main. In this case, we can only hide the console after the fact, which
  // will leave a brief flash.
  // TL;DR: You should compile ES under the "WINDOWS" subsystem.
  // I have no idea how this works with non-MSVC compilers.
  if(!Settings::getInstance()->getBool("HideConsole"))
  {
    // we want to show the console
    // if we're compiled in "CONSOLE" mode, this is already done.
    // if we're compiled in "WINDOWS" mode, no console is created for us automatically;
    // the user asked for one, so make one and then hook stdin/stdout/sterr up to it
    if(AllocConsole()) // should only pass in "WINDOWS" mode
    {
      freopen("CONIN$", "r", stdin);
      freopen("CONOUT$", "wb", stdout);
      freopen("CONOUT$", "wb", stderr);
    }
  }else{
    // we want to hide the console
    // if we're compiled with the "WINDOWS" subsystem, this is already done.
    // if we're compiled with the "CONSOLE" subsystem, a console is already created;
    // it'll flash open, but we hide it nearly immediately
    if(GetConsoleWindow()) // should only pass in "CONSOLE" mode
      ShowWindow(GetConsoleWindow(), SW_HIDE);
  }
  #endif

  // Randomize
  std::srand(std::time(nullptr));

  try
  {
    //if ~/.emulationstation doesn't exist and cannot be created, bail
    if (!verifyHomeFolderExists())
      return 1;

    // Set locale
    setLocale(argv[0]);

    // other init
    FileSorts::init(); // require locale
    Settings::getInstance()->setBool("ThemeChanged", false);
    Settings::getInstance()->setBool("ThemeHasMenuView", false);

    std::string arch;
    std::ifstream archFile;
    archFile.open("/recalbox/recalbox.arch");
    std::getline(archFile, arch);
    archFile.close();
    Settings::getInstance()->setString("Arch", arch);

    Renderer::init(width, height);
    Window window;
    ViewController::init(&window);
    window.pushGui(ViewController::get());

    if (!scrape_cmdline)
    {
      if (!window.init(width, height, false))
      {
        LOG(LogError) << "Window failed to initialize!";
        return 1;
      }

      std::string glExts = (const char*) glGetString(GL_EXTENSIONS);
      LOG(LogInfo) << "Checking available OpenGL extensions...";
      LOG(LogInfo) << " ARB_texture_non_power_of_two: "
                   << (glExts.find("ARB_texture_non_power_of_two") != std::string::npos ? "OK" : "MISSING");

      window.renderLoadingScreen();
    }

    // Initialize audio manager
    VolumeControl::getInstance()->init();
    AudioManager::getInstance()->init();

    playSound("loading");

    const char* errorMsg = nullptr;
    if (!loadSystemConfigFile(&errorMsg))
    {
      // something went terribly wrong
      if (errorMsg == nullptr)
      {
        LOG(LogError) << "Unknown error occured while parsing system config file.";
        if (!scrape_cmdline)
          Renderer::deinit();
        return 1;
      }

      // we can't handle es_systems.cfg file problems inside ES itself, so display the error message then quit
      window.pushGui(new GuiMsgBox(&window, errorMsg, _("QUIT"), []
      {
        SDL_Event* quit = nullptr;
        quit = new SDL_Event();
        quit->type = SDL_QUIT;
        SDL_PushEvent(quit);
      }));
    }

    RecalboxConf* recalboxConf = RecalboxConf::getInstance();
    if (recalboxConf->get("kodi.enabled") == "1" && recalboxConf->get("kodi.atstartup") == "1")
    {
      RecalboxSystem::getInstance()->launchKodi(&window);
    }
    RecalboxSystem::getInstance()->getIpAdress();
    // UPDATED VERSION MESSAGE
    std::string changelog = RecalboxUpgrade::getInstance()->getChangelog();
    if (!changelog.empty())
    {
      std::string message = "Changes :\n" + changelog;
      window.pushGui(new GuiMsgBoxScroll(&window, _("THE SYSTEM IS UP TO DATE"), message, _("OK"), []
      {
        RecalboxUpgrade::getInstance()->updateLastChangelogFile();
      }, "", nullptr, "", nullptr, ALIGN_LEFT));
    }

    // UPDATE CHECK THREAD
    LOG(LogDebug) << "Launching Network thread";
    NetworkThread networkThread(&window);

    // Start the socket server
    LOG(LogDebug) << "Launching Command thread";
    CommandThread commandThread(&window);

    // Starts Video engine
    LOG(LogDebug) << "Launching Video engine";
    VideoEngine::This().StartEngine();

    // Allocate custom event types
    unsigned int NetPlayPopupEvent = SDL_RegisterEvents(2);
    unsigned int MusicStartEvent = NetPlayPopupEvent + 1;
    AudioManager::getInstance()->SetMusicStartEvent(&window, MusicStartEvent);

    LOG(LogDebug) << "Launching Netplay thread";
    NetPlayThread netPlayThread(&window, NetPlayPopupEvent);

    //run the command line scraper then quit
    if (scrape_cmdline)
    {
      return run_scraper_cmdline();
    }

    //dont generate joystick events while we're loading (hopefully fixes "automatically started emulator" bug)
    SDL_JoystickEventState(SDL_DISABLE);


    // preload what we can right away instead of waiting for the user to select it
    // this makes for no delays when accessing content, but a longer startup time
    //ViewController::get()->preload();

    //choose which GUI to open depending on if an input configuration already exists
    LOG(LogDebug) << "Preparing GUI";
    if (errorMsg == nullptr)
    {
      if (fs::exists(InputManager::getConfigPath()) && InputManager::getInstance()->getNumConfiguredDevices() > 0)
      {
        ViewController::get()->goToStart();
      }
      else
      {
        window.pushGui(new GuiDetectDevice(&window, true, []
        { ViewController::get()->goToStart(); }));
      }
    }

    // Create a flag in  temporary directory to signal READY state
    fs::path ready_path = fs::temp_directory_path();
    ready_path /= "emulationstation.ready";
    FILE* ready_file = fopen(ready_path.c_str(), "w");
    if (ready_file)
      fclose(ready_file);

    //generate joystick events since we're done loading
    SDL_JoystickEventState(SDL_ENABLE);

    int popupDuration = Settings::getInstance()->getInt("MusicPopupTime");
    int lastTime = SDL_GetTicks();
    bool running = true;
    bool doReboot = false;
    bool doShutdown = false;

    DemoMode demoMode(window);

    LOG(LogDebug) << "Entering main loop";
    while (running)
    {
      SDL_Event event;
      while (SDL_PollEvent(&event))
      {
        switch (event.type)
        {
          case SDL_JOYHATMOTION:
          case SDL_JOYBUTTONDOWN:
          case SDL_JOYBUTTONUP:
          case SDL_KEYDOWN:
          case SDL_KEYUP:
          case SDL_JOYAXISMOTION:
          case SDL_TEXTINPUT:
          case SDL_TEXTEDITING:
          case SDL_JOYDEVICEADDED:
          case SDL_JOYDEVICEREMOVED:
            InputManager::getInstance()->parseEvent(event, &window);
            break;
          case SDL_QUIT:
            running = false;
            break;
          case RecalboxSystem::SDL_FAST_QUIT | RecalboxSystem::SDL_RB_REBOOT:
            running = false;
            doReboot = true;
            Settings::getInstance()->setBool("IgnoreGamelist", true);
            break;
          case RecalboxSystem::SDL_FAST_QUIT | RecalboxSystem::SDL_RB_SHUTDOWN:
            running = false;
            doShutdown = true;
            Settings::getInstance()->setBool("IgnoreGamelist", true);
            break;
          case SDL_QUIT | RecalboxSystem::SDL_RB_REBOOT:
            running = false;
            doReboot = true;
            break;
          case SDL_QUIT | RecalboxSystem::SDL_RB_SHUTDOWN:
            running = false;
            doShutdown = true;
            break;
          default:
          {
            if (event.type == NetPlayPopupEvent)
            {
              std::shared_ptr<GuiInfoPopup> popup = std::make_shared<GuiInfoPopup>(&window,
                                                                                   netPlayThread.GetLastPopupText(),
                                                                                   popupDuration, 20);
              window.setInfoPopup(popup);
            }
            else if (event.type == MusicStartEvent)
            {
              std::shared_ptr<GuiInfoPopup> popup = std::make_shared<GuiInfoPopup>(&window,
                                                                                   AudioManager::getInstance()->GetLastPopupText(),
                                                                                   popupDuration, 10);
              window.setInfoPopup(popup);
            }
            break;
          }
        }
      }

      if (window.isSleeping())
      {
        if (demoMode.hasDemoMode())
        {
          demoMode.runDemo();
        }

        lastTime = SDL_GetTicks();
        // this doesn't need to be accurate, we're just giving up our CPU time until something wakes us up
        SDL_Delay(1);
        continue;
      }

      int curTime = SDL_GetTicks();
      int deltaTime = curTime - lastTime;
      lastTime = curTime;

      // cap deltaTime at 1000
      if (deltaTime > 1000 || deltaTime < 0)
        deltaTime = 1000;

      window.update(deltaTime);
      window.render();
      Renderer::swapBuffers();

      Log::flush();

      // Immediate exit required?
      if(fs::exists("/tmp/emulationstation.quitnow"))
        break;
    }

    // Clean ready flag
    if (fs::exists(ready_path))
      fs::remove(ready_path);

    while (window.peekGui() != ViewController::get())
      delete window.peekGui();

    window.renderShutdownScreen();
    SystemData::deleteSystems();
    window.deinit();
    LOG(LogInfo) << "EmulationStation cleanly shutting down.";
    if (doReboot)
    {
      LOG(LogInfo) << "Rebooting system";
      int res1 = system("touch /tmp/reboot.please");
      int res2 = system("shutdown -r now");
      if ((res1 | res2) != 0)
      {
        LOG(LogError) << "Error rebooting system";
      }
    }
    else if (doShutdown)
    {
      LOG(LogInfo) << "Shutting system down";
      int res1 = system("touch /tmp/shutdown.please");
      int res2 = system("shutdown -h now");
      if ((res1 | res2) != 0)
      {
        LOG(LogError) << "Error shutting system down";
      }
    }
  }
  catch(std::exception& ex)
  {
    LOG(LogError) << "Main thread crashed.";
    LOG(LogError) << "Exception: " << ex.what();
  }

  return 0;
}

