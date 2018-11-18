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

std::vector<SystemData *> SystemData::sSystemVector;

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

SystemData::SystemData(const std::string &name, const std::string &fullName, const std::string &startPath,
                       const std::vector<std::string> &extensions, const std::string &command,
                       const std::vector<PlatformIds::PlatformId> &platformIds, const std::string &themeFolder,
                       std::map<std::string, std::vector<std::string> *> *emulators)
  : mName(name),
    mFullName(fullName),
    mStartPath(getExpandedPath(startPath)),
    mLaunchCommand(command),
    mThemeFolder(themeFolder),
    mEmulators(emulators),
    mSearchExtensions(extensions),
    mPlatformIds(platformIds)
{
  mSortId = RecalboxConf::getInstance()->getUInt(mName + ".sort");
  // make it absolute if needed
  {
    const std::string defaultRomsPath = getExpandedPath(Settings::getInstance()->getString("DefaultRomsPath"));

    if (!defaultRomsPath.empty())
    {
      mStartPath = fs::absolute(mStartPath, defaultRomsPath).generic_string();
    }
  }

  mRootFolder = new FileData(FOLDER, mStartPath, this);
  mRootFolder->Metadata().SetName(mFullName);

  if (RecalboxConf::getInstance()->get("emulationstation.gamelistonly") != "1")
    populateFolder(mRootFolder);

  if (!Settings::getInstance()->getBool("IgnoreGamelist"))
    parseGamelist(this);

  mIsFavorite = false;
  loadTheme();
}

SystemData::SystemData(const std::string &name, const std::string &fullName, const std::string &command,
                       const std::string &themeFolder, std::vector<SystemData *> *systems)
  : mName(name),
    mFullName(fullName),
    mStartPath(),
    mLaunchCommand(command),
    mThemeFolder(themeFolder),
    mEmulators(nullptr)
{
  mSortId = RecalboxConf::getInstance()->getUInt(mName + ".sort");
  mRootFolder = new FileData(FOLDER, mStartPath, this);
  mRootFolder->Metadata().SetName(mFullName);

  for (auto system : *systems)
  {
    std::vector<FileData *> favorites = system->getFavorites();
    for (auto favorite : favorites)
      mRootFolder->addAlreadyExistingChild(favorite);
  }

  mIsFavorite = true;
  mPlatformIds.push_back(PlatformIds::PLATFORM_IGNORE);
  loadTheme();
}

