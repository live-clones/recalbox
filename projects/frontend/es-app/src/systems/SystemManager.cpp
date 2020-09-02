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
#include <utils/locale/LocaleHelper.h>

SystemData* SystemManager::CreateRegularSystem(const SystemDescriptor& systemDescriptor, bool forceLoad)
{
  const Path defaultRomsPath = Path(Settings::Instance().DefaultRomsPath()).ToAbsolute();
  Path realPath = defaultRomsPath.IsEmpty() ? systemDescriptor.RomPath() : systemDescriptor.RomPath().ToAbsolute(defaultRomsPath);

  // Create system
  SystemData* result = new SystemData(*this, systemDescriptor, RootFolderData::Ownership::All, SystemData::Properties::None);

  // Avoid files being added more than once even through symlinks
  {
    FileData::StringMap doppelgangerWatcher;

    LOG(LogInfo) << "Creating & populating system: " << systemDescriptor.FullName();

    // Populate items from disk
    bool loadFromDisk = forceLoad || !RecalboxConf::Instance().AsBool("emulationstation.gamelistonly", false);
    if (loadFromDisk)
      result->populateFolder(&(result->mRootFolder), doppelgangerWatcher);
    // Populate items from gamelist.xml
    if (!Settings::Instance().IgnoreGamelist())
      result->ParseGamelistXml(doppelgangerWatcher, forceLoad);
    // Overrides?
    FileData::List allFolders = result->getRootFolder().getAllFolders();
    for(auto* folder : allFolders)
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
  SystemData* result = new SystemData(*this, descriptor, RootFolderData::Ownership::None, SystemData::Properties::Virtual | SystemData::Properties::AlwaysFlat | SystemData::Properties::Favorite);

  for (auto* system : systems)
  {
    FileData::List favs = system->getFavorites();
    if (!favs.empty())
    {
      LOG(LogWarning) << " Get " << favs.size() << " favorites for " << system->getName() << "!";
      for (auto* favorite : favs)
        result->mRootFolder.addChild(favorite, false);
    }
  }

  result->loadTheme();

  return result;
}

SystemData* SystemManager::CreateMetaSystem(const std::string& name, const std::string& fullName,
                                            const std::string& themeFolder, const std::vector<SystemData*>& systems,
                                            SystemData::Properties properties, FileData::StringMap& doppelganger,
                                            FileSorts::Sorts fixedSort)
{
  std::vector<PlatformIds::PlatformId> platformIds;
  platformIds.push_back(PlatformIds::PlatformId::PLATFORM_IGNORE);

  SystemDescriptor descriptor;
  descriptor.SetInformation("", name, fullName, "", "", themeFolder);
  SystemData* result = new SystemData(*this, descriptor, RootFolderData::Ownership::FolderOnly, SystemData::Properties::Virtual | properties, fixedSort);

  for (auto* system : systems)
  {
    FileData::List all = system->getRootFolder().getAllItems(true, system->IncludeAdultGames());
    if (!all.empty())
    {
      LOG(LogWarning) << "Add games from " << system->getName() << " into " << fullName;
      for (auto* fd : all)
        result->LookupOrCreateGame(fd->getSystem()->getRootFolder().getPath(), fd->getPath(), fd->getType(), doppelganger);
    }
  }

  result->loadTheme();

  return result;
}

SystemData* SystemManager::CreateMetaSystem(const std::string& name, const std::string& fullName,
                                            const std::string& themeFolder, const FileData::List& games,
                                            SystemData::Properties properties, FileData::StringMap& doppelganger,
                                            FileSorts::Sorts fixedSort)
{
  std::vector<PlatformIds::PlatformId> platformIds;
  platformIds.push_back(PlatformIds::PlatformId::PLATFORM_IGNORE);

  SystemDescriptor descriptor;
  descriptor.SetInformation("", name, fullName, "", "", themeFolder);
  SystemData* result = new SystemData(*this, descriptor, RootFolderData::Ownership::FolderOnly, SystemData::Properties::Virtual | properties, fixedSort);

  if (!games.empty())
  {
    LOG(LogWarning) << "Add " << games.size() << " games into " << fullName;
    for (auto* fd : games)
      result->LookupOrCreateGame(fd->getSystem()->getRootFolder().getPath(), fd->getPath(), fd->getType(), doppelganger);
  }

  result->loadTheme();

  return result;
}

void SystemManager::ThreadPoolTick(int completed, int /*total*/)
{
  if (mProgressInterface != nullptr)
    mProgressInterface->SetProgress(completed);
}

SystemData* SystemManager::ThreadPoolRunJob(SystemDescriptor& systemDescriptor)
{
  try
  {
    SystemData* newSys = CreateRegularSystem(systemDescriptor, mForceReload);
    if (newSys->getRootFolder().countAll(false, newSys->IncludeAdultGames()) == 0)
    {
      LOG(LogWarning) << "System \"" << systemDescriptor.Name() << "\" has no games! Ignoring it.";
      delete newSys;
      return nullptr;
    }
    else
    {
      mEmulatorGuard.Lock();
      mEmulatorManager.AddEmulatorList(*newSys);
      mEmulatorGuard.UnLock();
      LOG(LogWarning) << "Adding \"" << systemDescriptor.Name() << "\" in system list.";
      return newSys;
    }
  }
  catch(std::exception& ex)
  {
    LOG(LogError) << "System \"" << systemDescriptor.FullName() << "\" has raised an error. Ignored.";
    LOG(LogError) << "Exception: " << ex.what();
  }
  return nullptr;
}

bool SystemManager::AddFavoriteSystem()
{
  // Favorite system
  if (!mVisibleSystemVector.empty())
  {
    LOG(LogInfo) << "creating favorite system";
    SystemData *newSys = CreateFavoriteSystem(sFavoriteSystemShortName, _("Favorites"), sFavoriteSystemShortName, mVisibleSystemVector);
    mVisibleSystemVector.push_back(newSys);
  }

  return true;
}

bool SystemManager::AddArcadeMetaSystem()
{
  if (RecalboxConf::Instance().AsBool("emulationstation.arcade", false))
  {
    std::vector<SystemData*> arcades;
    bool includeNeogeo = RecalboxConf::Instance().AsBool("emulationstation.arcade.includeneogeo", true);
    FileData::StringMap doppelganger;

    // Lookup all non-empty arcade platforms
    for (SystemData* system: mVisibleSystemVector)
      if (system->getRootFolder().hasGame())
        for(int i = system->PlatformCount(); --i >= 0; )
          if ((system->PlatformIds(i) == PlatformIds::PlatformId::ARCADE) ||
              (system->PlatformIds(i) == PlatformIds::PlatformId::NEOGEO && includeNeogeo))
          {
            arcades.push_back(system);
            system->BuildDoppelgangerMap(doppelganger, false);
            break;
          }

    // Non empty?
    if (!arcades.empty())
    {
      // Remove Hidden systems fron the visible list
      bool hideOriginals = RecalboxConf::Instance().AsBool("emulationstation.arcade.hideoriginals", true);
      if (hideOriginals)
        for (SystemData* hidden: arcades)
        {
          auto it = std::find(mVisibleSystemVector.begin(), mVisibleSystemVector.end(), hidden);
          if (it != mVisibleSystemVector.end())
            mVisibleSystemVector.erase(it);
        }

      // Create meta-system
      SystemData* arcade = CreateMetaSystem("arcade", "Arcade", "arcade", arcades, SystemData::Properties::Virtual, doppelganger);
      LOG(LogInfo) << "creating Arcade meta-system";
      int position = RecalboxConf::Instance().AsInt("emulationstation.arcade.position", 0) % (int)mVisibleSystemVector.size();
      auto it = position >= 0 ? mVisibleSystemVector.begin() + position : mVisibleSystemVector.end() + (position + 1);
      mVisibleSystemVector.insert(it, arcade);
    }
  }

  return !mHiddenSystemVector.empty();
}

bool SystemManager::AddPorts()
{
  std::vector<SystemData*> ports;
  FileData::StringMap doppelganger;

  // Lookup all non-empty arcade platforms
  for (SystemData* system: mVisibleSystemVector)
    if (system->PlatformCount() == 1)
      if ((system->PlatformIds(0) > PlatformIds::PlatformId::PORT_START) &&
          (system->PlatformIds(0) < PlatformIds::PlatformId::PORT_STOP))
        if (system->getRootFolder().hasGame())
        {
          ports.push_back(system);
          system->BuildDoppelgangerMap(doppelganger, false);
        }

  // Non empty?
  if (!ports.empty())
  {
    // Remove port systems fron the visible list and add to the hidden list
    for (SystemData* port: ports)
    {
      auto it = std::find(mVisibleSystemVector.begin(), mVisibleSystemVector.end(), port);
      if (it != mVisibleSystemVector.end())
        mVisibleSystemVector.erase(it);
      mHiddenSystemVector.push_back(port);
    }

    // Create meta-system
    SystemData* portSystem = CreateMetaSystem("ports", "Ports", "ports", ports, SystemData::Properties::Virtual, doppelganger);
    LOG(LogInfo) << "creating Ports";
    // Seek defaulot position
    int position = 0;
    while((position < (int)mVisibleSystemVector.size()) && (portSystem->getFullName() > mVisibleSystemVector[position]->getFullName())) position++;
    position = RecalboxConf::Instance().AsInt("emulationstation.ports.position", position) % (int)mVisibleSystemVector.size();
    auto it = position >= 0 ? mVisibleSystemVector.begin() + position : mVisibleSystemVector.end() + (position + 1);
    mVisibleSystemVector.insert(it, portSystem);
  }

  return !mHiddenSystemVector.empty();
}

bool SystemManager::AddManuallyFilteredMetasystem(IFilter* filter, FileData::Comparer comparer, const std::string& identifier, const std::string& fullname, SystemData::Properties properties, FileSorts::Sorts fixedSort)
{
  std::string confPrefix("emulationstation.collection.");
  confPrefix += identifier;
  // Collection activated?
  bool collection = RecalboxConf::Instance().AsBool(confPrefix, false);
  if (collection)
  {
    // Get theme name
    std::string theme = RecalboxConf::Instance().AsString(confPrefix + ".theme", "auto-" + identifier);
    FileData::List allGames;
    FileData::StringMap doppelganger;

    // Filter and insert items
    for(const SystemData* system : mVisibleSystemVector)
      if (!system->IsVirtual())
      {
        FileData::List list = system->getRootFolder().getFilteredItemsRecursively(filter, true,
                                                                                  system->IncludeAdultGames());
        allGames.reserve(allGames.size() + list.size());
        allGames.insert(allGames.end(), list.begin(), list.end());
        // dopplegagner must be build using file only
        // Let the virtual system re-create all intermediate folder
        // ... and destroy them properly
        system->BuildDoppelgangerMap(doppelganger, false);
      }

    // Not empty?
    if (!allGames.empty())
    {
      // Sort if required
      if (comparer != nullptr)
        FolderData::Sort(allGames, comparer, true);
      // Limit if required
      int limit = RecalboxConf::Instance().AsInt(confPrefix + ".limit", 0);
      if (limit > 0)
        if (limit < (int)allGames.size())
          allGames.resize(limit);

      // Create!
      LOG(LogInfo) << "creating " << fullname << " meta-system";
      SystemData* allsystem = CreateMetaSystem(identifier, _S(fullname), theme, allGames, properties, doppelganger, fixedSort);

      // And add the system
      int position = RecalboxConf::Instance().AsInt(confPrefix + ".position", 0) % (int) mVisibleSystemVector.size();
      auto it = position >= 0 ? mVisibleSystemVector.begin() + position : mVisibleSystemVector.end() + (position + 1);
      mVisibleSystemVector.insert(it, allsystem);

      return true;
    }
  }
  return false;
}

bool SystemManager::AddAllGamesMetaSystem()
{
  class Filter: public IFilter
  {
    public:
      bool ApplyFilter(const FileData&) const override { return true; }
  } filter;
  return AddManuallyFilteredMetasystem(&filter, nullptr, sAllGamesSystemShortName, sAllGamesSystemFullName,
                                       SystemData::Properties::None);
}

bool SystemManager::AddMultiplayerMetaSystems()
{
  class Filter: public IFilter
  {
    public:
      bool ApplyFilter(const FileData& file) const override { return file.Metadata().PlayerMin() > 1 || file.Metadata().PlayerMax() > 1; }
  } filter;
  return AddManuallyFilteredMetasystem(&filter, nullptr, sMultiplayerSystemShortName, sMultiplayerSystemFullName,
                                       SystemData::Properties::None);
}

bool SystemManager::AddLastPlayedMetaSystem()
{
  class Filter: public IFilter
  {
    public:
      bool ApplyFilter(const FileData& file) const override { return file.Metadata().LastPlayedEpoc() != 0; }
  } filter;
  return AddManuallyFilteredMetasystem(&filter, nullptr, sLastPlayedSystemShortName, sLastPlayedSystemFullName,
                                       SystemData::Properties::FixedSort | SystemData::Properties::AlwaysFlat, FileSorts::Sorts::LastPlayedDescending);
}

bool SystemManager::AddGenresMetaSystem()
{
  const Genres::GenreMap& genres = Genres::GetShortNameMap();

  class Filter : public IFilter
  {
    private:
      GameGenres mGenre;
    public:
      explicit Filter(GameGenres genre) : mGenre(genre) {}
      bool ApplyFilter(const FileData& file) const override { return file.Metadata().GenreId() == mGenre; }
  };

  for(const auto& genre : genres)
  {
    Filter filter(genre.first);
    AddManuallyFilteredMetasystem(&filter, nullptr, genre.second, Genres::GetName(genre.first),
                                  SystemData::Properties::None);
  }
  return true;
}

bool SystemManager::AddSpecialCollectionsMetaSystems()
{
  AddPorts();
  AddAllGamesMetaSystem();
  AddLastPlayedMetaSystem();
  AddMultiplayerMetaSystems();
  AddGenresMetaSystem();

  return true;
}

// Creates systems from information located in a config file
bool SystemManager::LoadSystemConfigurations(FileNotifier& gamelistWatcher, bool forceReloadFromDisk)
{
  mForceReload = forceReloadFromDisk;

  SystemDeserializer deserializer;
  bool loaded = deserializer.LoadSystems();
  // Is there at least
  if (!loaded)
  {
    LOG(LogError) << "No es_systems.cfg file available!";
    GenerateExampleConfigurationFile(SystemDeserializer::UserConfigurationPath());
    return false;
  }

  DateTime start;

  // Get weight store
  StringMapFile weights(sWeightFilePath);
  weights.Load();
  // Create automatic thread-pool
  ThreadPool<SystemDescriptor, SystemData*> threadPool(this, "System-Load", false, 20);
  // Push system to process
  for (int index = 0; index < deserializer.Count(); ++index)
  {
    // Create system descriptor
    SystemDescriptor descriptor;
    if (deserializer.Deserialize(index, descriptor))
    {
      if (!RecalboxConf::Instance().AsBool(descriptor.Name() + ".ignore"))
      {
        // Get weight
        int weight = weights.GetInt(descriptor.RomPath().ToString(), 0);
        // Add system name and watch gamelist
        mAllDeclaredSystemShortNames.push_back(descriptor.Name());
        // Push weighted system
        threadPool.PushFeed(descriptor, weight);
      }
      else LOG(LogInfo) << descriptor.FullName() << " ignored in configuration.";
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
      weights.SetInt(system->getStartPath().ToString(), system->getRootFolder().countAll(true,
                                                                                         system->IncludeAdultGames()));
  }
  weights.Save();

  DateTime stop;
  LOG(LogInfo) << "Gamelist load time: " << std::to_string((stop-start).TotalMilliseconds()) << "ms";

  // Add special systems
  AddFavoriteSystem();
  AddSpecialCollectionsMetaSystems();
  AddArcadeMetaSystem(); // Must be latest

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
    if (!feed->IsVirtual())
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
    if (!system->IsVirtual())
      threadPool.PushFeed(system, 0);
  // Run the threadpool and automatically wait for all jobs to complete
  if (threadPool.PendingJobs() != 0)
    threadPool.Run(-2, false);

  DateTime stop;
  LOG(LogInfo) << "Gamelist update time: " << std::to_string((stop-start).TotalMilliseconds()) << "ms";
}

void SystemManager::DeleteAllSystems(bool updateGamelists)
{
  if (updateGamelists && !mAllSystemVector.empty())
    UpdateAllSystems();

  for(SystemData* system : mAllSystemVector)
    delete system;

  mVisibleSystemVector.clear();
  mAllSystemVector.clear();
  mHiddenSystemVector.clear();
}

SystemData *SystemManager::SystemByName(std::string &name)
{
  for (auto *system: mVisibleSystemVector)
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
    if (mVisibleSystemVector[i]->IsFavorite())
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

void SystemManager::UpdateLastPlayedSystem(FileData& game)
{
  // Get last-played system
  int index = getVisibleSystemIndex(sLastPlayedSystemShortName);
  if (index < 0) return; // No last played system
  SystemData& system = *mVisibleSystemVector[index];
  // Update system
  system.UpdateLastPlayedGame(game);
}

void SystemManager::SearchResultQuickSortAscending(FolderData::ResultList& items, int low, int high)
{
  int Low = low, High = high;
  const FolderData::FastSearchItem& pivot = items[(Low + High) >> 1];
  do
  {
    while(items[Low].Distance  < pivot.Distance) Low++;
    while(items[High].Distance > pivot.Distance) High--;
    if (Low <= High)
    {
      FolderData::FastSearchItem Tmp = items[Low]; items[Low] = items[High]; items[High] = Tmp;
      Low++; High--;
    }
  }while(Low <= High);
  if (High > low) SearchResultQuickSortAscending(items, low, High);
  if (Low < high) SearchResultQuickSortAscending(items, Low, high);
}

FileData::List SystemManager::searchTextInGames(FolderData::FastSearchContext context, const std::string& originaltext, int maxpersystem, int maxglobal)
{
  std::string lowercaseText = Strings::ToLowerUTF8(originaltext);

  // Get search results
  FolderData::ResultList searchResults;
  searchResults.reserve(5000);
  for(auto *system : mVisibleSystemVector)
    if (!system->IsVirtual())
    {
      int maximumResultPerSystem = maxpersystem;
      system->getRootFolder().FastSearch(context, lowercaseText, searchResults, maximumResultPerSystem);
    }

  // Sort results
  if (searchResults.size() > 1)
    SearchResultQuickSortAscending(searchResults, 0, (int)searchResults.size() - 1);

  // Copy to final list
  FileData::List results;
  results.reserve(searchResults.size());
  for(auto& sr : searchResults)
    if (--maxglobal >= 0)
      results.push_back(sr.Data);

  return results;
}