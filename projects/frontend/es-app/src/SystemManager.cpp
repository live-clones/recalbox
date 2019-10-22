//
// Created by bkg2k on 15/10/2019.
//

#include "SystemManager.h"
#include "Gamelist.h"
#include <Log.h>
#include <Settings.h>
#include <RecalboxConf.h>
#include <utils/os/fs/FileSystemUtil.h>
#include <boost/thread.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/make_shared.hpp>
#include <platform.h>
#include <utils/StringUtil.h>
#include <utils/os/fs/StringMapFile.h>

namespace fs = boost::filesystem;
namespace pt = boost::property_tree;

SystemData* SystemManager::CreateRegularSystem(const std::string& name, const std::string& fullName, const std::string& startPath,
                                            const std::string& filteredExtensions, const std::string& command,
                                            const std::vector<PlatformIds::PlatformId>& platformIds, const std::string& themeFolder,
                                            const Tree& emuNodes)
{
  const std::string defaultRomsPath = FileSystemUtil::getCanonicalPath(Settings::getInstance()->getString("DefaultRomsPath"));
  std::string realPath = defaultRomsPath.empty() ? startPath : fs::absolute(startPath, defaultRomsPath).generic_string();

  SystemData* result = new SystemData(name, fullName, realPath, filteredExtensions, command, platformIds, themeFolder, &emuNodes, true, false);

  // Avoid files being added more than once even through symlinks
  {
    FileData::StringMap doppelgangerWatcher;

    LOG(LogInfo) << "Creating & populating system: " << fullName;

    // Populate items from disk
    if (RecalboxConf::getInstance()->get("emulationstation.gamelistonly") != "1")
      result->populateFolder(&(result->mRootFolder), doppelgangerWatcher);
    // Populate items from gamelist.xml
    if (!Settings::getInstance()->getBool("IgnoreGamelist"))
      parseGamelist(result, doppelgangerWatcher);
    // Overrides?
    FileData::List allFolders = result->getRootFolder()->getAllFolders();
    for(auto folder : allFolders)
      overrideFolderInformation(folder);
  } // Let the doppelgangerWatcher to free its memory ASAP

  result->loadTheme();

  return result;
}

