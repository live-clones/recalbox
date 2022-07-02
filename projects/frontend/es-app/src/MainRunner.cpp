//
// Created by bkg2k on 13/11/2019.
//
#include <utils/locale/LocaleHelper.h>
#include <utils/os/fs/Path.h>
#include <utils/Log.h>
#include <utils/Files.h>
#include <utils/locale/Internationalizer.h>
#include <utils/sdl2/SyncronousEventService.h>
#include <audio/AudioManager.h>
#include <views/ViewController.h>
#include <systems/SystemManager.h>
#include <guis/GuiMsgBoxScroll.h>
#include <VideoEngine.h>
#include <guis/GuiDetectDevice.h>
#include <bios/BiosManager.h>
#include <guis/GuiMsgBox.h>
#include <scraping/ScraperFactory.h>
#include <audio/AudioController.h>
#include <utils/os/system/ProcessTree.h>
#include <recalbox/RecalboxSystem.h>
#include <guis/wizards/WizardAgo2.h>
#include <guis/wizards/WizardAgo3.h>
#include "MainRunner.h"
#include "EmulationStation.h"
#include "Upgrade.h"
#include "CommandThread.h"
#include <netplay/NetPlayThread.h>
#include "DemoMode.h"
#include <utils/network/DnsClient.h>
#include <music/RemotePlaylist.h>
#include <hardware/devices/storage/StorageDevices.h>
#include <guis/GuiInfoPopup.h>
#include <scraping/ScraperSeamless.h>
#include <sdl2/Sdl2Runner.h>
#include <emulators/run/GameRunner.h>
#include <sdl2/Sdl2Init.h>
#include <patreon/PatronInfo.h>

MainRunner::ExitState MainRunner::sRequestedExitState = MainRunner::ExitState::Quit;
bool MainRunner::sQuitRequested = false;
bool MainRunner::sForceReloadFromDisk = false;

MainRunner::MainRunner(const std::string& executablePath, unsigned int width, unsigned int height, bool windowed, int runCount, char** environment, bool debug)
  : mRequestedWidth(width)
  , mRequestedHeight(height)
  , mRequestWindowed(windowed)
  , mPendingExit(PendingExit::None)
  , mRunCount(runCount)
  , mNotificationManager(environment)
  , mApplicationWindow(nullptr)
{
  Intro(debug);
  SetLocale(executablePath);
  CheckHomeFolder();

  // Initialize SDL
  Sdl2Init::Initialize();
}

MainRunner::~MainRunner()
{
  // Finalize SDL
  Sdl2Init::Finalize();
}

