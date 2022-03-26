//
// Created by bkg2k on 02/09/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include <audio/AudioManager.h>
#include <padtokeyboard/PadToKeyboardManager.h>
#include <usernotifications/NotificationManager.h>
#include <utils/cplusplus/StaticLifeCycleControler.h>
#include <VideoEngine.h>
#include <input/InputMapper.h>
#include <utils/Files.h>
#include <sdl2/Sdl2Runner.h>
#include <sdl2/Sdl2Init.h>
#include <MainRunner.h>
#include <hardware/crt/CrtAdapterDetector.h>
#include "GameRunner.h"

bool GameRunner::sGameIsRunning = false;

IBoardInterface::CPUGovernance GameRunner::GetGovernance(const std::string& core)
{
  static IniFile governanceFile(Path(sGovernanceFile), false);

  std::string governance = governanceFile.AsString(core);

  if (governance.empty()) return IBoardInterface::CPUGovernance::FullSpeed;
  if (governance == "powersave") return IBoardInterface::CPUGovernance::PowerSave;
  if (governance == "ondemand") return IBoardInterface::CPUGovernance::OnDemand;
  if (governance == "performance") return IBoardInterface::CPUGovernance::FullSpeed;
  { LOG(LogError) << "[Gamelist] Unreconized governance " << governance << " in " << sGovernanceFile; }
  return IBoardInterface::CPUGovernance::FullSpeed;
}

int GameRunner::Run(const std::string& cmd_utf8, bool debug)
{
  static std::string output("/recalbox/share/system/logs/es_launch_stdout.log");
  static std::string outerr("/recalbox/share/system/logs/es_launch_stderr.log");

  // Run command
  std::string cmd(cmd_utf8);
  cmd.append(" > ").append(output)
     .append(" 2> ").append(outerr);
  int exitcode = system(cmd.data());

  // Get logs
  if (debug)
  {
    Path outPath(output);
    Path errPath(outerr);

    static constexpr int sLogSizeLimit = 2 << 20; // 2Mb

    // stdout
    if (outPath.Size() > sLogSizeLimit)
    {
      long long size = outPath.Size();
      std::string start = Files::LoadFile(outPath, 0, sLogSizeLimit / 2);
      std::string stop = Files::LoadFile(outPath, size - (sLogSizeLimit / 2), sLogSizeLimit / 2);
      { LOG(LogInfo) << "[Run] Configgen Output:\n" << start << "\n...\n" << stop; }
    }
    else
    {
      std::string content = Files::LoadFile(outPath);
      if (!content.empty()) { LOG(LogInfo) << "[Run] Configgen Output:\n" << content; }
    }

    // stderr
    if (errPath.Size() > sLogSizeLimit)
    {
      long long size = errPath.Size();
      std::string start = Files::LoadFile(errPath, 0, sLogSizeLimit / 2);
      std::string stop = Files::LoadFile(errPath, size - (sLogSizeLimit / 2), sLogSizeLimit / 2);
      { LOG(LogInfo) << "[Run] Configgen Errors:\n" << start << "\n...\n" << stop; }
    }
    else
    {
      std::string content = Files::LoadFile(errPath);
      if (!content.empty()) { LOG(LogInfo) << "[Run] Configgen Errors:\n" << content; }
    }
  }

  // Return state
  return exitcode;
}

