//
// Created by bkg2k on 15/10/2019.
//

#include "SystemManager.h"
#include "SystemDescriptor.h"
#include <utils/Log.h>
#include <Settings.h>
#include <RecalboxConf.h>
#include <utils/os/system/ThreadPool.h>
#include <utils/Strings.h>
#include <utils/os/fs/StringMapFile.h>
#include <utils/Files.h>
#include <algorithm>

SystemData* SystemManager::CreateRegularSystem(const SystemDescriptor& systemDescriptor, bool forceLoad)
{
  const Path defaultRomsPath = Path(Settings::Instance().DefaultRomsPath()).ToAbsolute();
  Path realPath = defaultRomsPath.IsEmpty() ? systemDescriptor.RomPath() : systemDescriptor.RomPath().ToAbsolute(defaultRomsPath);

  // Create system
  SystemData* result = new SystemData(systemDescriptor, true, false);

  // Avoid files being added more than once even through symlinks
  {
    FileData::StringMap doppelgangerWatcher;

    LOG(LogInfo) << "Creating & populating system: " << systemDescriptor.FullName();

    // Populate items from disk
    bool gameListOnly = RecalboxConf::Instance().AsBool("emulationstation.gamelistonly", false) && !forceLoad;
    if (!gameListOnly)
      result->populateFolder(&(result->mRootFolder), doppelgangerWatcher);
    // Populate items from gamelist.xml
    if (!Settings::Instance().IgnoreGamelist())
      result->ParseGamelistXml(doppelgangerWatcher);
    // Overrides?
    FileData::List allFolders = result->getRootFolder()->getAllFolders();
    for(auto folder : allFolders)
      SystemData::overrideFolderInformation(folder);
  } // Let the doppelgangerWatcher to free its memory ASAP

  result->loadTheme();

  return result;
}

