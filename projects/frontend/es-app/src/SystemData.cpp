#include "SystemData.h"
#include "Gamelist.h"
#include "AudioManager.h"
#include "VolumeControl.h"
#include "Log.h"
#include "Settings.h"
#include "Util.h"
#include <boost/thread.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/make_shared.hpp>
#include <RecalboxConf.h>
#include <RootFolders.h>
#include <recalbox/RecalboxSystem.h>
#include <VideoEngine.h>

std::vector<SystemData *> SystemData::sSystemVector;

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

SystemData* SystemData::CreateRegularSystem(const std::string& name, const std::string& fullName, const std::string& startPath,
                                            const std::string& filteredExtensions, const std::string& command,
                                            const std::vector<PlatformIds::PlatformId>& platformIds, const std::string& themeFolder,
                                            const Tree& emuNodes)
{
  const std::string defaultRomsPath = getExpandedPath(Settings::getInstance()->getString("DefaultRomsPath"));
  std::string realPath = defaultRomsPath.empty() ? startPath : fs::absolute(startPath, defaultRomsPath).generic_string();

  SystemData* result = new SystemData(name, fullName, realPath, filteredExtensions, command, platformIds, themeFolder, &emuNodes, true);

  // Avoid files being added more than once even through symlinks
  {
    FileData::StringMap doppelgangerWatcher;

    // Populate items from disk
    if (RecalboxConf::getInstance()->get("emulationstation.gamelistonly") != "1")
      result->populateFolder(&(result->mRootFolder), doppelgangerWatcher);
    // Populate items from gamelist.xml
    if (!Settings::getInstance()->getBool("IgnoreGamelist"))
      parseGamelist(result, doppelgangerWatcher);
  } // Let the doppelgangerWatcher to free its memory ASAP

  result->loadTheme();

  return result;
}

SystemData* SystemData::CreateFavoriteSystem(const std::string& name, const std::string& fullName, const std::string& command,
                                             const std::string& themeFolder, const std::vector<SystemData*>& systems)
{
  std::vector<PlatformIds::PlatformId> platformIds;
  platformIds.push_back(PlatformIds::PlatformId::PLATFORM_IGNORE);

  SystemData* result = new SystemData(name, fullName, "", "", command, platformIds, themeFolder, nullptr, false);
  result->mIsFavorite = true;

  for (auto system : systems)
  {
    FileData::List favs = system->getFavorites();
    if (!favs.empty())
    {
      LOG(LogWarning) << " Get " << favs.size() << " favorites for " << system->getName() << "!";
      for (auto favorite : favs)
        result->mRootFolder.addChild(favorite, false);
    }
  }

  result->loadTheme();

  return result;
}

SystemData::SystemData(const std::string &name, const std::string &fullName, const std::string &startPath,
                       const std::string& filteredExtensions, const std::string &command,
                       const std::vector<PlatformIds::PlatformId> &platformIds, const std::string &themeFolder,
                       const Tree* emuNodes, bool childOwnership)
  : mName(name),
    mFullName(fullName),
    mStartPath(getExpandedPath(startPath)),
    mSearchExtensions(filteredExtensions),
    mLaunchCommand(command),
    mPlatformIds(platformIds),
    mThemeFolder(themeFolder),
    mIsFavorite(false),
    mSortId(RecalboxConf::getInstance()->getUInt(mName + ".sort")),
    mRootFolder(childOwnership, mStartPath, this),
    mEmulators()
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
  //save changed game data back to xml
  if (!Settings::getInstance()->getBool("IgnoreGamelist") && !mIsFavorite)
    updateGamelist(this);

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
  for (unsigned int i = 0; i < pathStr.length(); i++)
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

  std::string controlersConfig = InputManager::getInstance()->configureEmulators();
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
  RecalboxSystem::getInstance()->NotifyGame(*game, true, false);
	int exitCode = runSystemCommand(command);
  RecalboxSystem::getInstance()->NotifyGame(*game, false, false);
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

  std::string controlersConfig = InputManager::getInstance()->configureEmulators();
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
  RecalboxSystem::getInstance()->NotifyGame(*game, true, true);
  int exitCode = runSystemCommand(command);
  RecalboxSystem::getInstance()->NotifyGame(*game, false, false);
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

