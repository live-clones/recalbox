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

namespace fs = boost::filesystem;

SystemData::SystemData(const std::string& name,
                       const std::string& fullName,
                       const std::string& startPath,
                       const std::string& filteredExtensions,
                       const std::string& command,
                       const std::vector<PlatformIds::PlatformId>& platformIds,
                       const std::string& themeFolder,
                       const Tree* emuNodes,
                       bool childOwnership,
                       bool favorite)
  : mName(name),
    mFullName(fullName),
    mStartPath(getExpandedPath(startPath)),
    mSearchExtensions(filteredExtensions),
    mLaunchCommand(command),
    mPlatformIds(platformIds),
    mThemeFolder(themeFolder),
    mEmulators(),
    mRootFolder(childOwnership, mStartPath, this),
    mSortId(RecalboxConf::getInstance()->getUInt(mName + ".sort")),
    mIsFavorite(favorite)
{
  mRootFolder.Metadata().SetName(mFullName);

  // emulators and cores
  if (emuNodes != nullptr)
  {
    //SystemData::Tree emulatorsNode = system.get("emulators", "");
    for (const auto& emuNode : *emuNodes)
    {
      const std::string& emulatorName = emuNode.second.get_child("<xmlattr>").get("name", "");
      mEmulators[emulatorName] = new std::vector<std::string>();
      for (const auto& coreNode : emuNode.second.get_child("cores"))
      {
        const std::string& corename = coreNode.second.data();
        mEmulators[emulatorName]->push_back(corename);
      }
    }
  }
}

SystemData::~SystemData()
{
  for (const auto &itr : mEmulators)
      delete (itr.second);
}

std::string strreplace(std::string str, const std::string &replace, const std::string &with)
{
  size_t pos;
  while ((pos = str.find(replace)) != std::string::npos)
    str = str.replace(pos, replace.length(), with.c_str(), with.length());

  return str;
}

// plaform-specific escape path function
// on windows: just puts the path in quotes
// everything else: assume bash and escape special characters with backslashes
std::string escapePath(const boost::filesystem::path &path)
{
#ifdef WIN32
  // windows escapes stuff by just putting everything in quotes
  return '"' + fs::path(path).make_preferred().string() + '"';
#else
  // a quick and dirty way to insert a backslash before most characters that would mess up a bash path
  std::string pathStr = path.string();

  const char *invalidChars = " '\"\\!$^&*(){}[]?;<>";
  for (unsigned int i = 0; i < (unsigned int)pathStr.length(); i++)
  {
    char c;
    unsigned int charNum = 0;
    do
    {
      c = invalidChars[charNum];
      if (pathStr[i] == c)
      {
        pathStr.insert(i, "\\");
        i++;
        break;
      }
      charNum++;
    } while (c != '\0');
  }

  return pathStr;
#endif
}