bool GameRunner::RunGame(FileData& game, const EmulatorData& emulator, const GameLinkedData& data)
{
  // Automatic running flag management
  GameRunning IAmRunning;

  { LOG(LogInfo) << "[Run] Launching game..."; }

  InputMapper mapper(nullptr);
  OrderedDevices controllers = InputManager::Instance().GetMappedDeviceList(mapper);
  std::string controlersConfig = InputManager::Instance().GetMappedDeviceListConfiguration(mapper);
  { LOG(LogInfo) << "[Run] Controllers config : " << controlersConfig; }

  VideoEngine::Instance().StopVideo(false);
  AudioManager::Instance().Deactivate();
  WindowManager::Finalize();

  std::string command = game.System().Descriptor().Command();

  const std::string rom = game.FilePath().MakeEscaped();
  const std::string basename = game.FilePath().FilenameWithoutExtension();
  const std::string rom_raw = game.FilePath().ToString();
  const std::string& core = data.NetPlay().NetplayMode() == NetPlayData::Mode::Client ? data.NetPlay().CoreName() : emulator.Core();

  Strings::ReplaceAllIn(command, "%ROM%", rom);
  Strings::ReplaceAllIn(command, "%CONTROLLERSCONFIG%", controlersConfig);
  Strings::ReplaceAllIn(command, "%SYSTEM%", game.System().Name());
  Strings::ReplaceAllIn(command, "%BASENAME%", basename);
  Strings::ReplaceAllIn(command, "%ROM_RAW%", rom_raw);
  Strings::ReplaceAllIn(command, "%EMULATOR%", emulator.Emulator());
  Strings::ReplaceAllIn(command, "%CORE%", core);
  Strings::ReplaceAllIn(command, "%RATIO%", game.Metadata().RatioAsString());
  Strings::ReplaceAllIn(command, "%NETPLAY%", NetplayOption(game, data.NetPlay()));
  Strings::ReplaceAllIn(command, "%CRT%", BuildCRTOptions(data.Crt(), false));

  bool debug = RecalboxConf::Instance().GetDebugLogs();
  if (debug) command.append(" -verbose");

  { LOG(LogInfo) << "[Run] Command: " << command; }
  int exitCode = -1;
  {
    Sdl2Runner sdl2Runner;

    PadToKeyboardManager padToKeyboard(controllers, game.FilePath(), sdl2Runner);
    padToKeyboard.StartMapping();
    if (padToKeyboard.IsValid() ||
        game.System().Descriptor().Type() == SystemDescriptor::SystemType::Computer) command.append(" -nodefaultkeymap");
    NotificationManager::Instance().Notify(game, Notification::RunGame);

    Board::Instance().SetCPUGovernance(GetGovernance(core));
    Board::Instance().StartInGameBackgroundProcesses(sdl2Runner);
    fputs("==============================================\n", stdout);
    // Start game thread
    ThreadRunner gameRunner(sdl2Runner, command, debug);
    // Start sdl2 loop
    sdl2Runner.Run();
    exitCode = gameRunner.ExitCode();
    fputs("==============================================\n", stdout);
    Board::Instance().StopInGameBackgroundProcesses(sdl2Runner);
    Board::Instance().SetCPUGovernance(IBoardInterface::CPUGovernance::PowerSave);
    NotificationManager::Instance().Notify(game, Notification::EndGame);
    padToKeyboard.StopMapping();

    if (exitCode != 0) { LOG(LogWarning) << "[Run] Non-Zero exit code " << exitCode << " !"; }
    else LOG(LogInfo) << "[Run] No error running " << game.FilePath().ToString();
  }

  // Reinit
  Sdl2Init::Finalize();
  Sdl2Init::Initialize();
  mWindowManager.ReInitialize();
  mWindowManager.normalizeNextUpdate();
  AudioManager::Instance().Reactivate();
  InputManager::Instance().Refresh(&mWindowManager, false);

  // Update number of times the game has been launched
  game.Metadata().IncPlaycount();

  // Update last played time
  mSystemManager.UpdateLastPlayedSystem(game);
  game.Metadata().SetLastplayedNow();

  return exitCode == 0;
}

std::string GameRunner::demoInitialize()
{
  { LOG(LogInfo) << "[DemoMode] Entering demo mode..."; }

  InputMapper mapper(nullptr);
  std::string controlersConfig = InputManager::Instance().GetMappedDeviceListConfiguration(mapper);
  { LOG(LogInfo) << "[DemoMode] Controllers config : " << controlersConfig; }

  VideoEngine::Instance().StopVideo(false);
  AudioManager::Instance().Deactivate();
  WindowManager::Finalize();

  return controlersConfig;
}

void GameRunner::demoFinalize()
{
  // Reinit
  Sdl2Init::Finalize();
  Sdl2Init::Initialize();
  mWindowManager.ReInitialize();
  mWindowManager.normalizeNextUpdate();
  AudioManager::Instance().Reactivate();
  InputManager::Instance().Refresh(&mWindowManager, false);
}