std::vector<std::string> readList(const std::string &str, const char *delims = " \t\r\n,")
{
  std::vector<std::string> ret;

  size_t prevOff = str.find_first_not_of(delims, 0);
  size_t off = str.find_first_of(delims, prevOff);
  while (off != std::string::npos || prevOff != std::string::npos)
  {
    ret.push_back(str.substr(prevOff, off - prevOff));

    prevOff = str.find_first_not_of(delims, off);
    off = str.find_first_of(delims, prevOff);
  }

  return ret;
}

SystemData *createSystem(const SystemData::Tree &system)
{
  std::string name, fullname, path, cmd, themeFolder;
  try
  {
    name = system.get("name", "");
    fullname = system.get("fullname", "");
    path = system.get("path", "");

//#ifdef DEBUG
//    strFindAndReplace(path, "roms", "romstest");
//#endif

    // convert extensions list from a string into a vector of strings
    std::string extensions = system.get("extension", "");

    cmd = system.get("command", "");

    // platform id list
    const std::string platformList = system.get("platform", "");
    std::vector<std::string> platformStrs = readList(platformList);
    std::vector<PlatformIds::PlatformId> platformIds;
    for (const auto &it : platformStrs)
    {
      const char *str = it.c_str();
      PlatformIds::PlatformId platformId = PlatformIds::getPlatformId(str);

      if (platformId == PlatformIds::PlatformId::PLATFORM_IGNORE)
      {
        // when platform is ignore, do not allow other platforms
        platformIds.clear();
        platformIds.push_back(platformId);
        break;
      }

      // if there appears to be an actual platform ID supplied but it didn't match the list, warn
      if ((str != nullptr) && (str[0] != '\0') && (platformId == PlatformIds::PlatformId::PLATFORM_UNKNOWN))
      {
        LOG(LogWarning) << "  Unknown platform for system \"" << name << "\" (platform \"" << str << "\" from list \"" << platformList << "\")";
      }
      else if (platformId != PlatformIds::PlatformId::PLATFORM_UNKNOWN)
        platformIds.push_back(platformId);
    }

    // theme folder
    themeFolder = system.get("theme", "");

    //validate
    if (name.empty() || path.empty() || extensions.empty() || cmd.empty())
    {
      LOG(LogError) << "System \"" << name << "\" is missing name, path, extension, or command!";
      return nullptr;
    }

    //convert path to generic directory seperators
    boost::filesystem::path genericPath(path);
    path = genericPath.generic_string();

    SystemData* newSys = SystemData::CreateRegularSystem(name, fullname,
                                                         path, extensions,
                                                         cmd, platformIds,
                                                         themeFolder,
                                                         system.get_child("emulators"));
    if (newSys->getRootFolder()->countAll(false) == 0)
    {
      LOG(LogWarning) << "System \"" << name << "\" has no games! Ignoring it.";
      delete newSys;
      return nullptr;
    }
    else
    {
      LOG(LogWarning) << "Adding \"" << name << "\" in system list.";
      return newSys;
    }
  }
  catch(std::exception& ex)
  {
    LOG(LogError) << "System \"" << fullname << "\" has raised an error. Ignored.";
    LOG(LogError) << "Exception: " << ex.what();
  }
  return nullptr;
}

bool SystemData::loadSystemNodes(XmlNodeCollisionMap &collisionMap, XmlNodeList &nodeStore, const Tree &document)
{
  deleteSystems();
  bool result = false;
  for (const auto& systemNode : document.get_child("systemList"))
  {
    // At least one node found
    result = true;

    // Get system
    const Tree &system = systemNode.second;

    // Composite key: fullname + platform
    std::string key = system.get("fullname", "");
    key += system.get("platform", "");
    LOG(LogDebug) << "Key: " << key;

    // Already exist in the store ?
    int index = (collisionMap.find(key) != collisionMap.end()) ? collisionMap[key] : -1;

    // If the system does not exists, add it to the end of list.
    // Replace the existing entry otherwise.
    // This way, we keep the original ordering of first-in entries
    if (index < 0)
    {
      collisionMap[key] = (int) nodeStore.size();
      nodeStore.push_back(system);
    } else nodeStore[index] = system;
  }

  if (!result)
  {
    LOG(LogError) << "missing or empty <systemList> tag!";
  }

  return result;
}

bool SystemData::loadXmlFile(Tree &document, const std::string &filePath)
{
  try
  {
    pt::read_xml(filePath, document);
  }
  catch (std::exception &e)
  {
    LOG(LogError) << "Could not parse " << filePath << " file!";
    LOG(LogError) << e.what();
    return false;
  }
  return true;
}