SystemData::~SystemData()
{
  //save changed game data back to xml
  if (!Settings::getInstance()->getBool("IgnoreGamelist"))
  {
    updateGamelist(this);
  }
  if (mRootFolder)
  {
    auto children = mRootFolder->getChildren();
    for (auto it : children)
    {

      mRootFolder->removeChild(it);
      delete it;
    }
    delete mRootFolder;
  }
  if (mEmulators)
  {
    for (const auto &itr : *mEmulators)
    {
      delete (itr.second);
    }
    mEmulators->clear();

    delete mEmulators;
  }
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

void SystemData::launchGame(Window* window, FileData* game, std::string netplay, std::string core, std::string ip, std::string port)
{
  LOG(LogInfo) << "Attempting to launch game...";


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
  command = strreplace(command, "%RATIO%", game->Metadata().RatingAsString());

	/*command = strreplace(command, "%ROM%", rom);
	command = strreplace(command, "%CONTROLLERSCONFIG%", controlersConfig);
	command = strreplace(command, "%SYSTEM%", game->Metadata().get("system"));
	command = strreplace(command, "%BASENAME%", basename);
	command = strreplace(command, "%ROM_RAW%", rom_raw);
	if (core != "")
	{
		command = strreplace(command, "%EMULATOR%", "libretro");
		command = strreplace(command, "%CORE%", core);
	}
	else
	{
		command = strreplace(command, "%EMULATOR%", game->Metadata().get("emulator"));
		command = strreplace(command, "%CORE%", game->Metadata().get("core"));
	}
	command = strreplace(command, "%RATIO%", game->Metadata().get("ratio"));*/

	if (netplay == "client")
	{
		command = strreplace(command, "%NETPLAY%", "-netplay client -netplay_port " + port + " -netplay_ip " + ip);
	}
	else if (netplay == "host")
	{
	    std::string hash = game->Metadata().RomCrc32AsString();
	    std::string hashcmd = "";
	    if (hash != "")
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
	int exitCode = runSystemCommand(command);
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
  int timesPlayed = game->Metadata().PlayCount();

  //update last played time
  game->Metadata().SetLastplayedNow();
}

void SystemData::populateFolder(FileData *folder)
{
  FileData::populateRecursiveFolder(folder, mSearchExtensions, this);
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
  //PlatformIds::PlatformId platformId = PlatformIds::PLATFORM_UNKNOWN;

  name = system.get("name", "");
  fullname = system.get("fullname", "");
  path = system.get("path", "");

  // convert extensions list from a string into a vector of strings
  std::vector<std::string> extensions = readList(system.get("extension", ""));

  cmd = system.get("command", "");

  // platform id list
  const std::string platformList = system.get("platform", "");
  std::vector<std::string> platformStrs = readList(platformList);
  std::vector<PlatformIds::PlatformId> platformIds;
  for (const auto &it : platformStrs)
  {
    const char *str = it.c_str();
    PlatformIds::PlatformId platformId = PlatformIds::getPlatformId(str);

    if (platformId == PlatformIds::PLATFORM_IGNORE)
    {
      // when platform is ignore, do not allow other platforms
      platformIds.clear();
      platformIds.push_back(platformId);
      break;
    }

    // if there appears to be an actual platform ID supplied but it didn't match the list, warn
    if (str != nullptr && str[0] != '\0' && platformId == PlatformIds::PLATFORM_UNKNOWN)
      LOG(LogWarning) << "  Unknown platform for system \"" << name << "\" (platform \"" << str << "\" from list \"" << platformList << "\")";
    else if (platformId != PlatformIds::PLATFORM_UNKNOWN)
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

  // emulators and cores
  auto systemEmulators = new std::map<std::string, std::vector<std::string> *>();
  //SystemData::Tree emulatorsNode = system.get("emulators", "");
  for (const SystemData::TreeNode &emuNode : system.get_child("emulators"))
  {
    const std::string &emulatorName = emuNode.second.get_child("<xmlattr>").get("name", "");
    (*systemEmulators)[emulatorName] = new std::vector<std::string>();
    for (const SystemData::TreeNode &coreNode : emuNode.second.get_child("cores"))
    {
      const std::string &corename = coreNode.second.data();
      (*systemEmulators)[emulatorName]->push_back(corename);
    }
  }

  auto newSys = new SystemData(name,
                               fullname,
                               path, extensions,
                               cmd, platformIds,
                               themeFolder,
                               systemEmulators);
  if (newSys->getRootFolder()->getDisplayableRecursive(GAME | FOLDER).empty())
  {
    LOG(LogWarning) << "System \"" << name << "\" has no games! Ignoring it.";
    delete newSys;
    return nullptr;
  } else
  {
    LOG(LogWarning) << "Adding \"" << name << "\" in system list.";
    return newSys;
  }
}

bool SystemData::loadSystemNodes(XmlNodeCollisionMap &collisionMap, XmlNodeList &nodeStore, const Tree &document)
{
  deleteSystems();
  bool result = false;
  for (const TreeNode &systemNode : document.get_child("systemList"))
  {
    // At least one node found
    result = true;

    // Get system
    const Tree &system = systemNode.second;

    // Composite key: short name + platform
    std::string key = system.get("name", "");
    key += system.get("platform", "");
    LOG(LogWarning) << "Key: " << key;

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
    LOG(LogError) << "missing or empty <systemList> tag!";

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
  if (!result) LOG(LogWarning) << filePath << " has no systems or systemList nodes";

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
  bool userValid = loadSystemList(userDocument, systemMap, systemList, getUserConfigPath());
  // Load template systems
  bool templateValid = loadSystemList(templateDocument, systemMap, systemList, getTemplateConfigPath());

  // Is there at least
  if (!templateValid && !userValid)
  {
    LOG(LogError) << "No es_systems.cfg file available!";
    writeExampleConfig(getUserConfigPath());
    return false;
  }

  // THE CREATION OF EACH SYSTEM
  boost::asio::io_service ioService;
  boost::thread_group threadpool;
  boost::asio::io_service::work work(ioService);
  for (int i = 4; --i >= 0;)
    threadpool.create_thread(boost::bind(&boost::asio::io_service::run, &ioService));

  // Iterate over the map using Iterator till end.
  std::vector<boost::unique_future<SystemData *>> pending_data;
  for (const Tree &system : systemList)
  {
    LOG(LogInfo) << "creating thread for system " << system.get("name", "???");
    typedef boost::packaged_task<SystemData *> task_t;
    boost::shared_ptr<task_t> task = boost::make_shared<task_t>(
      boost::bind(&createSystem, system));
    boost::unique_future<SystemData *> fut = task->get_future();
    pending_data.push_back(std::move(fut));
    ioService.post(boost::bind(&task_t::operator(), task));
  }
  boost::wait_for_all(pending_data.begin(), pending_data.end());

  for (auto &pending : pending_data)
  {
    SystemData *result = pending.get();
    if (result != nullptr)
      sSystemVector.push_back(result);
  }
  ioService.stop();
  threadpool.join_all();

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
      SystemData *newSys = new SystemData("favorites", fullname, cmd, themeFolder, &sSystemVector);
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
          "        <path>~/roms/nes</path>\n"
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

std::string SystemData::getUserConfigPath()
{
  fs::path path = getHomePath() + mUserConfigurationRelativePath;
  return path.generic_string();
}

std::string SystemData::getTemplateConfigPath()
{
  return mTemplateConfigurationAbsolutePath;
}

std::string SystemData::getGamelistPath(bool forWrite) const
{
  fs::path filePath;

  // If we have a gamelist in the rom directory, we use it
  filePath = mRootFolder->getPath() / "gamelist.xml";
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
  filePath = getHomePath() + "/.emulationstation/gamelists/" + mName + "/gamelist.xml";
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
  fs::path localThemePath = mRootFolder->getPath() / "theme.xml";
  if (fs::exists(localThemePath))
    return localThemePath.generic_string();

  // not in game folder, try system theme in theme sets
  localThemePath = ThemeData::getThemeFromCurrentSet(mThemeFolder);

  if (fs::exists(localThemePath))
    return localThemePath.generic_string();

  // not system theme, try default system theme in theme set
  localThemePath = localThemePath.parent_path().parent_path() / "theme.xml";

  return localThemePath.generic_string();
}

unsigned int SystemData::getGameCount() const
{
  return (unsigned int) mRootFolder->getFilesRecursive(GAME).size();
}

unsigned int SystemData::getFavoritesCount() const
{
  return (unsigned int) mRootFolder->getFavoritesRecursive(GAME).size();
}

unsigned int SystemData::getHiddenCount() const
{
  return (unsigned int) mRootFolder->getHiddenRecursive(GAME).size();
}

void SystemData::loadTheme()
{
  mTheme = std::make_shared<ThemeData>();

  std::string path = getThemePath();

  if (!fs::exists(path)) // no theme available for this platform
    return;

  try
  {
    //std::cout << "creating theme for: " << getName() << " / " << getFullName() << std::endl;
    mTheme->loadFile(getThemeFolder(), path);
    mHasFavorites = mTheme->getHasFavoritesInTheme();
  }
  catch (ThemeException &e)
  {
    LOG(LogError) << e.what();
    mTheme = std::make_shared<ThemeData>(); // reset to empty
  }
}

std::map<std::string, std::vector<std::string> *> *SystemData::getEmulators()
{
  return mEmulators;
}

std::vector<std::string> SystemData::getCores(const std::string &emulatorName)
{
  std::vector<std::string> list;
  for (auto emulator : *mEmulators)
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