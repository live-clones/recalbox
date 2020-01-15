//
// Created by bkg2k on 13/11/2019.
//
#include <LocaleHelper.h>
#include <utils/os/fs/Path.h>
#include <utils/Log.h>
#include <utils/Files.h>
#include <AudioManager.h>
#include <views/ViewController.h>
#include <systems/SystemManager.h>
#include <guis/GuiMsgBoxScroll.h>
#include <recalbox/RecalboxUpgrade.h>
#include <RecalboxConf.h>
#include <VideoEngine.h>
#include <guis/GuiDetectDevice.h>
#include <utils/sdl2/SyncronousEventService.h>
#include <bios/BiosManager.h>
#include "MainRunner.h"
#include "EmulationStation.h"
#include "VolumeControl.h"
#include "NetworkThread.h"
#include "CommandThread.h"
#include "NetPlayThread.h"
#include "DemoMode.h"

MainRunner::ExitState MainRunner::sRequestedExitState = MainRunner::ExitState::Quit;
bool MainRunner::sQuitRequested = false;
bool MainRunner::sForceReloadFromDisk = false;

MainRunner::MainRunner(const std::string& executablePath, unsigned int width, unsigned int height)
  : mRequestedWidth(width),
    mRequestedHeight(height)
{
  OpenLogs();
  SetLocale(executablePath);
  CheckHomeFolder();
  SetArchitecture();
}

MainRunner::ExitState MainRunner::Run()
{
  try
  {
    // Shut-up joysticks :)
    SDL_JoystickEventState(SDL_DISABLE);

    // Initialize the renderer first,'cause many things depend on renderer width/height
    if (!Renderer::initialize((int)mRequestedWidth, (int)mRequestedHeight))
    {
      LOG(LogError) << "Error initializing the GL renderer.";
      return ExitState::FatalError;
    }

    // Initialize main Window and ViewController
    SystemManager systemManager;
    ApplicationWindow window(systemManager);
    if (!window.Initialize(mRequestedWidth, mRequestedHeight, false))
    {
      LOG(LogError) << "Window failed to initialize!";
      return ExitState::FatalError;
    }

    // Initialize audio manager
    VolumeControl::getInstance()->init();
    AudioManager audioManager(window);

    // Display "loading..." screen
    //window.renderLoadingScreen();
    window.RenderAll();
    PlayLoadingSound(audioManager);

    // Try to load system configurations
    if (!TryToLoadConfiguredSystems(systemManager, sForceReloadFromDisk))
      return ExitState::FatalError;
    ResetForceReloadState();

    // Run kodi at startup?
    RecalboxConf& recalboxConf = RecalboxConf::Instance();
    if (recalboxConf.AsString("kodi.enabled") == "1" && recalboxConf.AsString("kodi.atstartup") == "1")
      RecalboxSystem::launchKodi(window);

    ExitState exitState;
    try
    {
      // Start update thread
      LOG(LogDebug) << "Launching Network thread";
      NetworkThread networkThread(window);
      // Start the socket server
      LOG(LogDebug) << "Launching Command thread";
      CommandThread commandThread(systemManager);
      // Start Video engine
      LOG(LogDebug) << "Launching Video engine";
      VideoEngine::This().StartEngine();
      // Start Neyplay thread
      LOG(LogDebug) << "Launching Netplay thread";
      NetPlayThread netPlayThread(window);

      // Update?
      CheckUpdateMessage(window);
      // Input ok?
      CheckAndInitializeInput(window);

      // Bios
      BiosManager biosManager;
      biosManager.LoadFromFile();
      biosManager.Scan(nullptr);

      // Main Loop!
      CreateReadyFlagFile();
      exitState = MainLoop(window, systemManager);
      ResetExitState();
      DeleteReadyFlagFile();
    }
    catch(std::exception& ex)
    {
      LOG(LogError) << "Main thread crashed (inner).";
      LOG(LogError) << "Exception: " << ex.what();
      exitState = ExitState::Relaunch;
    }

    // Exit
    window.GoToQuitScreen();
    //window.renderShutdownScreen();
    VideoEngine::This().StopVideo(true);
    window.deleteAllGui();
    systemManager.DeleteAllSystems(DoWeHaveToUpdateGamelist(exitState));
    Window::Finalize();

    return exitState;
  }
  catch(std::exception& ex)
  {
    LOG(LogError) << "Main thread crashed (outer).";
    LOG(LogError) << "Exception: " << ex.what();
  }

  // If we get there, a severe and probably non-recoverable error occured.
  // Just quit
  return ExitState::FatalError;
}

