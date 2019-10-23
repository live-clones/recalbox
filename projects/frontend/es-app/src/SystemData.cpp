#include "SystemData.h"
#include "Gamelist.h"
#include "AudioManager.h"
#include "VolumeControl.h"
#include "utils/Log.h"
#include "Util.h"
#include <RecalboxConf.h>
#include <RootFolders.h>
#include <recalbox/RecalboxSystem.h>
#include <VideoEngine.h>
#include <utils/StringUtil.h>
#include <utils/os/fs/FileSystemUtil.h>

namespace fs = boost::filesystem;

SystemData::EmulatorDescriptor SystemData::EmulatorList::sEmptyEmulator("NO EMULATOR");

SystemData::SystemData(const std::string& name,
                       const std::string& fullName,
                       const std::string& startPath,
                       const std::string& filteredExtensions,
                       const std::string& command,
                       const std::vector<PlatformIds::PlatformId>& platformIds,
                       const std::string& themeFolder,
                       const EmulatorList* emus,
                       bool childOwnership,
                       bool favorite)
  : mName(name),
    mFullName(fullName),
    mStartPath(getExpandedPath(startPath)),
    mSearchExtensions(filteredExtensions),
    mLaunchCommand(command),
    mPlatformCount(0),
    mPlatformIds(),
    mThemeFolder(themeFolder),
    mEmulators(),
    mRootFolder(childOwnership, mStartPath, this),
    mSortId(RecalboxConf::getInstance()->getUInt(mName + ".sort")),
    mIsFavorite(favorite)
{
  // Copy emu tree
  if (emus != nullptr)
    mEmulators = *emus;

  // Set name
  mRootFolder.Metadata().SetName(mFullName);

  // Convert platforms
  mPlatformCount = platformIds.size();
  if (mPlatformCount >= sMaximumPlatformIds)
  {
    LOG(LogError) << "Platform count for system " << mName << " clipped to " << sMaximumPlatformIds;
    mPlatformCount = sMaximumPlatformIds;
  }
  for (int i = sMaximumPlatformIds; --i >= 0;)
    mPlatformIds[i] = PlatformIds::PlatformId::PLATFORM_UNKNOWN;
  for (int i = mPlatformCount; --i >= 0;)
    mPlatformIds[i] = platformIds[i];
}

void SystemData::RunGame(Window& window, FileData& game, const std::string& netplay, const std::string& core,
                         const std::string& ip, const std::string& port)
{
  (void) core;

  LOG(LogInfo) << "Attempting to launch game...";

  VideoEngine::This().StopVideo();
  AudioManager::getInstance()->deinit();
  VolumeControl::getInstance()->deinit();

  std::string controlersConfig = InputManager::Instance().GenerateConfiggenConfiguration();
  LOG(LogInfo) << "Controllers config : " << controlersConfig;
  window.deinit();


  std::string command = mLaunchCommand;

  const std::string rom = FileSystemUtil::getEscapedPath(game.getPath().generic_string());
  const std::string basename = game.getPath().stem().string();
  const std::string rom_raw = fs::path(game.getPath()).make_preferred().string();

  command = StringUtil::replace(command, "%ROM%", rom);
  command = StringUtil::replace(command, "%CONTROLLERSCONFIG%", controlersConfig);
  command = StringUtil::replace(command, "%SYSTEM%", game.getSystem()->getName());
  command = StringUtil::replace(command, "%BASENAME%", basename);
  command = StringUtil::replace(command, "%ROM_RAW%", rom_raw);
  command = StringUtil::replace(command, "%EMULATOR%", game.Metadata().Emulator());
  command = StringUtil::replace(command, "%CORE%", game.Metadata().Core());
  command = StringUtil::replace(command, "%RATIO%", game.Metadata().RatioAsString());

  if (netplay == "client")
  {
    command = StringUtil::replace(command, "%NETPLAY%", "-netplay client -netplay_port " + port + " -netplay_ip " + ip);
  }
  else if (netplay == "host")
  {
    std::string hash = game.Metadata().RomCrc32AsString();
    std::string hashcmd;
    if (!hash.empty())
    {
      hashcmd = " -hash " + hash;
    }
    command = StringUtil::replace(command, "%NETPLAY%", "-netplay host -netplay_port " +
                                                        RecalboxConf::getInstance()->get("global.netplay.port") +
                                                        hashcmd);
  }
  else
  {
    command = StringUtil::replace(command, "%NETPLAY%", "");
  }


  LOG(LogInfo) << "	" << command;
  std::cout << "==============================================\n";
  RecalboxSystem::NotifyGame(game, true, false);
  int exitCode = runSystemCommand(command);
  RecalboxSystem::NotifyGame(game, false, false);
  std::cout << "==============================================\n";

  if (exitCode != 0)
  {
    LOG(LogWarning) << "...launch terminated with nonzero exit code " << exitCode << "!";
  }

  // Reinit
  window.init();
  VolumeControl::getInstance()->init();
  AudioManager::getInstance()->resumeMusic();
  window.normalizeNextUpdate();

  //update number of times the game has been launched
  game.Metadata().IncPlaycount();

  //update last played time
  game.Metadata().SetLastplayedNow();
}

