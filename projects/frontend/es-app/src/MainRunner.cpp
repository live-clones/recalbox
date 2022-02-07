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
#include "netplay/NetPlayThread.h"
#include "DemoMode.h"
#include <sdl2/Sdl2Runner.h>
#include <systems/GameRunner.h>
#include <sdl2/Sdl2Init.h>

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
    SystemManager systemManager;
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

    // Run kodi at startup?
    if (RecalboxSystem::kodiExists())
      if ((mRunCount == 0) && mConfiguration.GetKodiEnabled() && mConfiguration.GetKodiAtStartup())
        GameRunner::Instance().RunKodi();

    // Scrapers
    ScraperFactory scraperFactory;

    ExitState exitState;
    try
    {
      // Start update thread
      { LOG(LogDebug) << "[MainRunner] Launching Network thread"; }
      Upgrade networkThread(window);
      // Start the socket server
      { LOG(LogDebug) << "[MainRunner] Launching Command thread"; }
      CommandThread commandThread(systemManager);
      // Start Video engine
      { LOG(LogDebug) << "[MainRunner] Launching Video engine"; }
      VideoEngine videoEngine;
      // Start Neyplay thread
      { LOG(LogDebug) << "[MainRunner] Launching Netplay thread"; }
      NetPlayThread netPlayThread(window);

      // Update?
      CheckUpdateMessage(window);
      CheckUpdateFailed(window);
      // Input ok?
      CheckAndInitializeInput(window);
      // Wizard
      CheckFirstTimeWizard(window);

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
      case ExitState::FastReboot: NotificationManager::Instance().Notify(Notification::Reboot, exitState == ExitState::FastReboot ? "fast" : "normal"); break;
      case ExitState::Shutdown:
      case ExitState::FastShutdown: NotificationManager::Instance().Notify(Notification::Shutdown, exitState == ExitState::FastShutdown ? "fast" : "normal"); break;
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

  GameRunner gameRunner(window, systemManager);
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
          InputCompactEvent compactEvent = InputManager::Instance().ManageSDLEvent(&window, event);
          // Process
          if (!compactEvent.Empty())
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

void MainRunner::Intro(bool debug)
{
  atexit(&onExit); // Always close the log on exit

  if (debug || mConfiguration.AsBool("emulationstation.debuglogs", false))
  {
    Log::setReportingLevel(LogLevel::LogDebug);
    SDL_LogSetOutputFunction(Sdl2Log, nullptr);
    SDL_LogSetAllPriority(SDL_LogPriority::SDL_LOG_PRIORITY_VERBOSE);
  }

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
        std::string Headphones = "alsa_card.0:analog-output-headphones";
        if (IsApplicationRunning()) AudioManager::Instance().Deactivate();
        AudioController::Instance().SetDefaultPlayback(Headphones);
        if (IsApplicationRunning()) AudioManager::Instance().Reactivate();
        //RecalboxConf::Instance().SetAudioOuput(Headphones);
        // Create music popup
        int popupDuration = RecalboxConf::Instance().GetPopupMusic();
        { LOG(LogInfo) << "[OdroidAdvanceGo] Switch to Headphone!" << popupDuration << " " << (mApplicationWindow != nullptr ? "ok" : "nok"); }
        if (popupDuration != 0 && mApplicationWindow != nullptr)
          mApplicationWindow->InfoPopupAdd(new GuiInfoPopup(*mApplicationWindow, _("Switch audio output to Headphones!"),
                                                popupDuration, GuiInfoPopup::PopupType::Music));
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
        std::string Speakers = "alsa_card.0:multichannel-output";
        if (IsApplicationRunning()) AudioManager::Instance().Deactivate();
        AudioController::Instance().SetDefaultPlayback(Speakers);
        if (IsApplicationRunning()) AudioManager::Instance().Reactivate();
        //RecalboxConf::Instance().SetAudioOuput(Speakers);
        // Create music popup
        int popupDuration = RecalboxConf::Instance().GetPopupMusic();
        { LOG(LogInfo) << "[OdroidAdvanceGo] Switch to Speaker!" << popupDuration << " " << (mApplicationWindow != nullptr ? "ok" : "nok"); }
        if (popupDuration != 0 && mApplicationWindow != nullptr)
          mApplicationWindow->InfoPopupAdd(new GuiInfoPopup(*mApplicationWindow, _("Switch audio output back to Speakers!"),
                                                popupDuration, GuiInfoPopup::PopupType::Music));
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

void MainRunner::PowerButtonPressed(BoardType board, int milliseconds)
{
  (void)board;
  if (IsApplicationRunning())
  {
    // The application is running and is on screen.
    // Display little window to notify the user
    if (milliseconds < 500)
      mApplicationWindow->pushGui((new GuiWaitLongExecution<HardwareTriggeredSpecialOperations, bool>(*mApplicationWindow, *this))
                                    ->Execute(HardwareTriggeredSpecialOperations::Suspend, _("Entering standby...")));
    else
      mApplicationWindow->pushGui((new GuiWaitLongExecution<HardwareTriggeredSpecialOperations, bool>(*mApplicationWindow, *this))
                                    ->Execute(HardwareTriggeredSpecialOperations::PowerOff, _("Bye bye!")));
  }
  else
  {
    // The application is not Running, execute orders immediately
    if (milliseconds < 500)
      Board::Instance().Suspend();
    else
    {
      Files::SaveFile(Path(sQuitNow), std::string());
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
    case HardwareTriggeredSpecialOperations::Suspend:
    case HardwareTriggeredSpecialOperations::Resume:
    case HardwareTriggeredSpecialOperations::PowerOff:
    default: sleep(1); // Just sleep one second
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