void MainRunner::CreateReadyFlagFile()
{
  // Create a flag in  temporary directory to signal READY state
  Path ready(sReadyFile);
  Files::SaveFile(ready, "ready");
}

void MainRunner::DeleteReadyFlagFile()
{
  Path ready(sReadyFile);
  ready.Delete();
}

MainRunner::ExitState MainRunner::MainLoop(ApplicationWindow& window, SystemManager& systemManager)
{
  // Allow joystick event
  SDL_JoystickEventState(SDL_ENABLE);

  DemoMode demoMode(window, systemManager);

  LOG(LogDebug) << "Entering main loop";
  Path mustExit(sQuitNow);
  int lastTime = SDL_GetTicks();
  for(;;)
  {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0)
    {
      switch (event.type)
      {
        case SDL_QUIT: return ExitState::Quit;
        case SDL_TEXTINPUT:
        {
          window.textInput(event.text.text);
          break;
        }
        case SDL_JOYHATMOTION:
        case SDL_JOYBUTTONDOWN:
        case SDL_JOYBUTTONUP:
        case SDL_KEYDOWN:
        case SDL_KEYUP:
        case SDL_JOYAXISMOTION:
        case SDL_JOYDEVICEADDED:
        case SDL_JOYDEVICEREMOVED:
        {
          // Convert event
          InputCompactEvent compactEvent = InputManager::Instance().ManageSDLEvent(event);
          // Process
          if (!compactEvent.Empty()) window.ProcessInput(compactEvent);
          // Quit?
          if (window.Closed()) RequestQuit(ExitState::Quit);
          break;
        }
        default:
        {
          SyncronousEventService::Instance().Dispatch(&event);
          break;
        }
      }
    }

    if (window.isSleeping())
    {
      if (demoMode.hasDemoMode())
        demoMode.runDemo();

      lastTime = SDL_GetTicks();
      // Take a breath
      SDL_Delay(1);
      continue;
    }

    int curTime = SDL_GetTicks();
    int deltaTime = curTime - lastTime;
    lastTime = curTime;
    if (deltaTime > 1000 || deltaTime < 0) // cap deltaTime at 1000
      deltaTime = 1000;

    window.Update(deltaTime);
    window.RenderAll();

    // Immediate exit required? TODO: Filewatching!
    if (mustExit.Exists()) return ExitState::Quit;

    // Quit Request?
    if (sQuitRequested)
      return sRequestedExitState;
  }
}

void MainRunner::CheckAndInitializeInput(Window& window)
{
  // Choose which GUI to open depending on if an input configuration already exists
  LOG(LogDebug) << "Preparing GUI";
  if (InputManager::ConfigurationPath().Exists() && InputManager::Instance().ConfiguredDeviceCount() > 0)
    ViewController::Instance().goToStart();
  else
    window.pushGui(new GuiDetectDevice(window, true, [] { ViewController::Instance().goToStart(); }));
}

void MainRunner::CheckUpdateMessage(Window& window)
{
  // Push a message box with the whangelog if Recalbox has been updated
  std::string changelog = RecalboxUpgrade::getChangelog();
  if (!changelog.empty())
  {
    std::string message = "Changes :\n" + changelog;
    window.pushGui(new GuiMsgBoxScroll(window, _("THE SYSTEM IS UP TO DATE"), message, _("OK"), []
    {
      RecalboxUpgrade::updateLastChangelogFile();
    }, "", nullptr, "", nullptr, TextAlignment::Left));
  }
}