bool SystemData::loadSystemList(Tree &document, XmlNodeCollisionMap &collisionMap, XmlNodeList &nodeStore, const std::string &filePath)
{
  // Load user configuration
  if (!fs::exists(filePath))
  {
    LOG(LogError) << filePath << " file does not exist!";
    return false;
  }

  LOG(LogInfo) << "Loading system config files " << filePath << "...";
  if (!loadXmlFile(document, filePath)) return false;

  bool result = loadSystemNodes(collisionMap, nodeStore, document);
  if (!result)
  {
    LOG(LogWarning) << filePath << " has no systems or systemList nodes";
  }

  return result;
}

//creates systems from information located in a config file
bool SystemData::loadConfig()
{
  deleteSystems();

  // System store
  XmlNodeCollisionMap systemMap;    // System key to vector index
  XmlNodeList systemList;  // Sorted storage, keeping original system node ordering
  // XML Documents - declared here to keep node memory allocated
  pt::ptree templateDocument;
  pt::ptree userDocument;

  // Load user systems
  bool userValid = loadSystemList(userDocument, systemMap, systemList, getUserConfigurationAbsolutePath());
  // Load template systems
  bool templateValid = loadSystemList(templateDocument, systemMap, systemList, getTemplateConfigurationAbsolutePath());

  // Is there at least
  if (!templateValid && !userValid)
  {
    LOG(LogError) << "No es_systems.cfg file available!";
    writeExampleConfig(getUserConfigurationAbsolutePath());
    return false;
  }

  // Dont try to run multi-threading loading on old mono-core platforms
  // Would be waste of CPU cycles
  RaspberryGeneration hardware = getRaspberryVersion();
  DateTime start;
  switch(hardware)
  {
    case RaspberryGeneration::Pi0:
    case RaspberryGeneration::Pi1:
    {
      for (const Tree& system : systemList)
      {
        DateTime startSystem;
        SystemData* sys = createSystem(system);
        DateTime stopSystem;

        if (sys != nullptr)
        {
          LOG(LogDebug) << sys->getName() << " load time: " << (stopSystem - startSystem).ToStringFormat("%ss.%fff");
          sSystemVector.push_back(sys);
        }
      }
      break;
    }
    case RaspberryGeneration::NotRaspberry:
    case RaspberryGeneration::Pi2:
    case RaspberryGeneration::Pi3:
    case RaspberryGeneration::Pi3plus:
    case RaspberryGeneration::Pi4:
    case RaspberryGeneration::NotYetKnown:
    case RaspberryGeneration::UndetectedYet:;
    default:
    {
      // THE CREATION OF EACH SYSTEM
      boost::asio::io_service ioService;
      boost::thread_group threadpool;
      boost::asio::io_service::work work(ioService);
      for (int i = 4; --i >= 0;)
        threadpool.create_thread(boost::bind(&boost::asio::io_service::run, &ioService));

      // Iterate over the map using Iterator till end.
      std::vector <boost::unique_future<SystemData*>> pending_data;
      for (const Tree& system : systemList)
      {
        LOG(LogInfo) << "creating thread for system " << system.get("name", "???");
        typedef boost::packaged_task<SystemData*> task_t;
        boost::shared_ptr<task_t> task = boost::make_shared<task_t>(boost::bind(&createSystem, system));
        boost::unique_future<SystemData*> fut = task->get_future();
        pending_data.push_back(std::move(fut));
        ioService.post(boost::bind(&task_t::operator(), task));
      }
      boost::wait_for_all(pending_data.begin(), pending_data.end());

      for (auto& pending : pending_data)
      {
        SystemData* result = pending.get();
        if (result != nullptr)
          sSystemVector.push_back(result);
      }
      ioService.stop();
      threadpool.join_all();
      break;
    }
  }
  DateTime stop;
  LOG(LogInfo) << "Gamelist load time: " << (stop-start).ToStringFormat("%ss.%fff");

  if (sSystemVector.empty()) return true;
  // Favorite system
  for (const Tree &system : systemList)
  {
    std::string name = system.get("name", "");
    std::string fullname = system.get("fullname", "");
    std::string cmd = system.get("command", "");
    std::string themeFolder = system.get("theme", "");

    if (name == "favorites")
    {
      LOG(LogInfo) << "creating favorite system";
      SystemData *newSys = SystemData::CreateFavoriteSystem("favorites", fullname, cmd, themeFolder, sSystemVector);
      sSystemVector.push_back(newSys);
    }
  }

  return true;
}