bool
GameRunner::DemoRunGame(const FileData& game, const EmulatorData& emulator, int duration, int infoscreenduration, const std::string& controlersConfig)
{
  // Automatic running flag management
  GameRunning IAmRunning;

  { LOG(LogInfo) << "[Run] Launching game demo..."; }

  std::string command = game.System().Descriptor().Command();

  const std::string rom = game.FilePath().MakeEscaped();
  const std::string basename = game.FilePath().FilenameWithoutExtension();
  const std::string rom_raw = game.FilePath().ToString();

  Strings::ReplaceAllIn(command, "%ROM%", rom);
  Strings::ReplaceAllIn(command, "%CONTROLLERSCONFIG%", controlersConfig);
  Strings::ReplaceAllIn(command, "%SYSTEM%", game.System().Name());
  Strings::ReplaceAllIn(command, "%BASENAME%", basename);
  Strings::ReplaceAllIn(command, "%ROM_RAW%", rom_raw);
  Strings::ReplaceAllIn(command, "%EMULATOR%", emulator.Emulator());
  Strings::ReplaceAllIn(command, "%CORE%", emulator.Core());
  Strings::ReplaceAllIn(command, "%RATIO%", game.Metadata().RatioAsString());
  Strings::ReplaceAllIn(command, "%NETPLAY%", "");
  Strings::ReplaceAllIn(command, "%CRT%", BuildCRTOptions(GameLinkedData().Crt(), true));

  // Add demo stuff
  command.append(" -demo 1");
  command.append(" -demoduration ").append(Strings::ToString(duration));
  command.append(" -demoinfoduration ").append(Strings::ToString(infoscreenduration));

  bool debug = RecalboxConf::Instance().GetDebugLogs();
  if (debug)
    command.append(" -verbose");

  { LOG(LogInfo) << "[Run] Demo command: " << command; }
  NotificationManager::Instance().Notify(game, Notification::RunDemo);
  Board::Instance().SetCPUGovernance(IBoardInterface::CPUGovernance::FullSpeed);
  int exitCode = WEXITSTATUS(Run(command, debug));
  Board::Instance().SetCPUGovernance(IBoardInterface::CPUGovernance::PowerSave);
  NotificationManager::Instance().Notify(game, Notification::EndDemo);
  { LOG(LogInfo) << "[Run] Demo exit code :	" << exitCode; }


  // Configgen returns an exitcode 0x33 when the user interact with any pad/mouse
  if (exitCode == 0x33)
  {
    { LOG(LogInfo) << "[Run] Exiting demo upon user request"; }
    return true;
  }

  if (exitCode != 0)
  { LOG(LogWarning) << "[Run] ...launch terminated with nonzero exit code " << exitCode << "!"; }

  return false;
}

std::string GameRunner::NetplayOption(const FileData& game, const NetPlayData& netplay)
{
  switch(netplay.NetplayMode())
  {
    case NetPlayData::Mode::None: break;
    case NetPlayData::Mode::Client:
    {
      std::string netplayLine("-netplay client -netplay_port ");
      netplayLine.append(Strings::ToString(netplay.Port())).append(" -netplay_ip ").append(netplay.Ip());
      if (!netplay.PlayerPassword().empty()) netplayLine.append(" -netplay_playerpassword ").append(1, '"').append(netplay.PlayerPassword()).append(1, '"');
      if (!netplay.ViewerPassword().empty()) netplayLine.append(" -netplay_viewerpassword ").append(1, '"').append(netplay.ViewerPassword()).append(1, '"');
      if (netplay.IsViewerOnly()) netplayLine.append(" -netplay_vieweronly");
      if (game.Metadata().RomCrc32() != 0) netplayLine.append(" -hash ").append(game.Metadata().RomCrc32AsString());
      return netplayLine;
    }
    case NetPlayData::Mode::Server:
    {
      std::string netplayLine("-netplay host -netplay_port ");
      netplayLine.append(Strings::ToString(netplay.Port()));
      if (!netplay.PlayerPassword().empty()) netplayLine.append(" -netplay_playerpassword ").append(1, '"').append(netplay.PlayerPassword()).append(1, '"');
      if (!netplay.ViewerPassword().empty()) netplayLine.append(" -netplay_viewerpassword ").append(1, '"').append(netplay.ViewerPassword()).append(1, '"');
      if (game.Metadata().RomCrc32() != 0) netplayLine.append(" -hash ").append(game.Metadata().RomCrc32AsString());
      return netplayLine;
    }
  }

  return "";
}