void MainRunner::PlayLoadingSound(AudioManager& audioManager)
{
  std::string selectedTheme = Settings::Instance().ThemeSet();
  Path loadingMusic = RootFolders::DataRootFolder / "system/.emulationstation/themes" / selectedTheme / "fx/loading.ogg";
  if (!loadingMusic.Exists())
    loadingMusic = RootFolders::DataRootFolder / "themes" / selectedTheme / "fx/loading.ogg";
  if (loadingMusic.Exists())
  {
    audioManager.PlayMusic(audioManager.LoadMusic(loadingMusic), false);
  }
}

bool MainRunner::TryToLoadConfiguredSystems(SystemManager& systemManager, bool forceReloadFromDisk)
{
  if (!systemManager.LoadSystemConfigurations(forceReloadFromDisk))
  {
    LOG(LogError) << "Error while parsing systems configuration file!";
    LOG(LogError) << "IT LOOKS LIKE YOUR SYSTEMS CONFIGURATION FILE HAS NOT BEEN SET UP OR IS INVALID. YOU'LL NEED TO DO THIS BY HAND, UNFORTUNATELY.\n\n"
                     "VISIT EMULATIONSTATION.ORG FOR MORE INFORMATION.";
    return false;
  }

  if (systemManager.GetVisibleSystemList().empty())
  {
    LOG(LogError) << "No systems found! Does at least one system have a game present? (check that extensions match!)\n(Also, make sure you've updated your es_systems.cfg for XML!)";
    LOG(LogError) << "WE CAN'T FIND ANY SYSTEMS!\n"
                     "CHECK THAT YOUR PATHS ARE CORRECT IN THE SYSTEMS CONFIGURATION FILE, AND "
                     "YOUR GAME DIRECTORY HAS AT LEAST ONE GAME WITH THE CORRECT EXTENSION.\n"
                     "\n"
                     "VISIT RECALBOX.FR FOR MORE INFORMATION.";
    return false;
  }

  return true;
}

void onExit()
{
  Log::close();
}

void MainRunner::OpenLogs()
{
  atexit(&onExit); // Always close the log on exit

  Log::open();
  LOG(LogInfo) << "EmulationStation - v" << PROGRAM_VERSION_STRING << ", built " << PROGRAM_BUILT_STRING;
}

void MainRunner::CheckHomeFolder()
{
  //make sure the config directory exists
  Path home = RootFolders::DataRootFolder;
  Path configDir = home / "system/.emulationstation";
  if (!configDir.Exists())
  {
    LOG(LogError) << "Creating config directory \"" << configDir.ToString() << "\"\n";
    configDir.CreatePath();
    if (!configDir.Exists())
    {
      LOG(LogError) << "Config directory could not be created!\n";
    }
  }
}

void MainRunner::SetLocale(const std::string& executablePath)
{
  Path path(executablePath);
  path = path.Directory(); // Get executable folder
  if (path.IsEmpty() || !path.Exists())
  {
    LOG(LogError) << "Error getting path";
  }

  setlocale(LC_ALL,"");
  path = path / "locale/lang";
  bindtextdomain("emulationstation2", path.ToChars());
  textdomain("emulationstation2");

  LOG(LogInfo) << "Locals set...";
}

void MainRunner::SetArchitecture()
{
  if (Settings::Instance().Arch().empty())
    Settings::Instance().SetArch(Files::LoadFile(Path("/recalbox/recalbox.arch")));
}

void MainRunner::ReceiveSyncCallback(const SDL_Event& /*event*/)
{
  //sQuitRequested = true;
  //sRequestedExitState = (ExitState)event.user.code;
}

void MainRunner::RequestQuit(MainRunner::ExitState requestedState, bool forceReloadFromDisk)
{
  sQuitRequested = true;
  sRequestedExitState = requestedState;
  sForceReloadFromDisk = forceReloadFromDisk;
}

bool MainRunner::DoWeHaveToUpdateGamelist(MainRunner::ExitState state)
{
  switch(state)
  {
    case ExitState::Quit:
    case ExitState::NormalReboot:
    case ExitState::Shutdown:
    case ExitState::Relaunch: return true;
    case ExitState::FatalError:
    case ExitState::FastReboot:
    case ExitState::FastShutdown: break;
  }
  return false;
}