MainRunner::ExitState MainRunner::Run()
{
  try
  {
    // Hardware board
    Board board(*this);

    // Save power for battery-powered devices
    board.SetCPUGovernance(IBoardInterface::CPUGovernance::PowerSave);

    // Install CRT features if they are still unset
    InstallCRTFeatures();

    // Audio controller
    AudioController audioController;
    audioController.SetVolume(audioController.GetVolume());
    std::string originalAudioDevice = RecalboxConf::Instance().GetAudioOuput();
    std::string fixedAudioDevice = audioController.SetDefaultPlayback(originalAudioDevice);
    if (fixedAudioDevice != originalAudioDevice)
      RecalboxConf::Instance().SetAudioOuput(fixedAudioDevice).Save();

    // Notification Manager
    mNotificationManager.Notify(Notification::Start, Strings::ToString(mRunCount));

    // Shut-up joysticks :)
    SDL_JoystickEventState(SDL_DISABLE);

    // Initialize the renderer first,'cause many things depend on renderer width/height
    Renderer renderer((int)mRequestedWidth, (int)mRequestedHeight, mRequestWindowed);
    if (!renderer.Initialized()) { LOG(LogError) << "[Renderer] Error initializing the GL renderer."; return ExitState::FatalError; }

    // Initialize main Window and ViewController
    SystemManager systemManager(*this, mIgnoredFiles);
    ApplicationWindow window(systemManager);
    if (!window.Initialize(mRequestedWidth, mRequestedHeight, false)) { LOG(LogError) << "[Renderer] Window failed to initialize!"; return ExitState::FatalError; }
    InputManager::Instance().Initialize();
    mApplicationWindow = &window;
    // Brightness
    if (board.HasBrightnessSupport())
      board.SetBrightness(RecalboxConf::Instance().GetBrightness());

    // Initialize audio manager
    AudioManager audioManager(window);

    // Board-related background processes
    // Initialize here so that all global object are available
    board.StartGlobalBackgroundProcesses();

    // Display "loading..." screen
    //window.renderLoadingScreen();
    window.RenderAll();
    PlayLoadingSound(audioManager);

    // Try to load system configurations
    FileNotifier fileNotifier;
    if (!TryToLoadConfiguredSystems(systemManager, fileNotifier, sForceReloadFromDisk))
      return ExitState::FatalError;
    ResetForceReloadState();

    // Scrapers
    ScraperFactory scraperFactory;

    ExitState exitState = ExitState::Quit;
    try
    {
      // Patron Information
      PatronInfo patronInfo(this);
      // Remote music
      RemotePlaylist remotePlaylist;

      // Run kodi at startup?
      GameRunner gameRunner(window, systemManager, *this);
      //if (RecalboxSystem::kodiExists())
      if ((mRunCount == 0) && mConfiguration.GetKodiEnabled() && mConfiguration.GetKodiAtStartup())
        gameRunner.RunKodi();
      
      // Start update thread
      { LOG(LogDebug) << "[MainRunner] Launching Network thread"; }
      Upgrade networkThread(window);
      // Start the socket server
      { LOG(LogDebug) << "[MainRunner] Launching Command thread"; }
      CommandThread commandThread(systemManager);
      // Start Video engine
      { LOG(LogDebug) << "[MainRunner] Launching Video engine"; }
      VideoEngine videoEngine;
      // Start Netplay thread
      { LOG(LogDebug) << "[MainRunner] Launching Netplay thread"; }
      NetPlayThread netPlayThread(window);

      // Seamless scraper
      ScraperSeamless seamlessScraper;

      // Update?
      CheckUpdateMessage(window);
      CheckUpdateFailed(window);
      // Input ok?
      CheckAndInitializeInput(window);
      // Wizard
      CheckFirstTimeWizard(window);
      // Alert
      CheckAlert(window, systemManager);

      // Bios
      BiosManager biosManager;
      biosManager.LoadFromFile();
      biosManager.Scan(nullptr);

      // Main Loop!
      CreateReadyFlagFile();
      Path externalNotificationFolder = Path(sQuitNow).Directory();
      externalNotificationFolder.CreatePath();
      fileNotifier.WatchFile(externalNotificationFolder)
                  .SetEventNotifier(EventType::CloseWrite | EventType::Remove | EventType::Create, this);

      // Main SDL loop
      exitState = MainLoop(window, systemManager, fileNotifier);

      ResetExitState();
      fileNotifier.ResetEventNotifier();
      DeleteReadyFlagFile();
      window.deleteAllGui();
    }
    catch(std::exception& ex)
    {
      { LOG(LogError) << "[MainRunner] Main thread crashed (inner)."; }
      { LOG(LogError) << "[MainRunner] Exception: " << ex.what(); }
      exitState = ExitState::Relaunch;
    }

    // Exit
    mNotificationManager.Notify(Notification::Stop, Strings::ToString(mRunCount));
    window.GoToQuitScreen();
    systemManager.DeleteAllSystems(DoWeHaveToUpdateGamelist(exitState));
    WindowManager::Finalize();
    mApplicationWindow = nullptr;

    { LOG(LogInfo) << "[MainRunner] Quit requested (outer) [" << (int)sRequestedExitState << ']'; }
    switch(exitState)
    {
      case ExitState::Quit:
      case ExitState::FatalError: NotificationManager::Instance().Notify(Notification::Quit, exitState == ExitState::FatalError ? "fatalerror" : "quitrequested"); break;
      case ExitState::Relaunch:
      case ExitState::RelaunchNoUpdate: NotificationManager::Instance().Notify(Notification::Relaunch); break;
      case ExitState::NormalReboot:
      case ExitState::FastReboot:
      {
        NotificationManager::Instance().Notify(Notification::Reboot, exitState == ExitState::FastReboot ? "fast" : "normal");
        board.OnRebootOrShutdown();
        break;
      }
      case ExitState::Shutdown:
      case ExitState::FastShutdown: {
        NotificationManager::Instance().Notify(Notification::Shutdown, exitState == ExitState::FastShutdown ? "fast" : "normal");
        board.OnRebootOrShutdown();
        break;
      }
    }

    return exitState;
  }
  catch(std::exception& ex)
  {
    { LOG(LogError) << "[MainRunner] Main thread crashed (outer)."; }
    { LOG(LogError) << "[MainRunner] Exception: " << ex.what(); }
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

MainRunner::ExitState MainRunner::MainLoop(ApplicationWindow& window, SystemManager& systemManager, FileNotifier& fileNotifier)
{
  // Allow joystick event
  SDL_JoystickEventState(SDL_ENABLE);

  DemoMode demoMode(window, systemManager);

  { LOG(LogDebug) << "[MainRunner] Entering main loop"; }
  Path mustExit(sQuitNow);
  int lastTime = (int)SDL_GetTicks();
  for(;;)
  {
    // File watching
    fileNotifier.CheckAndDispatch();
    InputManager::Instance().WatchJoystickAddRemove(&window);

    // SDL
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
        //case SDL_JOYDEVICEADDED:
        //case SDL_JOYDEVICEREMOVED:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        {
          // Convert event
          //{ LOG(LogInfo) << "[MainRunner] Event in Loop event."; }
          InputCompactEvent compactEvent = InputManager::Instance().ManageSDLEvent(&window, event);
          // Process
          if (!compactEvent.Empty())
            if (!ProcessSpecialInputs(compactEvent))
              if (!Board::Instance().ProcessSpecialInputs(compactEvent))
                window.ProcessInput(compactEvent);
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

    if (window.isSleeping() && !GameClipView::IsGameClipEnabled())
    {
      if (DemoMode::hasDemoMode())
        demoMode.runDemo();

      lastTime = (int)SDL_GetTicks();
      // Take a breath
      SDL_Delay(1);
      continue;
    }

    int curTime = (int)SDL_GetTicks();
    int deltaTime = curTime - lastTime;
    lastTime = curTime;
    if (deltaTime > 1000 || deltaTime < 0) // cap deltaTime at 1000
      deltaTime = 1000;

    window.Update(deltaTime);
    window.RenderAll();

    // Quit Request?
    if (sQuitRequested)
    {
      { LOG(LogInfo) << "[MainRunner] Quit requested (inner) [" << (int)sRequestedExitState << ']'; }
      return sRequestedExitState;
    }

    // Quit pending?
    switch(mPendingExit)
    {
      case PendingExit::None: break;
      case PendingExit::GamelistChanged:
      case PendingExit::ThemeChanged:
      {
        std::string text = (mPendingExit == PendingExit::GamelistChanged) ?
          _("EmulationStation has detected external changes on a gamelist file.\nTo avoid loss of data, EmulationStation is about to relaunch and reload all files.") :
          _("EmulationStation has detected external changes on a theme file.\nTo avoid loss of data, EmulationStation is about to relaunch and reload all files.");
        GuiMsgBox* msgBox = new GuiMsgBox(window, text, _("OK"), [] { RequestQuit(ExitState::Relaunch, true); });
        window.pushGui(msgBox);
        break;
      }
      case PendingExit::MustExit: RequestQuit(ExitState::Quit);
      case PendingExit::WaitingExit: break;
    }
    if (mPendingExit != PendingExit::None)
      mPendingExit = PendingExit::WaitingExit; // Wait for exit
  }
}

void MainRunner::CheckAndInitializeInput(WindowManager& window)
{
  // Choose which GUI to open depending on if an input configuration already exists
  { LOG(LogDebug) << "[MainRunner] Preparing GUI"; }
  if (InputManager::ConfigurationPath().Exists() && InputManager::Instance().ConfiguredDeviceCount() > 0)
    ViewController::Instance().goToStart();
  else
    window.pushGui(new GuiDetectDevice(window, true, [] { ViewController::Instance().goToStart(); }));
}

void MainRunner::CheckAlert(WindowManager& window, SystemManager& systemManager)
{
  int memory = Board::Instance().TotalMemory();
  int maxSystem = 20 * (memory / 256);
  int maxGames = 5000 * (memory / 256);
  if (memory != 0 && memory <= 512)
  {
    int realSystemCount = 0;
    for(const SystemData* system : systemManager.GetVisibleSystemList())
      if (system->HasVisibleGame())
        realSystemCount++;
    if (realSystemCount > maxSystem)
    {
      std::string text = _("Your system has not enough memory to handle %SYSTEMS% systems. You should not exceed %MAXSYSTEMS% consoles/computers or you may face stability issues!\n\nYou can hide preinstalled games in UI SETTINGS menu to decrease active systems");
      Strings::ReplaceAllIn(text, "%SYSTEMS%", Strings::ToString(realSystemCount));
      Strings::ReplaceAllIn(text, "%MAXSYSTEMS%", Strings::ToString(maxSystem));
      window.pushGui(new GuiMsgBoxScroll(window, _("WARNING! SYSTEM OVERLOAD!"), text, _("OK"), nullptr, "", nullptr, "", nullptr, TextAlignment::Left));
    }
    else if (systemManager.GameCount() > maxGames)
    {
      std::string text = _("Your system has not enough memory to handle %GAMES% games. You should not exceed %MAXGAMES% or you may face stability issues!");
      Strings::ReplaceAllIn(text, "%GAMES%", Strings::ToString((int)systemManager.GameCount()));
      Strings::ReplaceAllIn(text, "%MAXGAMES%", Strings::ToString(maxGames));
      window.pushGui(new GuiMsgBoxScroll(window, _("WARNING! SYSTEM OVERLOAD!"), text, _("OK"), nullptr, "", nullptr, "", nullptr, TextAlignment::Left));
    }
  }
}

void MainRunner::CheckFirstTimeWizard(WindowManager& window)
{
  if (RecalboxConf::Instance().GetFirstTimeUse())
  {
    switch (Board::Instance().GetBoardType())
    {
      case BoardType::OdroidAdvanceGo:
      {
        window.pushGui(new WizardAGO2(window));
        return; // Let the OGA Wizard reset the flag
      }
      case BoardType::OdroidAdvanceGoSuper:
      {
        window.pushGui(new WizardAgo3(window));
        return; // Let the OGA Wizard reset the flag
      }
      case BoardType::PCx86:
      case BoardType::PCx64:
      case BoardType::UndetectedYet:
      case BoardType::Unknown:
      case BoardType::Pi0:
      case BoardType::Pi02:
      case BoardType::Pi1:
      case BoardType::Pi2:
      case BoardType::Pi3:
      case BoardType::Pi4:
      case BoardType::Pi400:
      case BoardType::Pi3plus:
      case BoardType::UnknownPi:
      default: break;
    }
    RecalboxConf::Instance().SetFirstTimeUse(false);
  }
}

void MainRunner::CheckUpdateMessage(WindowManager& window)
{
  // Push a message box with the changelog if Recalbox has been updated
  Path flag(sUpgradeFileFlag);
  if (flag.Exists())
  {
    std::string changelog = Files::LoadFile(Path(Upgrade::sLocalReleaseNoteFile));
    std::string message = "Changes :\n" + changelog;
    window.pushGui(new GuiMsgBoxScroll(window, _("THE SYSTEM IS UP TO DATE"), message, _("OK"), []{}, "", nullptr, "", nullptr, TextAlignment::Left));
  }
}

void MainRunner::CheckUpdateFailed(WindowManager& window)
{
  // Push a message if Recalbox upgrade has failed
  Path flag(sUpgradeFailedFlag);
  if (flag.Exists())
  {
    std::string version = Upgrade::CurrentVersion();
    std::string message = Strings::Format(_("The upgrade process has failed. You are back on Recalbox %s.\nPlease retry to upgrade your Recalbox, and contact the team on https://forum.recalbox.com if the problem persists.").c_str(), version.c_str());
    window.pushGui(new GuiMsgBoxScroll(window, _("THE UPGRADE HAS FAILED"), message, _("OK"), []{}, "", nullptr, "", nullptr, TextAlignment::Left));
  }
}

void MainRunner::PlayLoadingSound(AudioManager& audioManager)
{
  std::string selectedTheme = RecalboxConf::Instance().GetThemeFolder();
  Path loadingMusic = RootFolders::DataRootFolder / "system/.emulationstation/themes" / selectedTheme / "fx/loading.ogg";
  if (!loadingMusic.Exists())
    loadingMusic = RootFolders::DataRootFolder / "themes" / selectedTheme / "fx/loading.ogg";
  if (loadingMusic.Exists())
  {
    audioManager.PlayMusic(audioManager.LoadMusic(loadingMusic), false);
  }
}

bool MainRunner::TryToLoadConfiguredSystems(SystemManager& systemManager, FileNotifier& gamelistWatcher, bool forceReloadFromDisk)
{
  IniFile recalboxBootConf(Path("/boot/recalbox-boot.conf"), false);
  bool portable = recalboxBootConf.AsString("case") == "GPiV1:1";
  switch(Board::Instance().GetBoardType())
  {
    case BoardType::OdroidAdvanceGo:
    case BoardType::OdroidAdvanceGoSuper: portable = true; break;
    case BoardType::UndetectedYet:
    case BoardType::Unknown:
    case BoardType::Pi0:
    case BoardType::Pi02:
    case BoardType::Pi1:
    case BoardType::Pi2:
    case BoardType::Pi3:
    case BoardType::Pi3plus:
    case BoardType::Pi4:
    case BoardType::Pi400:
    case BoardType::UnknownPi:
    case BoardType::PCx86:
    case BoardType::PCx64:
    default: break;
  }

  if (!systemManager.LoadSystemConfigurations(gamelistWatcher, forceReloadFromDisk, portable))
  {
    { LOG(LogError) << "[MainRunner] Error while parsing systems configuration file!"; }
    { LOG(LogError) << "[MainRunner] IT LOOKS LIKE YOUR SYSTEMS CONFIGURATION FILE HAS NOT BEEN SET UP OR IS INVALID. YOU'LL NEED TO DO THIS BY HAND, UNFORTUNATELY.\n\n"
                       "VISIT EMULATIONSTATION.ORG FOR MORE INFORMATION."; }
    return false;
  }

  if (systemManager.GetVisibleSystemList().empty())
  {
    { LOG(LogError) << "[MainRunner] No systems found! Does at least one system have a game present? (check that extensions match!)\n(Also, make sure you've updated your es_systems.cfg for XML!)"; }
    { LOG(LogError) << "[MainRunner]  WE CAN'T FIND ANY SYSTEMS!\n"
                       "CHECK THAT YOUR PATHS ARE CORRECT IN THE SYSTEMS CONFIGURATION FILE, AND "
                       "YOUR GAME DIRECTORY HAS AT LEAST ONE GAME WITH THE CORRECT EXTENSION.\n"
                       "\n"
                       "VISIT RECALBOX.FR FOR MORE INFORMATION."; }
    return false;
  }

  return true;
}

void onExit()
{
  Log::close();
}

void Sdl2Log(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
  (void)userdata;
  const char* cat = "Unknown";
  switch(category)
  {
    case SDL_LOG_CATEGORY_APPLICATION: cat = "Application"; break;
    case SDL_LOG_CATEGORY_ERROR: cat = "Error"; break;
    case SDL_LOG_CATEGORY_ASSERT: cat = "Assert"; break;
    case SDL_LOG_CATEGORY_SYSTEM: cat = "System"; break;
    case SDL_LOG_CATEGORY_AUDIO: cat = "Audio"; break;
    case SDL_LOG_CATEGORY_VIDEO: cat = "Video"; break;
    case SDL_LOG_CATEGORY_RENDER: cat = "Render"; break;
    case SDL_LOG_CATEGORY_INPUT: cat = "Input"; break;
    case SDL_LOG_CATEGORY_TEST: cat = "Test"; break;
    default: break;
  }
  const char*  subType = "Unknown";
  switch(priority)
  {
    case SDL_LOG_PRIORITY_VERBOSE: subType = "Verbose"; break;
    case SDL_LOG_PRIORITY_DEBUG: subType = "Debug"; break;
    case SDL_LOG_PRIORITY_INFO: subType = "Info"; break;
    case SDL_LOG_PRIORITY_WARN: subType = "Warning"; break;
    case SDL_LOG_PRIORITY_ERROR: subType = "Error"; break;
    case SDL_LOG_PRIORITY_CRITICAL: subType = "Critical"; break;
    case SDL_NUM_LOG_PRIORITIES:
    default: break;
  }
  LOG(LogDebug) << "[SDL2] (" << cat << ':' << subType << ") " << message;
}

void MainRunner::SetDebugLogs(bool state)
{
  if (state)
  {
    Log::setReportingLevel(LogLevel::LogDebug);
    SDL_LogSetOutputFunction(Sdl2Log, nullptr);
    SDL_LogSetAllPriority(SDL_LogPriority::SDL_LOG_PRIORITY_VERBOSE);
  }
  else
  {
    Log::setReportingLevel(LogLevel::LogInfo);
    SDL_LogSetOutputFunction(nullptr, nullptr);
    SDL_LogSetAllPriority(SDL_LogPriority::SDL_LOG_PRIORITY_ERROR);
  }
}

void MainRunner::Intro(bool debug)
{
  if (atexit(&onExit) != 0) // Always close the log on exit
    { LOG(LogError) << "[MainRunner] Error setting exit function!"; }

  SetDebugLogs(debug || mConfiguration.GetDebugLogs());

  { LOG(LogInfo) << "[MainRunner] EmulationStation - v" << PROGRAM_VERSION_STRING << ", built " << PROGRAM_BUILT_STRING; }
}

void MainRunner::CheckHomeFolder()
{
  //make sure the config directory exists
  Path home = RootFolders::DataRootFolder;
  Path configDir = home / "system/.emulationstation";
  if (!configDir.Exists())
  {
    { LOG(LogError) << "[MainRunner] Creating config directory \"" << configDir.ToString() << "\"\n"; }
    configDir.CreatePath();
    if (!configDir.Exists()) { LOG(LogError) << "[MainRunner] Config directory could not be created!\n"; }
  }
}

void MainRunner::SetLocale(const std::string& executablePath)
{
  Path path(executablePath);
  path = path.Directory(); // Get executable folder
  if (path.IsEmpty() || !path.Exists()) { LOG(LogError) << "[Locale] Error getting executable path (received: " << executablePath << ')'; }

  // Get locale from configuration
  std::string localeName = RecalboxConf::Instance().GetSystemLanguage();

  // Set locale
  if (!Internationalizer::InitializeLocale(localeName, { path / "locale/lang", Path("/usr/share/locale") }, "emulationstation2"))
  { LOG(LogWarning) << "[Locale] No locale found. Default text used."; }
}

void MainRunner::ReceiveSyncCallback(const SDL_Event& /*event*/)
{
  //sQuitRequested = true;
  //sRequestedExitState = (ExitState)event.user.code;
}

void MainRunner::RequestQuit(MainRunner::ExitState requestedState, bool forceReloadFromDisk)
{
  ViewController::Instance().quitGameClipView();
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
    case ExitState::RelaunchNoUpdate:
    case ExitState::FatalError:
    case ExitState::FastReboot:
    case ExitState::FastShutdown: break;
  }
  return false;
}

void MainRunner::FileSystemWatcherNotification(EventType event, const Path& path, const DateTime& time)
{
  (void)time;

  if (path == sQuitNow)
    event = event | EventType::None;

  if (mPendingExit == PendingExit::None)
  {
    if (((event & EventType::Create) != 0) && (path == sQuitNow))
      mPendingExit = PendingExit::MustExit;
    else if ((event & (EventType::Remove | EventType::CloseWrite)) != 0)
    {
      if(IsFileIgnored(path.ToString()))
        return;

      std::string name = path.Filename();
      if (name == "gamelist.xml" || name == "gamelist.zip")
        mPendingExit = PendingExit::GamelistChanged;
      else if (path.ToString().find("themes") != std::string::npos)
        mPendingExit = PendingExit::ThemeChanged;
    }
  }
}

void MainRunner::HeadphonePluggedIn(BoardType board)
{
  { LOG(LogInfo) << "[Audio] Headphones plugged!"; }
  switch(board)
  {
    case BoardType::OdroidAdvanceGo:
    case BoardType::OdroidAdvanceGoSuper:
    {
      std::string currentAudio = RecalboxConf::Instance().GetAudioOuput();
      { LOG(LogInfo) << "[OdroidAdvanceGo2] Headphones plugged! " << currentAudio; }
      if (currentAudio == IAudioController::sAutoSwitch)
      {
        // Switch to headphone
        std::string Headphones = "alsa_output.platform-rk817-sound.multichannel-output:analog-output-headphones";
        if (IsApplicationRunning()) AudioManager::Instance().Deactivate();
        AudioController::Instance().SetDefaultPlayback(Headphones);
        if (IsApplicationRunning()) AudioManager::Instance().Reactivate();
        //RecalboxConf::Instance().SetAudioOuput(Headphones);
        // Create music popup
        int popupDuration = RecalboxConf::Instance().GetPopupMusic();
        { LOG(LogInfo) << "[OdroidAdvanceGo] Switch to Headphone!" << popupDuration << " " << (mApplicationWindow != nullptr ? "ok" : "nok"); }
        if (popupDuration != 0 && mApplicationWindow != nullptr)
          mApplicationWindow->InfoPopupAdd(new GuiInfoPopup(*mApplicationWindow, _("Switch audio output to Headphones!"),
                                                            popupDuration, GuiInfoPopupBase::PopupType::Music));
      }
      break;
    }
    case BoardType::UndetectedYet:
    case BoardType::Unknown:
    case BoardType::Pi0:
    case BoardType::Pi02:
    case BoardType::Pi1:
    case BoardType::Pi2:
    case BoardType::Pi3:
    case BoardType::Pi3plus:
    case BoardType::Pi4:
    case BoardType::Pi400:
    case BoardType::UnknownPi:
    case BoardType::PCx86:
    case BoardType::PCx64:
    default: break;
  }
}

void MainRunner::HeadphoneUnplugged(BoardType board)
{
  { LOG(LogInfo) << "[Audio] Headphones unplugged!"; }
  switch(board)
  {
    case BoardType::OdroidAdvanceGo:
    case BoardType::OdroidAdvanceGoSuper:
    {
      std::string currentAudio = RecalboxConf::Instance().GetAudioOuput();
      { LOG(LogInfo) << "[OdroidAdvanceGo2] Headphones unplugged! " << currentAudio; }
      if (currentAudio == IAudioController::sAutoSwitch)
      {
        // Switch back to speakers
        std::string Speakers = "alsa_output.platform-rk817-sound.multichannel-output:multichannel-output";
        if (IsApplicationRunning()) AudioManager::Instance().Deactivate();
        AudioController::Instance().SetDefaultPlayback(Speakers);
        if (IsApplicationRunning()) AudioManager::Instance().Reactivate();
        //RecalboxConf::Instance().SetAudioOuput(Speakers);
        // Create music popup
        int popupDuration = RecalboxConf::Instance().GetPopupMusic();
        { LOG(LogInfo) << "[OdroidAdvanceGo] Switch to Speaker!" << popupDuration << " " << (mApplicationWindow != nullptr ? "ok" : "nok"); }
        if (popupDuration != 0 && mApplicationWindow != nullptr)
          mApplicationWindow->InfoPopupAdd(new GuiInfoPopup(*mApplicationWindow, _("Switch audio output back to Speakers!"),
                                                            popupDuration, GuiInfoPopupBase::PopupType::Music));
      }
      break;
    }
    case BoardType::UndetectedYet:
    case BoardType::Unknown:
    case BoardType::Pi0:
    case BoardType::Pi02:
    case BoardType::Pi1:
    case BoardType::Pi2:
    case BoardType::Pi3:
    case BoardType::Pi3plus:
    case BoardType::Pi4:
    case BoardType::Pi400:
    case BoardType::UnknownPi:
    case BoardType::PCx86:
    case BoardType::PCx64:
    default: break;
  }
}

void MainRunner::ResetButtonPressed(BoardType board)
{
  (void)board;
  if (IsApplicationRunning())
  {
    // The application is running and is on screen.
    // Display little window to notify the user we are going to reset
    { LOG(LogDebug) << "[MainRunner] Reset Button Pressed : reseting"; }
    mApplicationWindow->pushGui((new GuiWaitLongExecution<HardwareTriggeredSpecialOperations, bool>(*mApplicationWindow, *this))
                                      ->Execute(HardwareTriggeredSpecialOperations::Reset, _("Restarting.")));
  } else {
    // Something is running (game, demo, kodi)
    Files::SaveFile(Path(sStopDemo), std::string());
    { LOG(LogDebug) << "[MainRunner] Reset Button Pressed in game : exiting subprocesses"; }
    ProcessTree::TerminateAll(1000);
  }
}

void MainRunner::PowerButtonPressed(BoardType board, int milliseconds)
{
  (void)board;
  if (IsApplicationRunning())
  {
    // The application is running and is on screen.
    // Display little window to notify the user
    if (milliseconds < sPowerButtonThreshold)
    {
      // Only if supported. Otherwise does nothing
      if (Board::Instance().HasSuspendResume())
        mApplicationWindow->pushGui((new GuiWaitLongExecution<HardwareTriggeredSpecialOperations, bool>(*mApplicationWindow, *this))
                                      ->Execute(HardwareTriggeredSpecialOperations::Suspend, _("Entering standby...")));
    }
    else
    {
      { LOG(LogDebug) << "[MainRunner] Power Button Pressed: shutting down"; }
      mApplicationWindow->pushGui((new GuiWaitLongExecution<HardwareTriggeredSpecialOperations, bool>(*mApplicationWindow, *this))
                                      ->Execute(HardwareTriggeredSpecialOperations::PowerOff, _("Bye bye!")));
    }
  }
  else
  {
    // The application is not Running, execute orders immediately
    if (milliseconds < sPowerButtonThreshold)
    {
      { LOG(LogDebug) << "[MainRunner] Power Button Pressed while running game: suspending"; }
      // Only if supported. Otherwise does nothing
      if (Board::Instance().HasSuspendResume())
        Board::Instance().Suspend();
    }
    else
    {
      { LOG(LogDebug) << "[MainRunner] Power Button Pressed while running game: shutting down"; }
      Files::SaveFile(Path(sQuitNow), std::string());
      Files::SaveFile(Path(sStopDemo), std::string());
      // Gracefuly qui emulators and all the call chain
      ProcessTree::TerminateAll(1000);
      // Quit
      RequestQuit(ExitState::Shutdown);
    }
  }
}

void MainRunner::Resume(BoardType board)
{
  (void)board;
  // so... Waking up :)
  if (mApplicationWindow != nullptr && IsApplicationRunning())
    mApplicationWindow->pushGui((new GuiWaitLongExecution<HardwareTriggeredSpecialOperations, bool>(*mApplicationWindow, *this))
                                ->Execute(HardwareTriggeredSpecialOperations::Resume, _("Waking up!")));
}

bool MainRunner::Execute(GuiWaitLongExecution<HardwareTriggeredSpecialOperations, bool>& from,
                         const HardwareTriggeredSpecialOperations& parameter)
{
  (void)from;
  switch(parameter)
  {
    case HardwareTriggeredSpecialOperations::PowerOff:
    case HardwareTriggeredSpecialOperations::Reset:
    {
      mApplicationWindow->DoWake();
      Thread::Sleep(1000); // Just sleep one second
      break;
    }
    case HardwareTriggeredSpecialOperations::Suspend:
    case HardwareTriggeredSpecialOperations::Resume:
    default: Thread::Sleep(1000); // Just sleep one second
  }
  return false; // unused
}

void MainRunner::Completed(const HardwareTriggeredSpecialOperations& parameter, const bool& result)
{
  (void)result;
  switch(parameter)
  {
    case HardwareTriggeredSpecialOperations::Suspend:
    {
      // Here is a little trick to erase the window from the screen before suspending
      // To show the user we're soon suspending the hardware, just display the last screen in half luminosity
      mApplicationWindow->Update(20);
      mApplicationWindow->RenderAll(true);

      // This method won't return until wake up
      Board::Instance().Suspend();
      break;
    }
    case HardwareTriggeredSpecialOperations::PowerOff:
    {
      // Bye bye :)
      RequestQuit(ExitState::Shutdown);
      break;
    }
    case HardwareTriggeredSpecialOperations::Reset:
    {
      RequestQuit(ExitState::NormalReboot);
      break;
    }
    case HardwareTriggeredSpecialOperations::Resume:
    {
      // Set audio output since headphone may have been plugged/unplugged
      std::string output = RecalboxConf::Instance().GetAudioOuput();
      AudioManager::Instance().Deactivate();
      AudioController::Instance().Refresh();
      AudioController::Instance().SetDefaultPlayback(output);
      AudioManager::Instance().Reactivate();
    }
    default: break;
  }
}

void MainRunner::VolumeDecrease(BoardType board, float percent)
{
  (void)board;

  int value = RecalboxConf::Instance().GetAudioVolume() - (int)(100 * percent);
  value = Math::clampi(value, 0, 100);
  value = (value / 10) * 10;
  AudioController::Instance().SetVolume(value);
  RecalboxConf::Instance().SetAudioVolume(value);
  RecalboxConf::Instance().Save();
}

void MainRunner::VolumeIncrease(BoardType board, float percent)
{
  (void)board;

  int value = RecalboxConf::Instance().GetAudioVolume() + (int)(100 * percent);
  value = Math::clampi(value, 0, 100);
  value = (value / 10) * 10;
  AudioController::Instance().SetVolume(value);
  RecalboxConf::Instance().SetAudioVolume(value);
  RecalboxConf::Instance().Save();
}

void MainRunner::RomPathAdded(const DeviceMount& device)
{
  std::string text = _("The device %NAME% containing roms has been plugged in! EmulationStation must relaunch to load new games.");
  Strings::ReplaceAllIn(text, "%NAME%", device.Name());
  if (device.ReadOnly())
    text.append(_("\nWARNING: You device may not have been properly unplugged and has consistency errors. As a result, it's been mounted as read-only. You should plug your device in a Window PC and use the repair tool."));
  GuiMsgBox* msgBox = new GuiMsgBox(*mApplicationWindow, text, _("OK"), [] { RequestQuit(ExitState::Relaunch, true); }, _("LATER"), []{});
  mApplicationWindow->pushGui(msgBox);
}

void MainRunner::RomPathRemoved(const DeviceMount& device)
{
  (void)device;
  std::string text = _("A device containing roms has been unplugged! EmulationStation must relaunch to remove unavailable games.");
  GuiMsgBox* msgBox = new GuiMsgBox(*mApplicationWindow, text, _("OK"), [] { RequestQuit(ExitState::Relaunch, true); }, _("LATER"), []{});
  mApplicationWindow->pushGui(msgBox);
}

void MainRunner::NoRomPathFound(const DeviceMount& device)
{
  auto initializeRoms = [this, &device]
  {
    USBInitialization init(USBInitializationAction::OnlyRomFolders, &device);
    mApplicationWindow->pushGui((new GuiWaitLongExecution<USBInitialization, bool>(*mApplicationWindow, *this))->Execute(init, _("Initializing roms folders...")));
  };

  auto moveShareFolder = [this, &device]
  {
    USBInitialization init(USBInitializationAction::CompleteShare, &device);
    mApplicationWindow->pushGui((new GuiWaitLongExecution<USBInitialization, bool>(*mApplicationWindow, *this))->Execute(init, _("Initializing share folders...")));
  };

  std::string text = _("The USB device %NAME% with no rom folder and no share folder has been plugged in! Would you like to initialize this device?");
              text.append(1, '\n');
              text.append(_("• Choose '%INIT%' to create only all the rom folders")).append(1, '\n')
                  .append(_("• Choose '%MOVE%' to copy all the current share to the new device, automatically switch to this device, and reboot")).append(1, '\n')
                  .append(_("• Or just chose '%CANCEL%' to do nothing with this new device"));
  Strings::ReplaceAllIn(text, "%NAME%", device.Name());
  Strings::ReplaceAllIn(text, "%INIT%", _("INITIALIZE"));
  Strings::ReplaceAllIn(text, "%MOVE%", _("MOVE SHARE"));
  Strings::ReplaceAllIn(text, "%CANCEL%", _("CANCEL"));
  GuiMsgBox* msgBox = new GuiMsgBox(*mApplicationWindow, text, _("INITIALIZE"), initializeRoms,
                                                               _("MOVE SHARE"), moveShareFolder,
                                                               _("CANCEL"), nullptr, TextAlignment::Left);
  mApplicationWindow->pushGui(msgBox);
}

void MainRunner::InstallCRTFeatures()
{
  if (Board::Instance().CrtBoard().GetCrtAdapter() != CrtAdapterType::None)
  {
    RecalboxConf& conf = RecalboxConf::Instance();
    if (!conf.HasScreenSaverType()) conf.SetScreenSaverType("demo");
    std::string recalboxTheme = "recalbox-next";
    if (conf.GetThemeFolder() == recalboxTheme)
    {
      if (!Strings::Contains(conf.GetThemeSystemView(recalboxTheme), "240p"))
        conf.SetThemeSystemView(recalboxTheme, "9-240p");

      if (!Strings::Contains(conf.GetThemeMenuSet(recalboxTheme), "240p"))
        conf.SetThemeMenuSet(recalboxTheme, "7-240p");

      if (!Strings::Contains(conf.GetThemeGameClipView(recalboxTheme), "240p"))
        conf.SetThemeGameClipView(recalboxTheme, "3-240p");

      if (!Strings::Contains(conf.GetThemeGamelistView(recalboxTheme), "240p"))
        conf.SetThemeGamelistView(recalboxTheme, "10-240p");
    }
    conf.SetBool("240ptestsuite.ignore", false);
  }
}

void MainRunner::PatreonState(PatronAuthenticationResult result, int level, const std::string& name)
{
  std::string message;
  switch(result)
  {
    case PatronAuthenticationResult::Patron:
    {
      message = _("Welcome back %NAME%!\nPatron level %LEVEL%\nYou are now connected to your recalbox patron account, and all exclusives features are available!");
      Strings::ReplaceAllIn(message, "%NAME%", name);
      Strings::ReplaceAllIn(message, "%LEVEL%", Strings::ToString(level));
      break;
    }
    case PatronAuthenticationResult::FormerPatron:
    {
      message = _("Hello %NAME%, your private key is linked to a Patreon account which is no longer a Recalbox Patron.\nWe still hope to see you back soon as a Recalbox Patron!\nDelete your private key to suppress this message.");
      Strings::ReplaceAllIn(message, "%NAME%", name);
      break;
    }
    case PatronAuthenticationResult::Invalid:
    {
      message = _("Your private key does not allow to retrieve your Patreon information. Go to recalbox.com/patreon to generate a new valid key!");
      break;
    }
    case PatronAuthenticationResult::NetworkError:
    {
      message = _("Sorry we're not able to retrieve your Patron level because no network is available!");
      break;
    }
    case PatronAuthenticationResult::HttpError:
    case PatronAuthenticationResult::ApiError:
    {
      message = _("We're not able to retrieve your Patron level! Sorry for the inconvenience, we're already working on a fix!");
      break;
    }
    case PatronAuthenticationResult::NoPatron:
    case PatronAuthenticationResult::Unknown:
    default: break;
  }

  if (!message.empty())
    mApplicationWindow->InfoPopupAdd(new GuiInfoPopup(*mApplicationWindow, message, 15, GuiInfoPopupBase::PopupType::Help));
}

bool MainRunner::Execute(GuiWaitLongExecution<USBInitialization, bool>& from, const USBInitialization& parameter)
{
  (void) from;
  switch(parameter.Action())
  {
    case USBInitializationAction::None: break;
    case USBInitializationAction::OnlyRomFolders: return SystemManager::CreateRomFoldersIn(parameter.Device()); break;
    case USBInitializationAction::CompleteShare:
    {
      struct
      {
        Path From;
        Path To;
      }
      pathFromTo[]
      {
        { Path("/recalbox/share/bios")       , parameter.Device().MountPoint() / "recalbox/bios"       },
        { Path("/recalbox/share/cheats")     , parameter.Device().MountPoint() / "recalbox/cheats"     },
        { Path("/recalbox/share/kodi")       , parameter.Device().MountPoint() / "recalbox/kodi"       },
        { Path("/recalbox/share/music")      , parameter.Device().MountPoint() / "recalbox/music"      },
        { Path("/recalbox/share/overlays")   , parameter.Device().MountPoint() / "recalbox/overlays"   },
        { Path("/recalbox/share/roms")       , parameter.Device().MountPoint() / "recalbox/roms"       },
        { Path("/recalbox/share/saves")      , parameter.Device().MountPoint() / "recalbox/saves"      },
        { Path("/recalbox/share/screenshots"), parameter.Device().MountPoint() / "recalbox/screenshots"},
        { Path("/recalbox/share/shaders")    , parameter.Device().MountPoint() / "recalbox/shaders"    },
        { Path("/recalbox/share/system")     , parameter.Device().MountPoint() / "recalbox/system"     },
        { Path("/recalbox/share/themes")     , parameter.Device().MountPoint() / "recalbox/themes"     },
        { Path("/recalbox/share/userscripts"), parameter.Device().MountPoint() / "recalbox/userscripts"},
      };

      // Copy share folders
      for(const auto& ft : pathFromTo)
      {
        { LOG(LogInfo) << "[MainRunner] USB Initialization: Copying " << ft.From.Filename(); }
        from.SetText(Strings::Replace(_("Copying %s folder..."), "%s", ft.From.Filename()));
        if (!Files::CopyFolder(ft.From, ft.To, true)) return false;
      }

      // Install new device
      { LOG(LogInfo) << "[MainRunner] USB Initialization: Setup boot device."; }
      from.SetText(_("Setting up boot device..."));
      StorageDevices storages;
      for(const StorageDevices::Device& storage : storages.GetStorageDevices())
        if (storage.DevicePath == parameter.Device().Device().ToString())
        {
          { LOG(LogInfo) << "[MainRunner] USB Initialization: Boot device set to " << storage.UUID; }
          storages.SetStorageDevice(storage);
          return true;
        }
    }
  }
  return false;
}

void MainRunner::Completed(const USBInitialization& parameter, const bool& result)
{
  switch(parameter.Action())
  {
    case USBInitializationAction::None: break;
    case USBInitializationAction::OnlyRomFolders:
    {
      mApplicationWindow->pushGui(new GuiMsgBox(*mApplicationWindow, result ? _("Your USB device has been initialized! You can unplug it and copy your games on it.")
                                                                            : _("Initialization failed! Your USB device is full or contains errors. Please repair or use another device."), _("OK"), nullptr));
      break;
    }
    case USBInitializationAction::CompleteShare:
    {
      if (result)
        mApplicationWindow->pushGui(new GuiMsgBox(*mApplicationWindow, _("Your USB device has been initialized! Ready to reboot on your new share device!"),
                                                  _("OK"), [] { MainRunner::RequestQuit(MainRunner::ExitState::Relaunch); }));
      else
        mApplicationWindow->pushGui(new GuiMsgBox(*mApplicationWindow, _("Initialization failed! Your USB device is full or contains errors. Please repair or use another device."), _("OK"), nullptr));
      break;
    }
  }
}

void MainRunner::Sdl2EventReceived(const SDL_Event& event)
{
  if (event.type == SDL_KEYDOWN)
    ProcessSpecialInputs(InputManager::Instance().ManageSDLEvent(mApplicationWindow, event));
}

bool MainRunner::ProcessSpecialInputs(const InputCompactEvent& event)
{
  { LOG(LogInfo) << "[MainRunner] Processing special input."; }

  const InputEvent& raw = event.RawEvent();
  if (raw.Type() == InputEvent::EventType::Key)
    if (raw.Value() == 1) // KEYDOWN
      switch(raw.Id())
      {
        case SDLK_POWER: PowerButtonPressed(Board::Instance().GetBoardType(), sPowerButtonThreshold); return true;
        case SDLK_SLEEP: ResetButtonPressed(Board::Instance().GetBoardType()); return true;
        default: break;
      }

  return false;
}