void SystemData::writeExampleConfig(const std::string &path)
{
  std::ofstream file(path.c_str());

  file << "<!-- This is the EmulationStation Systems configuration file.\n"
          "All systems must be contained within the <systemList> tag.-->\n"
          "\n"
          "<systemList>\n"
          "    <!-- Here's an example system to get you started. -->\n"
          "    <system>\n"
          "\n"
          "        <!-- A short name, used internally. Traditionally lower-case. -->\n"
          "        <name>nes</name>\n"
          "\n"
          "        <!-- A \"pretty\" name, displayed in menus and such. -->\n"
          "        <fullname>Nintendo Entertainment System</fullname>\n"
          "\n"
          "        <!-- The path to start searching for ROMs in. '~' will be expanded to $HOME on Linux or %HOMEPATH% on Windows. -->\n"
          "        <path>/recalbox/share/roms/nes</path>\n"
          "\n"
          "        <!-- A list of extensions to search for, delimited by any of the whitespace characters (\", \\r\\n\\t\").\n"
          "        You MUST include the period at the start of the extension! It's also case sensitive. -->\n"
          "        <extension>.nes .NES</extension>\n"
          "\n"
          "        <!-- The shell command executed when a game is selected. A few special tags are replaced if found in a command:\n"
          "        %ROM% is replaced by a bash-special-character-escaped absolute path to the ROM.\n"
          "        %BASENAME% is replaced by the \"base\" name of the ROM.  For example, \"/foo/bar.rom\" would have a basename of \"bar\". Useful for MAME.\n"
          "        %ROM_RAW% is the raw, unescaped path to the ROM. -->\n"
          "        <command>retroarch -L ~/cores/libretro-fceumm.so %ROM%</command>\n"
          "\n"
          "        <!-- The platform to use when scraping. You can see the full list of accepted platforms in src/PlatformIds.cpp.\n"
          "        It's case sensitive, but everything is lowercase. This tag is optional.\n"
          "        You can use multiple platforms too, delimited with any of the whitespace characters (\", \\r\\n\\t\"), eg: \"genesis, megadrive\" -->\n"
          "        <platform>nes</platform>\n"
          "\n"
          "        <!-- The theme to load from the current theme set.  See THEMES.md for more information.\n"
          "        This tag is optional. If not set, it will default to the value of <name>. -->\n"
          "        <theme>nes</theme>\n"
          "    </system>\n"
          "</systemList>\n";

  file.close();

  LOG(LogError) << "Example config written!  Go read it at \"" << path << "\"!";
}

bool deleteSystem(SystemData *system)
{
  delete system;
  return true;
}

void SystemData::deleteSystems()
{
  if (!sSystemVector.empty())
  {
    // THE DELETION OF EACH SYSTEM
    boost::asio::io_service ioService;
    boost::thread_group threadpool;
    boost::asio::io_service::work work(ioService);
    //std::vector<boost::thread *> threads;
    for (int i = 0; i < 4; i++)
    {
      threadpool.create_thread(
        boost::bind(&boost::asio::io_service::run, &ioService)
      );
    }

    std::vector<boost::unique_future<bool>> pending_data;
    for (SystemData *system : sSystemVector)
    {
      typedef boost::packaged_task<bool> task_t;
      boost::shared_ptr<task_t> task = boost::make_shared<task_t>(
        boost::bind(&deleteSystem, system));
      boost::unique_future<bool> fut = task->get_future();
      pending_data.push_back(std::move(fut));
      ioService.post(boost::bind(&task_t::operator(), task));
    }

    boost::wait_for_all(pending_data.begin(), pending_data.end());

    ioService.stop();
    threadpool.join_all();
    sSystemVector.clear();
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

std::map<std::string, std::vector<std::string> *> *SystemData::getEmulators()
{
  return &mEmulators;
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


SystemData *SystemData::getSystem(std::string &name)
{
  for (auto system: sSystemVector)
  {
    if (system->mName == name)
    {
      return system;
    }
  }
  return nullptr;
}

SystemData *SystemData::getFavoriteSystem()
{
  for (int i = (int) sSystemVector.size(); --i >= 0;)
    if (sSystemVector[i]->isFavorite())
      return sSystemVector[i];
  return nullptr;
}

int SystemData::getSystemIndex(const std::string &name)
{
  for (int i = (int) sSystemVector.size(); --i >= 0;)
    if (sSystemVector[i]->mName == name)
      return i;
  return -1;
}

SystemData* SystemData::getFirstSystemWithGame()
{
  for (auto &system : sSystemVector)
    if (system->hasGame())
      return system;

  return nullptr;
}