void SystemData::launchGame(Window* window, FileData* game, const std::string& netplay, const std::string& core, const std::string& ip, const std::string& port)
{
  (void)core;

  LOG(LogInfo) << "Attempting to launch game...";

  VideoEngine::This().StopVideo();
  AudioManager::getInstance()->deinit();
  VolumeControl::getInstance()->deinit();

  std::string controlersConfig = InputManager::Instance().GenerateConfiggenConfiguration();
  LOG(LogInfo) << "Controllers config : " << controlersConfig;
  window->deinit();


  std::string command = mLaunchCommand;

  const std::string rom = escapePath(game->getPath());
  const std::string basename = game->getPath().stem().string();
  const std::string rom_raw = fs::path(game->getPath()).make_preferred().string();

  command = strreplace(command, "%ROM%", rom);
  command = strreplace(command, "%CONTROLLERSCONFIG%", controlersConfig);
  command = strreplace(command, "%SYSTEM%", game->getSystem()->getName());
  command = strreplace(command, "%BASENAME%", basename);
  command = strreplace(command, "%ROM_RAW%", rom_raw);
  command = strreplace(command, "%EMULATOR%", game->Metadata().Emulator());
  command = strreplace(command, "%CORE%", game->Metadata().Core());
  command = strreplace(command, "%RATIO%", game->Metadata().RatioAsString());

	if (netplay == "client")
	{
		command = strreplace(command, "%NETPLAY%", "-netplay client -netplay_port " + port + " -netplay_ip " + ip);
	}
	else if (netplay == "host")
	{
	    std::string hash = game->Metadata().RomCrc32AsString();
	    std::string hashcmd;
	    if (!hash.empty())
	    {
	        hashcmd = " -hash " + hash;
	    }
		command = strreplace(command, "%NETPLAY%", "-netplay host -netplay_port " + RecalboxConf::getInstance()->get("global.netplay.port") + hashcmd);
	}
	else
	{
		command = strreplace(command, "%NETPLAY%", "");
	}


	LOG(LogInfo) << "	" << command;
	std::cout << "==============================================\n";
  RecalboxSystem::NotifyGame(*game, true, false);
	int exitCode = runSystemCommand(command);
  RecalboxSystem::NotifyGame(*game, false, false);
	std::cout << "==============================================\n";

  if( exitCode != 0)
  {
    LOG(LogWarning) << "...launch terminated with nonzero exit code " << exitCode << "!";
  }

  // Reinit
  window->init();
  VolumeControl::getInstance()->init();
  AudioManager::getInstance()->resumeMusic();
  window->normalizeNextUpdate();

  //update number of times the game has been launched
  game->Metadata().IncPlaycount();

  //update last played time
  game->Metadata().SetLastplayedNow();
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

bool SystemData::demoLaunchGame(FileData* game, int duration, int infoscreenduration, const std::string& controlersConfig)
{

  std::string command = mLaunchCommand;

  const std::string rom = escapePath(game->getPath());
  const std::string basename = game->getPath().stem().string();
  const std::string rom_raw = fs::path(game->getPath()).make_preferred().string();

  command = strreplace(command, "%ROM%", rom);
  command = strreplace(command, "%CONTROLLERSCONFIG%", controlersConfig);
  command = strreplace(command, "%SYSTEM%", game->getSystem()->getName());
  command = strreplace(command, "%BASENAME%", basename);
  command = strreplace(command, "%ROM_RAW%", rom_raw);
  command = strreplace(command, "%EMULATOR%", game->Metadata().Emulator());
  command = strreplace(command, "%CORE%", game->Metadata().Core());
  command = strreplace(command, "%RATIO%", game->Metadata().RatioAsString());
  command = strreplace(command, "%NETPLAY%", "");

  // Add demo stuff
  command += " -demo 1";
  command += " -demoduration ";
  command += std::to_string(duration);
  command += " -demoinfoduration ";
  command += std::to_string(infoscreenduration);

  LOG(LogInfo) << "Demo command: " << command;
  RecalboxSystem::NotifyGame(*game, true, true);
  int exitCode = runSystemCommand(command);
  RecalboxSystem::NotifyGame(*game, false, false);
  LOG(LogInfo) << "Demo exit code :	" << exitCode;


  // Configgen returns an exitcode 0x33 when the user interact with any pad/mouse
  // this exitcode returns here byte-swapped or shifted. Need further investigation
  if( exitCode == 0x3300)
  {
    LOG(LogInfo) << "Exiting demo upon user request";
    return true;
  }

  if( exitCode != 0)
  {
    LOG(LogWarning) << "...launch terminated with nonzero exit code " << exitCode << "!";
  }

  return false;
}

void SystemData::populateFolder(FolderData *folder, FileData::StringMap& doppelgangerWatcher)
{
  LOG(LogInfo) << folder->getSystem()->getFullName() << ": Searching games/roms in " << folder->getPath().generic_string() << "...";

  try
  {
    folder->populateRecursiveFolder(mSearchExtensions, this, doppelgangerWatcher);
  }
  catch(std::exception& ex)
  {
    LOG(LogError) << "Reading folder \"" << folder->getPath().generic_string() << "\" has raised an error!";
    LOG(LogError) << "Exception: " << ex.what();
  }
}

std::string SystemData::getGamelistPath(bool forWrite) const
{
  fs::path filePath;

  // If we have a gamelist in the rom directory, we use it
  filePath = mRootFolder.getPath() / "gamelist.xml";
  if (fs::exists(filePath))
    return filePath.generic_string();

  // else we try to create it
  if (forWrite)
  { // make sure the directory exists if we're going to write to it, or crashes will happen
    if (fs::exists(filePath.parent_path()) || fs::create_directories(filePath.parent_path()))
    {
      return filePath.generic_string();
    }
  }
  // Unable to get or create directory in roms, fallback on ~
  filePath = RootFolders::DataRootFolder + "/system/.emulationstation/gamelists/" + mName + "/gamelist.xml";
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
  catch (ThemeException &e)
  {
        LOG(LogError) << e.what();
	    mThemeFolder = "default";
	    mTheme->loadFile(getThemeFolder(), path);
  }
}

std::vector<std::string> SystemData::getCores(const std::string &emulatorName)
{
  std::vector<std::string> list;
  for (auto& emulator : mEmulators)
  {
    if (emulatorName == emulator.first)
    {
      list = *emulator.second;
      break;
    }
  }
  return list;
}