bool GameRunner::RunKodi()
{
  { LOG(LogInfo) << "[System] Attempting to launch kodi..."; }

  std::string commandline = demoInitialize();
  std::string command = "configgen -system kodi -rom '' " + commandline;

  NotificationManager::Instance().NotifyKodi();

  bool debug = RecalboxConf::Instance().GetDebugLogs();
  int exitCode = Run(command.c_str(), debug);
  if (WIFEXITED(exitCode))
  {
    exitCode = WEXITSTATUS(exitCode);
  }

  demoFinalize();

  // handle end of kodi
  switch (exitCode)
  {
    case 10: { MainRunner::RequestQuit(MainRunner::ExitState::NormalReboot); return true; }
    case 11: { MainRunner::RequestQuit(MainRunner::ExitState::Shutdown);     return true; }
    default: break;
  }

  return exitCode == 0;
}

std::string GameRunner::BuildCRTOptions(const CrtData& data, const bool demo)
{
  std::string result;

  const ICrtInterface& crtBoard = Board::Instance().CrtBoard();
  if (crtBoard.IsCrtAdapterAttached()) {
    result.append(" -crtadaptor ").append("present");
    result.append(" -crtscreentype ").append(crtBoard.GetHorizontalFrequency() == ICrtInterface::HorizontalFrequency::KHz15 ? "15kHz" : "31kHz");
    result.append(" -crtverticaloffset ").append(Strings::ToString(CrtConf::Instance().GetSystemCRTVerticalOffset()));
    result.append(" -crthorizontaloffset ").append(Strings::ToString(CrtConf::Instance().GetSystemCRTHorizontalOffset()));
    result.append(" -crtviewportwidth ").append(Strings::ToString(CrtConf::Instance().GetSystemCRTViewportWidth()));

    // Resolution type
    if(crtBoard.GetHorizontalFrequency() == ICrtInterface::HorizontalFrequency::KHz31)
    {
      // force 240p only if game resolution select is active and 240p is selected
      if(demo)
        result.append(" -crtresolutiontype ").append(CrtConf::Instance().GetSystemCRTRunDemoIn240pOn31kHz() ? "doublefreq" : "progressive");
      else if(CrtConf::Instance().GetSystemCRTGameResolutionSelect())
        result.append(" -crtresolutiontype ").append(data.HighResolution() ? "progressive" : "doublefreq");
      else
        result.append(" -crtresolutiontype ").append("progressive");
      result.append(" -crtvideostandard ntsc");
      // Scanlines
      if(CrtConf::Instance().GetSystemCRTScanlines31kHz())
        result.append(" -crtscanlines 1");
    }
    else
    {
      result.append(" -crtresolutiontype ").append(data.HighResolution() ? "interlaced" : "progressive");

        result.append(" -crtvideostandard ");
        // Force pal if switch 50hz
        if (crtBoard.MustForce50Hz())
        {
            result.append("pal");
        } else
        {
            switch (data.VideoStandard())
            {
                case CrtData::CrtVideoStandard::PAL:
                    result.append("pal");
                    break;
                case CrtData::CrtVideoStandard::NTSC:
                    result.append("ntsc");
                    break;
                case CrtData::CrtVideoStandard::AUTO:
                default:
                    result.append("auto");
                    break;
            }
        }

        result.append(" -crtregion ");
        // Force eu if switch 50hz
        if (crtBoard.MustForce50Hz())
        {
            result.append("eu");
        } else
        {
            switch (data.Region())
            {
                case CrtData::CrtRegion::EU:
                    result.append("eu");
                    break;
                case CrtData::CrtRegion::US:
                    result.append("us");
                    break;
                case CrtData::CrtRegion::JP:
                    result.append("jp");
                    break;
                case CrtData::CrtRegion::AUTO:
                default:
                    result.append("auto");
                    break;
            }
        }
    }
  }

  return result;
}