SystemData* SystemManager::CreateFavoriteSystem(const std::string& name, const std::string& fullName,
                                             const std::string& themeFolder, const std::vector<SystemData*>& systems)
{
  std::vector<PlatformIds::PlatformId> platformIds;
  platformIds.push_back(PlatformIds::PlatformId::PLATFORM_IGNORE);

  SystemData* result = new SystemData(name, fullName, "", "", "", platformIds, themeFolder, nullptr, false, true);

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

SystemData* SystemManager::CreateMetaSystem(const std::string& name, const std::string& fullName,
                                         const std::string& themeFolder, const std::vector<SystemData*>& systems)
{
  std::vector<PlatformIds::PlatformId> platformIds;
  platformIds.push_back(PlatformIds::PlatformId::PLATFORM_IGNORE);

  SystemData* result = new SystemData(name, fullName, "", "", "", platformIds, themeFolder, nullptr, false, false);

  for (auto system : systems)
  {
    FileData::List all = system->getRootFolder()->getAllItems(true);
    if (!all.empty())
    {
      LOG(LogWarning) << " Send games from " << system->getName() << " into Arcade";
      for (auto fd : all)
        result->mRootFolder.addChild(fd, false);
    }
  }

  result->loadTheme();

  return result;
}

SystemData* SystemManager::ThreadPoolRunJob(Tree& system)
{
  std::string name, fullname, path, cmd, themeFolder;
  try
  {
    name = system.get("name", "");
    fullname = system.get("fullname", "");
    path = system.get("path", "");

    //#ifdef DEBUG
    //    path = StringUtil::replace(path, "roms", "romstest");
    //#endif

    // convert extensions list from a string into a vector of strings
    std::string extensions = system.get("extension", "");

    cmd = system.get("command", "");

    // platform id list
    const std::string platformList = system.get("platform", "");
    std::vector<std::string> platformStrs = StringUtil::commaStringToVector(platformList);
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

    SystemData* newSys = CreateRegularSystem(name, fullname,
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

bool SystemManager::loadSystemNodes(XmlNodeCollisionMap &collisionMap, XmlNodeList &nodeStore, const Tree &document)
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

bool SystemManager::loadXmlFile(Tree &document, const std::string &filePath)
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

bool SystemManager::loadSystemList(Tree &document, XmlNodeCollisionMap &collisionMap, XmlNodeList &nodeStore, const std::string &filePath)
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

bool SystemManager::AddFavoriteSystem(const XmlNodeList& systemList)
{
  // Favorite system
  if (!sSystemVector.empty())
    for (const Tree &system : systemList)
    {
      std::string name = system.get("name", "");
      std::string fullname = system.get("fullname", "");
      std::string themeFolder = system.get("theme", "");

      if (name == "favorites")
      {
        LOG(LogInfo) << "creating favorite system";
        SystemData *newSys = CreateFavoriteSystem("favorites", fullname, themeFolder, sSystemVector);
        sSystemVector.push_back(newSys);
      }
    }

  return true;
}

bool SystemManager::AddArcadeMetaSystem()
{
  if (RecalboxConf::Instance().getBool("global.arcade", false))
  {
    bool includeNeogeo = RecalboxConf::Instance().getBool("global.arcade.includeneogeo", true);
    // Lookup all non-empty arcade platforms
    for (SystemData* system: sSystemVector)
      if (system->getRootFolder()->hasGame())
        for(int i = system->getPlatformCount(); --i >= 0; )
          if ((system->getPlatformIds(i) == PlatformIds::PlatformId::ARCADE) ||
              (system->getPlatformIds(i) == PlatformIds::PlatformId::NEOGEO && includeNeogeo))
          {
            sHiddenSystemVector.push_back(system);
            break;
          }

    // Non empty?
    if (!sHiddenSystemVector.empty())
    {
      // Remove Hidden systems fron the visible ist
      bool hideOriginals = RecalboxConf::Instance().getBool("global.arcade.hideoriginals", true);
      if (hideOriginals)
        for (SystemData* hidden: sHiddenSystemVector)
        {
          auto it = std::find(sSystemVector.begin(), sSystemVector.end(), hidden);
          if (it != sSystemVector.end())
            sSystemVector.erase(it);
        }
      // Create meta-system
      SystemData* arcade = CreateMetaSystem("arcade", "Arcade", "arcade", sHiddenSystemVector);
      LOG(LogInfo) << "creating Arcade meta-system";
      int position = RecalboxConf::Instance().getInt("global.arcade.position", 0) % (int)sSystemVector.size();
      auto it = position >= 0 ? sSystemVector.begin() + position : sSystemVector.end() + (position + 1);
      sSystemVector.insert(it, arcade);
      // Hide originals?
      if (!hideOriginals)
        sHiddenSystemVector.clear();
    }
  }

  return !sHiddenSystemVector.empty();
}

//creates systems from information located in a config file
bool SystemManager::loadConfig()
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


  DateTime start;

  // Get weight store
  StringMapFile weights(sWeightFilePath);
  weights.Load();
  // Create automatic thread-pool
  ThreadPool<Tree, SystemData*> threadPool(this, "System-Loader", -2, false);
  // Push system to process
  for (const Tree& system : systemList)
  {
    // Get weight
    std::string key = system.get("path", "unknown");
    //key = StringUtil::replace(key, "roms", "romstest");
    int weight = weights.GetInt(key, 0);
    // Push weighted system
    threadPool.PushFeed(system, weight);
  }
  // Run the threadpool and automatically wait for all jobs to complete
  int count = threadPool.PendingJobs();
  threadPool.Run(false);
  // Push result
  sSystemVector.resize(count, nullptr);
  int index = 0;
  for(SystemData* result = nullptr; threadPool.PopResult(result, index); )
    sSystemVector[index] = result;
  // Shrink & update weights
  for(int i = count; --i >= 0; )
  {
    SystemData* system = sSystemVector[i];
    if (system == nullptr)
      sSystemVector.erase(sSystemVector.begin() + i);
    else
      weights.SetInt(system->mStartPath, system->getRootFolder()->countAll(true));
  }
  weights.Save();

  DateTime stop;
  LOG(LogInfo) << "Gamelist load time: " << std::to_string((stop-start).TotalMilliseconds());

  // Add special systems
  AddArcadeMetaSystem();
  AddFavoriteSystem(systemList);

  // Set *all* service vector
  for(SystemData* service : sSystemVector)       sAllSystemVector.push_back(service);
  for(SystemData* service : sHiddenSystemVector) sAllSystemVector.push_back(service);

  return true;
}

void SystemManager::writeExampleConfig(const std::string &path)
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

bool SystemManager::ThreadPoolRunJob(SystemData*& feed)
{
  // Save changed game data back to xml
  if (!Settings::getInstance()->getBool("IgnoreGamelist"))
    if (feed->getRootFolder()->hasChildrenOwnership())
      updateGamelist(feed);

  return true;
}

void SystemManager::deleteSystems()
{
  if (!sAllSystemVector.empty())
  {
    DateTime start;

    // Create automatic thread-pool
    ThreadPool<SystemData*, bool> threadPool(this, "System-Updater", -2, false);
    // Push system to process
    for (SystemData* system : sAllSystemVector)
      threadPool.PushFeed(system, 0);
    // Run the threadpool and automatically wait for all jobs to complete
    threadPool.Run(false);

    DateTime stop;
    LOG(LogInfo) << "Gamelist update time: " << std::to_string((stop-start).TotalMilliseconds());

    sSystemVector.clear();
    sAllSystemVector.clear();
    sHiddenSystemVector.clear();
  }
}

SystemData *SystemManager::getSystem(std::string &name)
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

SystemData *SystemManager::getFavoriteSystem()
{
  for (int i = (int) sSystemVector.size(); --i >= 0;)
    if (sSystemVector[i]->isFavorite())
      return sSystemVector[i];
  return nullptr;
}

int SystemManager::getVisibleSystemIndex(const std::string &name)
{
  for (int i = (int) sSystemVector.size(); --i >= 0;)
    if (sSystemVector[i]->mName == name)
      return i;
  return -1;
}

SystemData* SystemManager::getFirstSystemWithGame()
{
  for (auto &system : sSystemVector)
    if (system->hasGame())
      return system;

  return nullptr;
}