SystemData* SystemManager::CreateFavoriteSystem(const std::string& name, const std::string& fullName,
                                                const std::string& themeFolder, const std::vector<SystemData*>& systems)
{
  std::vector<PlatformIds::PlatformId> platformIds;
  platformIds.push_back(PlatformIds::PlatformId::PLATFORM_IGNORE);

  SystemDescriptor descriptor;
  descriptor.SetInformation("", name, fullName, "", "", themeFolder);
  SystemData* result = new SystemData(descriptor, false, true);

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

  SystemDescriptor descriptor;
  descriptor.SetInformation("", name, fullName, "", "", themeFolder);
  SystemData* result = new SystemData(descriptor, false, false);

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

void SystemManager::DeserializeEmulatorTree(XmlNode emulators, EmulatorList& emulatorList)
{
  emulatorList.Clear();
  for (const auto& emulator : emulators.children("emulator"))
  {
    const std::string& emulatorName = Xml::AttributeAsString(emulator, "name", "");
    EmulatorDescriptor emulatorDescriptor(emulatorName);
    XmlNode cores = emulator.child("cores");
    if (cores != nullptr)
      for (const auto& coreNode : cores.children("core"))
        emulatorDescriptor.AddCore(coreNode.child_value());
    if (emulatorDescriptor.HasAny()) emulatorList.AddEmulator(emulatorDescriptor);
  }
}

bool SystemManager::DeserializeSystemDescriptor(XmlNode system, SystemDescriptor& systemDescriptor)
{
  systemDescriptor.ClearEmulators();
  systemDescriptor.ClearPlatforms();

  // Information
  systemDescriptor.SetInformation(Xml::AsString(system, "path", ""),
                                  Xml::AsString(system, "name", ""),
                                  Xml::AsString(system, "fullname", ""),
                                  Xml::AsString(system, "command", ""),
                                  Xml::AsString(system, "extension", ""),
                                  Xml::AsString(system, "theme", ""));

  // Check
  if (systemDescriptor.IsValid())
  {
    // Emulator tree
    EmulatorList emulatorList;
    DeserializeEmulatorTree(system.child("emulators"), emulatorList);
    systemDescriptor.SetEmulatorList(emulatorList);

    // Platform list
    std::vector<std::string> platforms = Strings::Split(Xml::AsString(system, "platform", ""), ' ');
    for (const auto &platform : platforms)
    {
      PlatformIds::PlatformId platformId = PlatformIds::getPlatformId(platform);
      if (platformId == PlatformIds::PlatformId::PLATFORM_IGNORE)
      {
        systemDescriptor.ClearPlatforms();
        systemDescriptor.AddPlatformIdentifiers(platformId);
        break;
      }
      if (!systemDescriptor.AddPlatformIdentifiers(platformId))
        LOG(LogError) << "Platform count for system " << systemDescriptor.Name() << " full. " << platform << " ignored!";
    }
    return true;
  }

  LOG(LogError) << "System \"" << systemDescriptor.Name() << "\" is missing name, path, extension, or command!";
  return false;
}

void SystemManager::ThreadPoolTick(int completed, int /*total*/)
{
  if (mProgressInterface != nullptr)
    mProgressInterface->SetProgress(completed);
}

SystemData* SystemManager::ThreadPoolRunJob(SystemDescriptor& system)
{
  try
  {
    SystemData* newSys = CreateRegularSystem(system, mForceReload);
    if (newSys->getRootFolder()->countAll(false) == 0)
    {
      LOG(LogWarning) << "System \"" << system.Name() << "\" has no games! Ignoring it.";
      delete newSys;
      return nullptr;
    }
    else
    {
      LOG(LogWarning) << "Adding \"" << system.Name() << "\" in system list.";
      return newSys;
    }
  }
  catch(std::exception& ex)
  {
    LOG(LogError) << "System \"" << system.FullName() << "\" has raised an error. Ignored.";
    LOG(LogError) << "Exception: " << ex.what();
  }
  return nullptr;
}

bool SystemManager::LoadSystemXMLNodes(XmlNodeCollisionMap &collisionMap, XmlNodeList &nodeStore, const XmlDocument& document)
{
  bool result = false;
  XmlNode systemList = document.child("systemList");
  if (systemList != nullptr)
    for (const XmlNode system : systemList.children("system"))
    {
      // At least one node found
      result = true;

      // Composite key: fullname + platform
      std::string key = Xml::AsString(system, "fullname", "");
      key += Xml::AsString(system, "platform", "");
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

bool SystemManager::LoadXMLFile(XmlDocument& document, const Path& filepath)
{
  XmlResult result = document.load_file(filepath.ToChars());
  if (!result)
  {
    LOG(LogError) << "Could not parse " << filepath.ToString() << " file!";
    return false;
  }
  return true;
}

bool SystemManager::LoadSystemList(XmlDocument &document, XmlNodeCollisionMap &collisionMap, XmlNodeList &nodeStore, const Path &filepath)
{
  // Load user configuration
  if (!filepath.Exists())
  {
    LOG(LogError) << filepath.ToString() << " file does not exist!";
    return false;
  }

  LOG(LogInfo) << "Loading system config files " << filepath.ToString() << "...";
  if (!LoadXMLFile(document, filepath)) return false;

  bool result = LoadSystemXMLNodes(collisionMap, nodeStore, document);
  if (!result)
  {
    LOG(LogWarning) << filepath.ToString() << " has no systems or systemList nodes";
  }

  return result;
}

bool SystemManager::AddFavoriteSystem(const XmlNodeList& systemList)
{
  // Favorite system
  if (!mVisibleSystemVector.empty())
    for (const XmlNode system : systemList)
    {
      std::string name = Xml::AsString(system, "name", "");
      std::string fullname = Xml::AsString(system, "fullname", "");
      std::string themeFolder = Xml::AsString(system, "theme", "");

      if (name == "favorites")
      {
        LOG(LogInfo) << "creating favorite system";
        SystemData *newSys = CreateFavoriteSystem("favorites", fullname, themeFolder, mVisibleSystemVector);
        mVisibleSystemVector.push_back(newSys);
      }
    }

  return true;
}

bool SystemManager::AddArcadeMetaSystem()
{
  if (RecalboxConf::Instance().AsBool("global.arcade", false))
  {
    bool includeNeogeo = RecalboxConf::Instance().AsBool("global.arcade.includeneogeo", true);
    // Lookup all non-empty arcade platforms
    for (SystemData* system: mVisibleSystemVector)
      if (system->getRootFolder()->hasGame())
        for(int i = system->PlatformCount(); --i >= 0; )
          if ((system->PlatformIds(i) == PlatformIds::PlatformId::ARCADE) ||
              (system->PlatformIds(i) == PlatformIds::PlatformId::NEOGEO && includeNeogeo))
          {
            mHiddenSystemVector.push_back(system);
            break;
          }

    // Non empty?
    if (!mHiddenSystemVector.empty())
    {
      // Remove Hidden systems fron the visible ist
      bool hideOriginals = RecalboxConf::Instance().AsBool("global.arcade.hideoriginals", true);
      if (hideOriginals)
        for (SystemData* hidden: mHiddenSystemVector)
        {
          auto it = std::find(mVisibleSystemVector.begin(), mVisibleSystemVector.end(), hidden);
          if (it != mVisibleSystemVector.end())
            mVisibleSystemVector.erase(it);
        }
      // Create meta-system
      SystemData* arcade = CreateMetaSystem("arcade", "Arcade", "arcade", mHiddenSystemVector);
      LOG(LogInfo) << "creating Arcade meta-system";
      int position = RecalboxConf::Instance().AsInt("global.arcade.position", 0) % (int)mVisibleSystemVector.size();
      auto it = position >= 0 ? mVisibleSystemVector.begin() + position : mVisibleSystemVector.end() + (position + 1);
      mVisibleSystemVector.insert(it, arcade);
      // Hide originals?
      if (!hideOriginals)
        mHiddenSystemVector.clear();
    }
  }

  return !mHiddenSystemVector.empty();
}

//creates systems from information located in a config file
bool SystemManager::LoadSystemConfigurations(FileNotifier& gamelistWatcher, bool forceReloadFromDisk)
{
  mForceReload = forceReloadFromDisk;

  // System store
  XmlNodeCollisionMap systemMap;    // System key to vector index
  XmlNodeList systemList;           // Sorted storage, keeping original system node ordering

  // XML Documents - declared here to keep node memory allocated
  XmlDocument templateDocument;
  XmlDocument userDocument;

  // Load user systems
  bool userValid = LoadSystemList(userDocument, systemMap, systemList, UserConfigurationPath());
  // Load template systems
  bool templateValid = LoadSystemList(templateDocument, systemMap, systemList, TemplateConfigurationPath());

  // Is there at least
  if (!templateValid && !userValid)
  {
    LOG(LogError) << "No es_systems.cfg file available!";
    GenerateExampleConfigurationFile(UserConfigurationPath());
    return false;
  }

  DateTime start;

  // Get weight store
  StringMapFile weights(sWeightFilePath);
  weights.Load();
  // Create automatic thread-pool
  ThreadPool<SystemDescriptor, SystemData*> threadPool(this, "System-Loader", false, 20);
  // Push system to process
  for (const XmlNode system : systemList)
  {
    // Get weight
    std::string key = Xml::AsString(system, "path", "unknown");
    int weight = weights.GetInt(key, 0);
    // Create system descriptor
    SystemDescriptor descriptor;
    if (DeserializeSystemDescriptor(system, descriptor))
    {
      // Add system name and watch gamelist
      mAllDeclaredSystemShortNames.push_back(descriptor.Name());
      // Push weighted system
      threadPool.PushFeed(descriptor, weight);
    }
  }
  // Run the threadpool and automatically wait for all jobs to complete
  int count = threadPool.PendingJobs();
  if (mProgressInterface != nullptr)
    mProgressInterface->SetMaximum(count);
  threadPool.Run(-2, false);
  // Push result
  mVisibleSystemVector.resize(count, nullptr);
  int index = 0;
  for(SystemData* result = nullptr; threadPool.PopResult(result, index); )
    mVisibleSystemVector[index] = result;

  // Shrink & update weights
  for(int i = count; --i >= 0; )
  {
    SystemData* system = mVisibleSystemVector[i];
    if (system == nullptr)
      mVisibleSystemVector.erase(mVisibleSystemVector.begin() + i);
    else
      weights.SetInt(system->getStartPath().ToString(), system->getRootFolder()->countAll(true));
  }
  weights.Save();

  DateTime stop;
  LOG(LogInfo) << "Gamelist load time: " << std::to_string((stop-start).TotalMilliseconds()) << "ms";

  // Add special systems
  AddArcadeMetaSystem();
  AddFavoriteSystem(systemList);

  // Set *all* service vector
  for(SystemData* service : mVisibleSystemVector) mAllSystemVector.push_back(service);
  for(SystemData* service : mHiddenSystemVector) mAllSystemVector.push_back(service);

  // Add gamelist watching
  for(SystemData* system : mAllSystemVector)
    gamelistWatcher.WatchFile(system->getGamelistPath(false));

  return true;
}

void SystemManager::GenerateExampleConfigurationFile(const Path& path)
{
  std::string text =
    "<!-- This is the EmulationStation Systems configuration file.\n"
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

  Files::SaveFile(path, text);

  LOG(LogError) << "Example config written!  Go read it at \"" << path.ToString() << "\"!";
}

bool SystemManager::ThreadPoolRunJob(SystemData*& feed)
{
  // Save changed game data back to xml
  if (!Settings::Instance().IgnoreGamelist())
    if (feed->getRootFolder()->hasChildrenOwnership())
      feed->UpdateGamelistXml();

  return true;
}

void SystemManager::UpdateAllSystems()
{
  DateTime start;

  if (mProgressInterface != nullptr)
    mProgressInterface->SetMaximum(mAllSystemVector.size());
  // Create automatic thread-pool
  ThreadPool<SystemData*, bool> threadPool(this, "System-Save", false, 20);
  // Push system to process
  for (SystemData* system : mAllSystemVector)
    threadPool.PushFeed(system, 0);
  // Run the threadpool and automatically wait for all jobs to complete
  threadPool.Run(-2, false);

  DateTime stop;
  LOG(LogInfo) << "Gamelist update time: " << std::to_string((stop-start).TotalMilliseconds()) << "ms";
}

void SystemManager::DeleteAllSystems(bool updateGamelists)
{
  if (updateGamelists && !mAllSystemVector.empty())
    UpdateAllSystems();

  mVisibleSystemVector.clear();
  mAllSystemVector.clear();
  mHiddenSystemVector.clear();
}

SystemData *SystemManager::SystemByName(std::string &name)
{
  for (auto system: mVisibleSystemVector)
  {
    if (system->getName() == name)
    {
      return system;
    }
  }
  return nullptr;
}

SystemData *SystemManager::FavoriteSystem()
{
  for (int i = (int) mVisibleSystemVector.size(); --i >= 0;)
    if (mVisibleSystemVector[i]->isFavorite())
      return mVisibleSystemVector[i];
  return nullptr;
}

int SystemManager::getVisibleSystemIndex(const std::string &name)
{
  for (int i = (int) mVisibleSystemVector.size(); --i >= 0;)
    if (mVisibleSystemVector[i]->getName() == name)
      return i;
  return -1;
}

SystemData* SystemManager::FirstNonEmptySystem()
{
  for (auto &system : mVisibleSystemVector)
    if (system->HasGame())
      return system;

  return nullptr;
}