std::string SystemData::demoInitialize(Window& window)
{
  LOG(LogInfo) << "Entering demo mode...";

  std::string controlersConfig = InputManager::Instance().GenerateConfiggenConfiguration();
  LOG(LogInfo) << "Controllers config : " << controlersConfig;

  VideoEngine::This().StopVideo();
  AudioManager::getInstance()->deinit();
  VolumeControl::getInstance()->deinit();

  window.deinit();

  return controlersConfig;
}

void SystemData::demoFinalize(Window& window)
{
  // Reinit
  window.init();
  VolumeControl::getInstance()->init();
  AudioManager::getInstance()->resumeMusic();
  window.normalizeNextUpdate();
}

bool
SystemData::DemoRunGame(const FileData& game, int duration, int infoscreenduration, const std::string& controlersConfig)
{
  std::string command = mLaunchCommand;

  const std::string rom = FileSystemUtil::getEscapedPath(game.getPath().generic_string());
  const std::string basename = game.getPath().stem().string();
  const std::string rom_raw = fs::path(game.getPath()).make_preferred().string();

  command = StringUtil::replace(command, "%ROM%", rom);
  command = StringUtil::replace(command, "%CONTROLLERSCONFIG%", controlersConfig);
  command = StringUtil::replace(command, "%SYSTEM%", game.getSystem()->getName());
  command = StringUtil::replace(command, "%BASENAME%", basename);
  command = StringUtil::replace(command, "%ROM_RAW%", rom_raw);
  command = StringUtil::replace(command, "%EMULATOR%", game.Metadata().Emulator());
  command = StringUtil::replace(command, "%CORE%", game.Metadata().Core());
  command = StringUtil::replace(command, "%RATIO%", game.Metadata().RatioAsString());
  command = StringUtil::replace(command, "%NETPLAY%", "");

  // Add demo stuff
  command += " -demo 1";
  command += " -demoduration ";
  command += std::to_string(duration);
  command += " -demoinfoduration ";
  command += std::to_string(infoscreenduration);

  LOG(LogInfo) << "Demo command: " << command;
  RecalboxSystem::NotifyGame(game, true, true);
  int exitCode = runSystemCommand(command);
  RecalboxSystem::NotifyGame(game, false, false);
  LOG(LogInfo) << "Demo exit code :	" << exitCode;


  // Configgen returns an exitcode 0x33 when the user interact with any pad/mouse
  // this exitcode returns here byte-swapped or shifted. Need further investigation
  if (exitCode == 0x3300)
  {
    LOG(LogInfo) << "Exiting demo upon user request";
    return true;
  }

  if (exitCode != 0)
  {
    LOG(LogWarning) << "...launch terminated with nonzero exit code " << exitCode << "!";
  }

  return false;
}

void SystemData::populateFolder(FolderData* folder, FileData::StringMap& doppelgangerWatcher)
{
  LOG(LogInfo) << folder->getSystem()->getFullName() << ": Searching games/roms in "
               << folder->getPath().generic_string() << "...";

  try
  {
    folder->populateRecursiveFolder(mSearchExtensions, this, doppelgangerWatcher);
  }
  catch (std::exception& ex)
  {
    LOG(LogError) << "Reading folder \"" << folder->getPath().generic_string() << "\" has raised an error!";
    LOG(LogError) << "Exception: " << ex.what();
  }
}

std::string SystemData::getGamelistPath(bool forWrite) const
{
  fs::path filePath;
  filePath = mRootFolder.getPath() / "gamelist.xml";
  if (forWrite && !fs::exists(filePath))
    fs::create_directories(filePath.parent_path());
  return filePath.generic_string();
}

std::string SystemData::getThemePath() const
{
  // where we check for themes, in order:
  // 1. [SYSTEM_PATH]/theme.xml
  // 2. system theme from currently selected theme set [CURRENT_THEME_PATH]/[SYSTEM]/theme.xml
  // 3. default system theme from currently selected theme set [CURRENT_THEME_PATH]/theme.xml

  // first, check game folder
  fs::path localThemePath = mRootFolder.getPath() / "theme.xml";
  if (fs::exists(localThemePath)) return localThemePath.generic_string();

  // not in game folder, try system theme in theme sets
  localThemePath = ThemeData::getThemeFromCurrentSet(mThemeFolder);
  if (fs::exists(localThemePath)) return localThemePath.generic_string();

  // not system theme, try default system theme in theme set
  localThemePath = localThemePath.parent_path().parent_path() / "theme.xml";
  if (fs::exists(localThemePath)) return localThemePath.generic_string();

  //none of the above, try default
  localThemePath = localThemePath.parent_path() / "default/theme.xml";
  if (fs::exists(localThemePath)) return localThemePath.generic_string();

  // No luck...
  return "";
}

void SystemData::loadTheme()
{
  mTheme = std::make_shared<ThemeData>();

  std::string path = getThemePath();

  if (!fs::exists(path)) // no theme available for this platform
    return;

  try
  {
    mTheme->loadFile(getThemeFolder(), path);
  }
  catch (ThemeException& e)
  {
    LOG(LogError) << e.what();
    mThemeFolder = "default";
    mTheme->loadFile(getThemeFolder(), path);
  }
}

