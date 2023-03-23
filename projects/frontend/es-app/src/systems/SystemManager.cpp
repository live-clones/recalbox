//
// Created by bkg2k on 15/10/2019.
//

#include "SystemManager.h"
#include "SystemDescriptor.h"
#include "SystemDeserializer.h"
#include "LightGunDatabase.h"
#include "games/classifications/Versions.h"
#include "utils/hash/Crc32.h"
#include <systems/SystemSorting.h>
#include <utils/Log.h>
#include <RecalboxConf.h>
#include <utils/os/system/ThreadPool.h>
#include <utils/Strings.h>
#include <utils/os/fs/StringMapFile.h>
#include <utils/Files.h>
#include <algorithm>
#include <utils/locale/LocaleHelper.h>
#include <dirent.h>

SystemManager::RomSources SystemManager::GetRomSource(const SystemDescriptor& systemDescriptor, PortTypes port)
{
  RomSources roots;
  if (Strings::Contains(systemDescriptor.RomPath().ToString(), sRootTag))
  {
    std::string rootTag(sRootTag);
    // Share_init roms
    Path root = Path(Strings::Replace(systemDescriptor.RomPath().ToString(), rootTag, sShareInitRomRoot));
    if (root.Exists() && port != PortTypes::ShareOnly) roots[root.ToString()] = true;

    if (port != PortTypes::ShareInitOnly)
    {
      // Share roms
      root = Path(Strings::Replace(systemDescriptor.RomPath().ToString(), rootTag, sShareRomRoot));
      if (root.Exists()) roots[root.ToString()] = false;
      // External mount points
      for(const Path& externalRoms : mMountPoints)
      {
        root = Path(Strings::Replace(systemDescriptor.RomPath().ToString(), rootTag, externalRoms.ToString()));
        if (root.Exists())
        {
          const DeviceMount* mount = mMountPointMonitoring.SizeOf(root);
          bool readOnly = (mount != nullptr && mount->ReadOnly());
          roots[root.ToString()] = readOnly;
        }
      }
    }
  }
  else
  {
    // For compatibility until we move romfs
    bool ok = false;
    Path originalRomPath(sShareRomRoot);
    Path relative(systemDescriptor.RomPath().MakeRelative(originalRomPath, ok));
    if (ok)
    {
      Path root = Path(sShareInitRomRoot) / relative;
      if (root.Exists() && port != PortTypes::ShareOnly) roots[root.ToString()] = true;
      if (port != PortTypes::ShareInitOnly)
      {
        root = Path(sShareRomRoot) / relative;
        if (root.Exists()) roots[root.ToString()] = false;
      }
    }
    else
    {
      roots[systemDescriptor.RomPath().ToString()] = false;
      { LOG(LogError) << "[System] " << systemDescriptor.RomPath().ToString() << " is a standalone folder."; }
    }
  }

  return roots;
}

void SystemManager::CheckAutoScraping(SystemData& system)
{
  class : public IParser
  {
    public:
      void Parse(FileData& game) override
      {
        static std::string png(LEGACY_STRING(".png"));
        if (game.IsGame())
          if (game.Metadata().Image().IsEmpty())
            if (Strings::ToLowerASCII(game.RomPath().Extension()) == png)
              game.Metadata().SetImagePath(game.RomPath());
      }
  } autoScraper;

  if ((system.IsAutoScrapable()))
    system.MasterRoot().ParseAllItems(autoScraper);
}

void SystemManager::CheckFolderOverriding(SystemData& system)
{
  class : public IParser
  {
    private:
      /*!
       * @brief Get localized text inside a text. Look for [lg] tags to mark start/end of localized texts
       * if the current language is not found, the method looks for [en].
       * if still not found, the whole text is returned
       * @param source Source text
       * @return localized text
       */
      static std::string LocalizedText(const std::string& source)
      {
        // Extract prefered language/region
        std::string locale = Strings::ToLowerASCII(RecalboxConf::Instance().GetSystemLanguage());

        // Get start
        std::string key(1, ']');
        key.append(locale).append(1, ']');
        unsigned long start = source.find(key);
        if (start == std::string::npos)
        {
          std::string language = (locale.length() == 5) ? locale.substr(0, 2) : "en";
          key.clear();
          key.append(1, '[').append(language).append(1, ']');
          start = source.find(key);
          if (start == std::string::npos)
          {
            key = "[en]";
            start = source.find(key);
            if (start == std::string::npos)
              return source;
          }
        }

        // Get end
        unsigned long stop = source.find('[', start + key.length());
        if (stop == std::string::npos) stop = source.length();

        // Trimming
        start = source.find_first_not_of(" \t\n\r", start + key.length());
        if (start == std::string::npos) return "";
        stop = source.find_last_not_of(" \t\n\r", stop);

        return source.substr(start, stop - start);
      }

    public:
      /*!
       * @brief Looks for folder override files in the given folder.
       * If overriden images/texts are found, thay are loaded to override empty or gamelist information
       * The methods looks for:
       * - .folder.picture.svg or .folder.picture.png
       * - .folder.description.txt
       * @param game folder data to override
       */
      void Parse(FileData& game) override
      {
        if (game.IsFolder())
        {
          Path romPath = game.RomPath();
          // Override image
          Path fullPath = romPath / ".folder.picture.png";
          if (fullPath.Exists())
          {
            game.Metadata().SetVolatileImagePath(fullPath);
            fullPath = romPath / ".folder.description.txt";
            std::string text = Files::LoadFile(fullPath);
            if (!text.empty())
            {
              text = LocalizedText(text);
              if (text.length() != 0)
                game.Metadata().SetVolatileDescription(text);
            }
          }
        }
      }
  } overrider;

  system.MasterRoot().ParseAllItems(overrider);
}

void SystemManager::CheckMissingHashed(SystemData& system)
{
  class MissingHashes : public IParser
  {
    private:
      int mCount;
      bool mFound;

    public:
      MissingHashes() : mCount(0), mFound(false) {}

      void Parse(FileData& game) override
      {
        if (game.IsGame())
          if (game.Metadata().RomCrc32() == 0)
          {
            game.CalculateHash();
            mCount++;
            mFound = true;
          }
      }

      [[nodiscard]] int HashCount() const { return mCount; }
      [[nodiscard]] bool MissingHashesFound() const { return mFound; }
  } missingHashes;

  if (RecalboxConf::Instance().GetNetplayEnabled())
    if (system.Descriptor().HasNetPlayCores())
    {
      DateTime start;
      system.MasterRoot().ParseAllItems(missingHashes);
      if (missingHashes.MissingHashesFound())
      { LOG(LogInfo) << "[System] Calculated " << missingHashes.HashCount() << " missing hashes of " << system.FullName() << ". Took " << (DateTime() - start).TotalMilliseconds() << "ms."; }
    }
}

void SystemManager::BuildDynamicMetadata(SystemData& system)
{
  class : public IParser
  {
    private:
      //! Mini structure to store a game and its version
      struct VersionedGame
      {
        FileData*              Game;
        Versions::GameVersions Version;
        VersionedGame(FileData& game, Versions::GameVersions version)
          : Game(&game)
          , Version(version)
        {}
      };
      //! Keep the highest versioned FileData instance for a given key (game+regions)
      HashMap<std::string, VersionedGame> mHighestVersions;

    public:
      void Parse(FileData& game) override
      {
        if (game.IsGame())
        {
          // Highest version
          Path romPath = game.RomPath();
          std::string fileName = romPath.Filename();
          Versions::GameVersions version = Versions::ExtractGameVersionNoIntro(fileName);
          std::string gameNameWithRegion = Strings::RemoveParenthesis(fileName).append(Regions::Serialize4Regions(Regions::ExtractRegionsFromNoIntroName(fileName)));

          VersionedGame* previous = mHighestVersions.try_get(gameNameWithRegion);
          if (previous == nullptr)
          {
            game.Metadata().SetLatestVersion(true);
            mHighestVersions.insert_or_assign(gameNameWithRegion, VersionedGame(game, version));
          }
          else if (version > previous->Version)
          {
            previous->Game->Metadata().SetLatestVersion(false);
            game.Metadata().SetLatestVersion(true);
            mHighestVersions.insert_or_assign(gameNameWithRegion, VersionedGame(game, version));
          }

          // Not a game?
          game.Metadata().SetNoGame(Strings::StartsWith(game.Name(),LEGACY_STRING("ZZZ")) ||
                                    Strings::StartsWith(fileName, LEGACY_STRING("[BIOS]")));
        }
      }
  } dynamicMetadata;

  system.MasterRoot().ParseAllItems(dynamicMetadata);
}

SystemData* SystemManager::CreateRegularSystem(const SystemDescriptor& systemDescriptor, bool forceLoad)
{
  // Create system
  SystemData::Properties properties = SystemData::Properties::Searchable;
  if (systemDescriptor.Name() == "pico8") properties |= SystemData::Properties::GameInPng;
  if (systemDescriptor.Name() == "imageviewer") properties = SystemData::Properties::GameInPng | SystemData::Properties::ScreenShots;

  SystemData* result = new SystemData(*this, systemDescriptor, properties);

  PortTypes port = PortTypes::None;
  if (systemDescriptor.IsPort())
    port = systemDescriptor.IsReadOnly() ? PortTypes::ShareInitOnly : PortTypes::ShareOnly;

  // Build root list
  HashMap<std::string, bool> roots = GetRomSource(systemDescriptor, port);

  // Avoid files being added more than once even through symlinks
  for(const auto& rootPath : roots)
  {
    RootFolderData& root = result->LookupOrCreateRootFolder(Path(rootPath.first),
                                                            RootFolderData::Ownership::All,
                                                            rootPath.second ? RootFolderData::Types::ReadOnly : RootFolderData::Types::None);
    FileData::StringMap doppelgangerWatcher;

    { LOG(LogInfo) << "[System] Creating & populating system: " << systemDescriptor.FullName() << " (from " << rootPath.first << ')'; }

    // Populate items from disk
    bool loadFromDisk = forceLoad || !RecalboxConf::Instance().GetStartupGamelistOnly();
    if (loadFromDisk)
      result->populateFolder(root, doppelgangerWatcher);

    // Populate items from gamelist.xml
    result->ParseGamelistXml(root, doppelgangerWatcher, forceLoad);

    #ifdef DEBUG
    { LOG(LogInfo) << "[System] " << root.CountAll(false, FileData::Filter::None) << " games found for " << systemDescriptor.FullName() << " in " << rootPath.first; }
    #endif
  } // Let the doppelgangerWatcher to free its memory ASAP

  // Hashing
  CheckMissingHashed(*result);
  // Game In Png?
  CheckAutoScraping(*result);
  // Overrides?
  CheckFolderOverriding(*result);
  // Dynamic data
  BuildDynamicMetadata(*result);

  // Load theme
  result->loadTheme();

  return result;
}

SystemData* SystemManager::CreateFavoriteSystem(const std::string& name, const std::string& fullName,
                                                const std::string& themeFolder, const std::vector<SystemData*>& systems)
{
  std::vector<PlatformIds::PlatformId> platformIds;
  platformIds.push_back(PlatformIds::PlatformId::PLATFORM_IGNORE);

  SystemDescriptor descriptor;
  descriptor.SetSystemInformation(fullName, name, fullName)
            .SetPropertiesInformation("virtual", "mandatory", "mandatory", "mandatory", "2020-01-01", "None", false, false, false, "")
            .SetDescriptorInformation("", "", themeFolder, "", "", false, false);
  SystemData* result = new SystemData(*this, descriptor, SystemData::Properties::Virtual | SystemData::Properties::AlwaysFlat | SystemData::Properties::Favorite);

  FolderData& root = result->LookupOrCreateRootFolder(Path(), RootFolderData::Ownership::None, RootFolderData::Types::Virtual);
  for (auto* system : systems)
  {
    FileData::List favs = system->getFavorites();
    if (!favs.empty())
    {
      { LOG(LogWarning) << "[System]   Get " << favs.size() << " favorites for " << system->Name() << "!"; }
      for (auto* favorite : favs)
        root.AddChild(favorite, false);
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
  descriptor.SetSystemInformation(fullName, name, fullName)
            .SetPropertiesInformation("engine", "mandatory", "optional", "no", "2020-01-01", "None", false, false, false, "")
            .SetDescriptorInformation("", "", themeFolder, "", "", false, false);
  SystemData* result = new SystemData(*this, descriptor, SystemData::Properties::Virtual | properties, fixedSort);

  RootFolderData& root = result->LookupOrCreateRootFolder(Path(), RootFolderData::Ownership::FolderOnly, RootFolderData::Types::Virtual);
  for(SystemData* source : systems)
  {
    FileData::List all = source->getTopGamesAndFolders();
    if (!all.empty())
    {
      { LOG(LogWarning) << "[System] Add games from " << source->Name() << " into " << fullName; }
      for (auto* fd : all)
        result->LookupOrCreateGame(root, fd->TopAncestor().RomPath(), fd->RomPath(), fd->Type(), doppelganger);
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
  descriptor.SetSystemInformation(fullName, name, fullName)
            .SetPropertiesInformation("virtual", "mandatory", "mandatory", "mandatory", "2020-01-01", "None", false, false, false, "")
            .SetDescriptorInformation("", "", themeFolder, "", "", false, false);
  SystemData* result = new SystemData(*this, descriptor, SystemData::Properties::Virtual | properties, fixedSort);

  if (!games.empty())
  {
    RootFolderData& root = result->CreateRootFolder(Path(), RootFolderData::Ownership::FolderOnly, RootFolderData::Types::Virtual);
    { LOG(LogWarning) << "[System] Add " << games.size() << " games into " << fullName; }
    for (auto* fd : games)
      result->LookupOrCreateGame(root, fd->TopAncestor().RomPath(), fd->RomPath(), fd->Type(), doppelganger);
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
    if (!newSys->HasGame())
    {
      { LOG(LogWarning) << "[System] System \"" << systemDescriptor.Name() << "\" has no games! Ignoring it."; }
      delete newSys;
      return nullptr;
    }
    else
    {
      mEmulatorGuard.Lock();
      mEmulatorManager.AddEmulatorList(*newSys);
      mEmulatorGuard.UnLock();
      { LOG(LogWarning) << "[System] Adding \"" << systemDescriptor.Name() << "\" in system list."; }
      return newSys;
    }
  }
  catch(std::exception& ex)
  {
    { LOG(LogError) << "[System] System \"" << systemDescriptor.FullName() << "\" has raised an error. Ignored."; }
    { LOG(LogError) << "[System] Exception: " << ex.what(); }
  }
  return nullptr;
}

bool SystemManager::AddFavoriteSystem()
{
  // Favorite system
  if (!mVisibleSystemVector.empty())
  {
    { LOG(LogInfo) << "[System] Creating favorite system"; }
    SystemData *newSys = CreateFavoriteSystem(sFavoriteSystemShortName, _("Favorites"), sFavoriteSystemShortName, mVisibleSystemVector);
    mVisibleSystemVector.push_back(newSys);
  }

  return true;
}

bool SystemManager::AddArcadeMetaSystem()
{
  if (RecalboxConf::Instance().GetCollectionArcade())
  {
    std::vector<SystemData*> arcades;
    bool includeNeogeo = RecalboxConf::Instance().GetCollectionArcadeNeogeo();
    FileData::StringMap doppelganger;

    // Lookup all non-empty arcade platforms
    for (SystemData* system: mVisibleSystemVector)
      if (system->HasGame())
        if (system->Descriptor().Type() == SystemDescriptor::SystemType::Arcade ||
            (includeNeogeo && system->Descriptor().Name() == "neogeo"))
        {
          arcades.push_back(system);
          system->BuildDoppelgangerMap(doppelganger, false);
        }

    // Non empty?
    if (!arcades.empty())
    {
      // Remove Hidden systems from the visible list
      bool hideOriginals = RecalboxConf::Instance().GetCollectionArcadeHide();
      if (hideOriginals)
        for (SystemData* hidden: arcades)
        {
          auto it = std::find(mVisibleSystemVector.begin(), mVisibleSystemVector.end(), hidden);
          if (it != mVisibleSystemVector.end())
            mVisibleSystemVector.erase(it);
          mHiddenSystemVector.push_back(hidden);
        }

      // Create meta-system
      SystemData::Properties properties = SystemData::Properties::Virtual;
      if (hideOriginals) properties |= SystemData::Properties::Searchable;
      SystemData* arcade = CreateMetaSystem("arcade", "Arcade", "arcade", arcades, properties, doppelganger);
      { LOG(LogInfo) << "[System] Creating Arcade meta-system"; }
      int position = RecalboxConf::Instance().GetCollectionArcadePosition() % (int)mVisibleSystemVector.size();
      auto it = position >= 0 ? mVisibleSystemVector.begin() + position : mVisibleSystemVector.end() + (position + 1);
      mVisibleSystemVector.insert(it, arcade);
    }

    return !arcades.empty();
  }
  return false;
}

bool SystemManager::AddPorts()
{
  std::vector<SystemData*> ports;
  FileData::StringMap doppelganger;

  // Lookup all non-empty arcade platforms
  for (SystemData* system: mVisibleSystemVector)
    if (system->Descriptor().IsPort() && system->HasGame())
      {
        ports.push_back(system);
        system->BuildDoppelgangerMap(doppelganger, false);
      }

  // Non empty?
  if (!ports.empty())
  {
    // Remove port systems from the visible list and add to the hidden list
    for (SystemData* port: ports)
    {
      auto it = std::find(mVisibleSystemVector.begin(), mVisibleSystemVector.end(), port);
      if (it != mVisibleSystemVector.end())
        mVisibleSystemVector.erase(it);
      mHiddenSystemVector.push_back(port);
    }

    if ((!RecalboxConf::Instance().GetCollectionHide("ports")) || (mVisibleSystemVector.size() == 1))
    {
      // Create meta-system
      SystemData* portSystem = CreateMetaSystem("ports", "Ports", "ports", ports, SystemData::Properties::Virtual | SystemData::Properties::Searchable, doppelganger);
      { LOG(LogInfo) << "[System] Creating Ports"; }
      // Seek default position
      int position = RecalboxConf::Instance().GetCollectionPosition("ports") % (int)mVisibleSystemVector.size();
      if (position == 0)
        while((position < (int)mVisibleSystemVector.size()) && (portSystem->Name() > mVisibleSystemVector[position]->Name())) position++;
      auto it = position >= 0 ? mVisibleSystemVector.begin() + position : mVisibleSystemVector.end() + (position + 1);
      mVisibleSystemVector.insert(it, portSystem);
    }
  }

  return !ports.empty();
}

bool SystemManager::AddManuallyFilteredMetasystem(IFilter* filter, FileData::Comparer comparer, const std::string& identifier, const std::string& fullname, SystemData::Properties properties, FileSorts::Sorts fixedSort)
{
  // Collection activated?
  bool collection = RecalboxConf::Instance().GetCollection(identifier);
  if (collection)
  {
    // Get theme name
    std::string theme = RecalboxConf::Instance().GetCollectionTheme(identifier);
    FileData::List allGames;
    FileData::StringMap doppelganger;

    // Filter and insert items
    for(const SystemData* system : mVisibleSystemVector)
      if (!system->IsVirtual())
      {
        for (const RootFolderData* root : system->MasterRoot().SubRoots())
          if (!root->Virtual())
          {
            FileData::List list = root->GetFilteredItemsRecursively(filter, true);
            allGames.reserve(allGames.size() + list.size());
            allGames.insert(allGames.end(), list.begin(), list.end());
          }
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
      int limit = RecalboxConf::Instance().GetCollectionLimit(identifier);
      if (limit > 0)
        if (limit < (int)allGames.size())
          allGames.resize(limit);

      // Create!
      { LOG(LogInfo) << "[System] Creating " << fullname << " meta-system"; }
      SystemData* allsystem = CreateMetaSystem(identifier, _S(fullname), theme, allGames, properties, doppelganger, fixedSort);

      // And add the system
      int position = RecalboxConf::Instance().GetCollectionPosition(identifier) % (int) mVisibleSystemVector.size();
      auto it = position >= 0 ? mVisibleSystemVector.begin() + position : mVisibleSystemVector.end() + (position + 1);
      mVisibleSystemVector.insert(it, allsystem);

      return true;
    }
  }
  return false;
}

bool SystemManager::AddLightGunMetaSystem()
{
  std::string identifier("lightgun");
  std::string fullname("LightGun Games");
  // Collection activated?
  bool collection = !RecalboxConf::Instance().GetCollectionHide(identifier);
  if (collection)
  {
    // Get theme name
    std::string theme = RecalboxConf::Instance().GetCollectionTheme(identifier);
    FileData::List allGames;
    FileData::StringMap doppelganger;

    // Filter and insert items
    LightGunDatabase database;
    for(const SystemData* system : mVisibleSystemVector)
      if (database.SetCurrentSystem(*system))
      {
        for (const RootFolderData* root : system->MasterRoot().SubRoots())
          if (!root->Virtual())
          {
            FileData::List list = root->GetFilteredItemsRecursively(&database, true);
            allGames.reserve(allGames.size() + list.size());
            allGames.insert(allGames.end(), list.begin(), list.end());
          }
        // dopplegagner must be build using file only
        // Let the virtual system re-create all intermediate folder
        // ... and destroy them properly
        system->BuildDoppelgangerMap(doppelganger, false);
      }

    // Not empty?
    if (!allGames.empty())
    {
      // Limit if required
      int limit = RecalboxConf::Instance().GetCollectionLimit(identifier);
      if (limit > 0)
        if (limit < (int)allGames.size())
          allGames.resize(limit);

      // Create!
      { LOG(LogInfo) << "[System] Creating " << fullname << " meta-system"; }
      SystemData::Properties props = SystemData::Properties::Virtual |
                                     SystemData::Properties::AlwaysFlat;
      SystemData* allsystem = CreateMetaSystem(identifier, _S(fullname), theme, allGames, props, doppelganger);

      // And add the system
      int position = RecalboxConf::Instance().GetCollectionPosition(identifier) % (int) mVisibleSystemVector.size();
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
      [[nodiscard]] bool ApplyFilter(const FileData& fileData) const override
      {
        return fileData.IsDisplayable();
      }
  } filter;
  return AddManuallyFilteredMetasystem(&filter, nullptr, sAllGamesSystemShortName, sAllGamesSystemFullName,
                                       SystemData::Properties::None);
}

bool SystemManager::AddMultiplayerMetaSystems()
{
  class Filter: public IFilter
  {
    public:
      [[nodiscard]] bool ApplyFilter(const FileData& file) const override
      {
        return file.IsDisplayable() && (file.Metadata().PlayerMin() > 1 || file.Metadata().PlayerMax() > 1);
      }
  } filter;
  return AddManuallyFilteredMetasystem(&filter, nullptr, sMultiplayerSystemShortName, sMultiplayerSystemFullName,
                                       SystemData::Properties::None);
}

bool SystemManager::AddLastPlayedMetaSystem()
{
  class Filter: public IFilter
  {
    public:
      [[nodiscard]] bool ApplyFilter(const FileData& file) const override
      {
        return file.IsDisplayable() && file.Metadata().LastPlayedEpoc() != 0;
      }
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
      bool mSubGenre;

    public:
      explicit Filter(GameGenres genre)
        : mGenre(genre)
        , mSubGenre(Genres::IsSubGenre(genre))
      {
      }
      [[nodiscard]] bool ApplyFilter(const FileData& file) const override
      {
        bool isDisplayable = file.IsDisplayable();
        if (mSubGenre) return isDisplayable && file.Metadata().GenreId() == mGenre;
        return isDisplayable && Genres::TopGenreMatching(file.Metadata().GenreId(), mGenre);
      }
  };

  for(const auto& genre : genres)
  {
    Filter filter(genre.first);
    AddManuallyFilteredMetasystem(&filter, nullptr, genre.second, Genres::GetName(genre.first),
                                  SystemData::Properties::None);
  }
  return true;
}

bool SystemManager::AddSpecialCollectionsMetaSystems(bool portableSystem)
{
  AddAllGamesMetaSystem();
  AddLastPlayedMetaSystem();
  AddMultiplayerMetaSystems();
  if (!portableSystem) AddLightGunMetaSystem();
  AddGenresMetaSystem();

  return true;
}

// Creates systems from information located in a config file
bool SystemManager::LoadSystemConfigurations(FileNotifier& gamelistWatcher, bool forceReloadFromDisk, bool portableSystem)
{
  mForceReload = forceReloadFromDisk;

  SystemDeserializer deserializer;
  bool loaded = deserializer.LoadSystems();
  // Is there at least
  if (!loaded)
  {
    { LOG(LogError) << "[System] No systemlist.xml file available!"; }
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
        int weight = weights.GetInt(descriptor.FullName(), 0);
        // Add system name and raw rompath
        mAllDeclaredSystemShortNames.push_back(descriptor.Name());
        mAllDeclaredSystemRomPathes.push_back(descriptor.RomPath());
        for(const std::string& ext : Strings::Split(descriptor.Extension(), ' ', true))
          mAllDeclaredSystemExtensionSet.insert(ext);
        // Push weighted system
        threadPool.PushFeed(descriptor, weight);
      }
      else { LOG(LogInfo) << "[System] " << descriptor.FullName() << " ignored in configuration."; }
    }
  }

  // Initialize external mount points first so that they can be used in system loading
  InitializeMountPoints();

  // Run the threadpool and automatically wait for all jobs to complete
  int count = threadPool.PendingJobs();
  if (mProgressInterface != nullptr)
    mProgressInterface->SetMaximum(count);
  threadPool.Run(-2, false);
  // Push result
  { LOG(LogInfo) << "[System] Store visible systems"; }
  mVisibleSystemVector.resize(count, nullptr);
  int index = 0;
  for(SystemData* result = nullptr; threadPool.PopResult(result, index); )
    mVisibleSystemVector[index] = result;

  // Shrink & update weights
  { LOG(LogInfo) << "[System] Update weights"; }
  std::vector<SystemData*> visibleSystem;
  for(SystemData* system : mVisibleSystemVector)
    if (system != nullptr)
    {
      visibleSystem.push_back(system);
      weights.SetInt(system->FullName(), system->GameAndFolderCount());
    }
  mVisibleSystemVector = visibleSystem;
  { LOG(LogInfo) << "[System] Final non-virtual visible systems: " << mVisibleSystemVector.size(); }
  weights.Save();

  DateTime stop;
  { LOG(LogInfo) << "[System] Gamelist load time: " << std::to_string((stop-start).TotalMilliseconds()) << "ms"; }

  // Cleanup metadata
  MetadataDescriptor::CleanupHolders();

  // Add special systems
  AddFavoriteSystem();
  AddPorts(); // Must be first after "normal systems"
  AddSpecialCollectionsMetaSystems(portableSystem);
  AddArcadeMetaSystem(); // Must be latest

  // Sort systems based on conf option
  mOriginalVisibleSystemVector = mVisibleSystemVector;
  SystemSorting(mVisibleSystemVector, mOriginalVisibleSystemVector);

  // Set *all* service vector
  for(SystemData* service : mVisibleSystemVector) mAllSystemVector.push_back(service);
  for(SystemData* service : mHiddenSystemVector) mAllSystemVector.push_back(service);

  // Add gamelist watching
  for(SystemData* system : mAllSystemVector)
    for(const Path& path : system->WritableGamelists())
      if (path.Exists())
        gamelistWatcher.WatchFile(path);

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

  { LOG(LogError) << "[System] Example config written!  Go read it at \"" << path.ToString() << "\"!"; }
}

bool SystemManager::ThreadPoolRunJob(SystemData*& feed)
{
  // Save changed game data back to xml
  if (!feed->IsVirtual())
    feed->UpdateGamelistXml();

  return true;
}

void SystemManager::UpdateAllSystems()
{
  DateTime start;

  if (mProgressInterface != nullptr)
    mProgressInterface->SetMaximum((int)mAllSystemVector.size());
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
  { LOG(LogInfo) << "[System] Gamelist update time: " << std::to_string((stop-start).TotalMilliseconds()) << "ms"; }
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

SystemData *SystemManager::SystemByName(const std::string &name)
{
  for (auto *system: mVisibleSystemVector)
    if (system->Name() == name)
      return system;
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
    if (mVisibleSystemVector[i]->Name() == name)
      return i;
  return -1;
}

SystemData* SystemManager::FirstNonEmptySystem()
{
  for (auto &system : mVisibleSystemVector)
    if (system->HasVisibleGame())
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

FileData::List SystemManager::SearchFullMatchInGames(FolderData::FastSearchContext context, const std::string& originaltext, int maxglobal, const SystemData* targetSystem)
{
  std::string lowercaseText = Strings::ToLowerUTF8(originaltext);
  FileData::List resultsList;

  // Build searchable system list
  std::list<const SystemData*> searchableSystems;

  if (targetSystem != nullptr && targetSystem->IsSearchable() && !targetSystem->IsFavorite())
    searchableSystems.push_back(targetSystem);
  else
    for (const SystemData* system : GetVisibleSystemList())
    {
      if (system->IsSearchable() && system->HasVisibleGame())
        searchableSystems.push_back(system);
    }

  for (const SystemData* system: searchableSystems)
  {
    for (auto& game: system->getGames()) {
      if(game->IsGame())
      {
        std::string found;
        switch (context)
        {
          case FolderData::FastSearchContext::Name: found = game->Metadata().Name();
            break;
          case FolderData::FastSearchContext::Alias: found = game->Metadata().Alias();
            break;
          case FolderData::FastSearchContext::Family: found = game->Metadata().Family();
            break;
          case FolderData::FastSearchContext::Path:
          case FolderData::FastSearchContext::Description:
          case FolderData::FastSearchContext::Developer:
          case FolderData::FastSearchContext::Publisher:
          case FolderData::FastSearchContext::All: break;
        }

        if (Strings::ToLowerUTF8(found) == lowercaseText)

          resultsList.push_back(game);
      }
      if ((int)resultsList.size() >= maxglobal)
        break;
    }
  }

  return resultsList;
}


FileData::List SystemManager::SearchTextInGames(FolderData::FastSearchContext context, const std::string& originaltext, int maxglobal, const SystemData* targetSystem)
{
  // Everything to lowercase cause search is not case sensitive
  std::string lowercaseText = Strings::ToLowerUTF8(originaltext);

  // Fast search into metadata, collecting index and distances
  { LOG(LogDebug) << "[Search] Start searching for '" << lowercaseText << '\''; }
  MetadataStringHolder::FoundTextList resultIndexes(1024, 1024);
  switch(context)
  {
    case FolderData::FastSearchContext::Name       :
      MetadataDescriptor::SearchInNames(lowercaseText, resultIndexes, (int)FolderData::FastSearchContext::Name);
      MetadataDescriptor::SearchInAlias(lowercaseText, resultIndexes, (int)FolderData::FastSearchContext::Alias);
      break;

    case FolderData::FastSearchContext::Alias      : MetadataDescriptor::SearchInAlias(lowercaseText, resultIndexes, (int)FolderData::FastSearchContext::Alias); break;
    case FolderData::FastSearchContext::Family     : MetadataDescriptor::SearchInFamily(lowercaseText, resultIndexes, (int)FolderData::FastSearchContext::Family); break;
    case FolderData::FastSearchContext::Path       : MetadataDescriptor::SearchInPath(lowercaseText, resultIndexes, (int)FolderData::FastSearchContext::Path); break;
    case FolderData::FastSearchContext::Description: MetadataDescriptor::SearchInDescription(lowercaseText, resultIndexes, (int)FolderData::FastSearchContext::Description); break;
    case FolderData::FastSearchContext::Developer  : MetadataDescriptor::SearchInDeveloper(lowercaseText, resultIndexes, (int)FolderData::FastSearchContext::Developer); break;
    case FolderData::FastSearchContext::Publisher  : MetadataDescriptor::SearchInPublisher(lowercaseText, resultIndexes, (int)FolderData::FastSearchContext::Publisher); break;
    case FolderData::FastSearchContext::All        :
    {
      MetadataDescriptor::SearchInNames(lowercaseText, resultIndexes, (int)FolderData::FastSearchContext::Name); break;
      MetadataDescriptor::SearchInAlias(lowercaseText, resultIndexes, (int)FolderData::FastSearchContext::Alias); break;
      MetadataDescriptor::SearchInPath(lowercaseText, resultIndexes, (int)FolderData::FastSearchContext::Path); break;
      MetadataDescriptor::SearchInDescription(lowercaseText, resultIndexes, (int)FolderData::FastSearchContext::Description); break;
      MetadataDescriptor::SearchInDeveloper(lowercaseText, resultIndexes, (int)FolderData::FastSearchContext::Developer); break;
      MetadataDescriptor::SearchInPublisher(lowercaseText, resultIndexes, (int)FolderData::FastSearchContext::Publisher); break;
      break;
    }
    default: break;
  }
  { LOG(LogDebug) << "[Search] Found " << resultIndexes.Count() << " matching file/folders"; }

  // Remove dups by index. Higher index are removed so that the lowest distances remain
  resultIndexes.Sort([](const MetadataStringHolder::IndexAndDistance& a, const MetadataStringHolder::IndexAndDistance& b) -> int { return a.Index - b.Index; });
  for(int i = resultIndexes.Count() - 1; --i >= 0;)
    if (resultIndexes(i).Index == resultIndexes(i + 1).Index)
    {
      resultIndexes(i + 1) = resultIndexes[resultIndexes.Count() - 1];
      resultIndexes.TruncateTo(resultIndexes.Count() - 1);
    }
  { LOG(LogDebug) << "[Search] " << resultIndexes.Count() << " remaining matching file/folders after removing duplicates"; }
  // Sort first: Distance
  resultIndexes.Sort([](const MetadataStringHolder::IndexAndDistance& a, const MetadataStringHolder::IndexAndDistance& b) -> int { return a.Distance - b.Distance; });

  // Build searchable system list
  Array<const SystemData*> searchableSystems((int)GetVisibleSystemList().size());
  if (targetSystem != nullptr) searchableSystems.Add(targetSystem);
  else
    for(const SystemData* system : GetVisibleSystemList())
      if (system->IsSearchable())
        searchableSystems.Add(system);
  { LOG(LogDebug) << "[Search] Lookup in " << searchableSystems.Count() << " systems"; }

  // Build Item series
  DateTime start;
  CreateFastSearchCache(resultIndexes, searchableSystems, context);
  if (FolderData::FastSearchContext::Name == context)
    CreateFastSearchCache(resultIndexes, searchableSystems, FolderData::FastSearchContext::Alias);

  { LOG(LogDebug) << "[Search] Fast lookup cache built in " << ((DateTime() - start).TotalMilliseconds()) << "ms"; }

  // Collect result
  FileData::Set results;
  for(int i = -1; ++i < (int)resultIndexes.Count(); )
  {
    const MetadataStringHolder::IndexAndDistance& resultIndex = resultIndexes(i);
    FolderData::FastSearchItemSerie& serie = mFastSearchSeries[resultIndex.Context];
    for(FolderData::FastSearchItem* item = serie.Get(resultIndex.Index); item != nullptr; item = serie.Next(item))
      if (item->Game != nullptr)
        results.insert((FileData*) item->Game);
    if ((int)results.size() >= maxglobal)
      break;
  }
  { LOG(LogDebug) << "[Search] Final result: " << results.size() << " games"; }

  FileData::List resultsList;
  for(auto& game : results)
    resultsList.push_back(game);

  return resultsList;
}

void SystemManager::SystemSorting()
{
  SystemSorting(mVisibleSystemVector, mOriginalVisibleSystemVector);
}

void SystemManager::SystemSorting(std::vector<SystemData *>& systems, const std::vector<SystemData *>& originalSystems)
{
	switch (RecalboxConf::Instance().GetSystemSorting())
  {
    case SystemSorting::Name:                                       { std::sort(systems.begin(), systems.end(), SortingName()); break; }
    case SystemSorting::ReleaseDate:                                { std::sort(systems.begin(), systems.end(), SortingReleaseDate()); break; }
    case SystemSorting::SystemTypeThenName:                         { std::sort(systems.begin(), systems.end(), Sorting1Type2Name()); break; }
    case SystemSorting::SystemTypeThenReleaseDate:                  { std::sort(systems.begin(), systems.end(), Sorting1Type2ReleaseDate()); break; }
    case SystemSorting::ManufacturerThenName:                       { std::sort(systems.begin(), systems.end(), Sorting1Manufacturer2Name()); break; }
    case SystemSorting::ManufacturerThenReleaseData:                { std::sort(systems.begin(), systems.end(), Sorting1Manufacturer2ReleaseDate()); break; }
    case SystemSorting::SystemTypeThenManufacturerThenName:         { std::sort(systems.begin(), systems.end(), Sorting1Type2Manufacturer3Name()); break; }
    case SystemSorting::SystemTypeThenManufacturerThenReleasdeDate: { std::sort(systems.begin(), systems.end(), Sorting1Type2Manufacturer3ReleaseDate()); break; }
    case SystemSorting::Default:
    default: systems = originalSystems; break;
  }
}

void SystemManager::CreateFastSearchCache(const MetadataStringHolder::FoundTextList& resultIndexes, const Array<const SystemData*>& searchableSystems, FolderData::FastSearchContext context)
{
  // Calculate searchable system hash
  unsigned int hash = 0;
  for(int i = searchableSystems.Count(); --i >= 0; )
  {
    const SystemData& system = *searchableSystems[i];
    String key = String(system.FullName().c_str()).Append((int)context);
    hash = crc32_16bytes(key.c_str(), key.size(), hash);
  }

  // Refresh hash?
  if (hash != mFastSearchCacheHash)
  {
    mFastSearchCacheHash = hash;
    { LOG(LogDebug) << "[Search] Fast cache refresh required"; }
    mFastSearchSeries.clear();
    for(int i = (int)FolderData::FastSearchContext::All + 1; --i >= 0; )
      mFastSearchSeries.push_back(FolderData::FastSearchItemSerie(0));
    for(int i = (int)resultIndexes.Count(); --i >= 0; )
      if (mFastSearchSeries[resultIndexes[i].Context].Empty())
      {
        int count = 0;
        switch((FolderData::FastSearchContext)resultIndexes[i].Context)
        {
          case FolderData::FastSearchContext::Path: count = MetadataDescriptor::FileIndexCount(); break;
          case FolderData::FastSearchContext::Name: count = MetadataDescriptor::NameIndexCount(); break;
          case FolderData::FastSearchContext::Alias: count = MetadataDescriptor::AliasIndexCount(); break;
          case FolderData::FastSearchContext::Family: count = MetadataDescriptor::FamilyIndexCount(); break;
          case FolderData::FastSearchContext::Description: count = MetadataDescriptor::DescriptionIndexCount(); break;
          case FolderData::FastSearchContext::Developer: count = MetadataDescriptor::DeveloperIndexCount(); break;
          case FolderData::FastSearchContext::Publisher: count = MetadataDescriptor::PublisherIndexCount(); break;
          case FolderData::FastSearchContext::All:break;
        }
        FolderData::FastSearchItemSerie serie(count);
        for(int s = searchableSystems.Count(); --s >= 0; )
          searchableSystems[s]->BuildFastSearchSeries(serie, (FolderData::FastSearchContext) resultIndexes[i].Context);
        mFastSearchSeries[resultIndexes[i].Context] = std::move(serie);
      }
  }
}

void SystemManager::DeleteFastSearchCache()
{
  mFastSearchSeries.clear();
}

void SystemManager::NotifyDeviceUnmount(const DeviceMount& mountpoint)
{
  for(const SystemData* system : mAllSystemVector)
    for(const RootFolderData* root : system->MasterRoot().SubRoots())
      if (root->RomPath().StartWidth(mountpoint.MountPoint()))
        if (root->HasGame())
        {
          { LOG(LogWarning) << "[SystemManager] " << mountpoint.MountPoint().ToString() << " used at least in " << system->FullName(); }
          mRomFolderChangeNotificationInterface.RomPathRemoved(mountpoint);
          return;
        }
}

void SystemManager::NotifyDeviceMount(const DeviceMount& mountpoint)
{
  Path romPath;
  switch(CheckMountPoint(mountpoint, romPath))
  {
    case RomStructure::None:
    {
      { LOG(LogWarning) << "[SystemManager] " << mountpoint.MountPoint().ToString() << " does not contain any known rom folder"; }
      mRomFolderChangeNotificationInterface.NoRomPathFound(mountpoint);
      break;
    }
    case RomStructure::Filled:
    {
      { LOG(LogInfo) << "[SystemManager] " << mountpoint.MountPoint().ToString() << " contains rom folder " << romPath.ToString(); }
      mRomFolderChangeNotificationInterface.RomPathAdded(mountpoint);
      break;
    }
    case RomStructure::Empty:
    {
      { LOG(LogInfo) << "[SystemManager] " << mountpoint.MountPoint().ToString() << " contains empty rom folder " << romPath.ToString(); }
      break;
    }
    default: break;
  }
}

bool SystemManager::HasFileWithExt(const Path& path, HashSet<std::string>& extensionSet)
{
  DIR* dir = opendir(path.ToChars());
  if (dir == nullptr) return false;
  const struct dirent* entry = nullptr;
  bool found = false;
  while(!found && (entry = readdir(dir)) != nullptr)
  {
    // File and extension in list?
    if (entry->d_type == DT_REG)
      found = (extensionSet.contains(Strings::ToLowerASCII(Path(entry->d_name).Extension())));
    else if (entry->d_type == DT_DIR)
      if (entry->d_name[0] != '.')
        found = HasFileWithExt(path / entry->d_name, extensionSet);
  }
  closedir(dir);

  return found;
}

SystemManager::RomStructure SystemManager::CheckMountPoint(const DeviceMount& root, Path& outputRomPath)
{
  static const Path pathes[] =
  {
    Path("recalbox/roms"),
    Path("roms"),
    Path(""),
  };

  { LOG(LogDebug) << "[SystemManager] Analysing mount point: " << root.MountPoint().ToString(); }
  RomStructure result = RomStructure::None;

  // Check known inner path
  for(const Path& romPath : pathes)
  {
    Path main = root.MountPoint() / romPath;
    if (main.Exists())
      for(const Path& rawPath : mAllDeclaredSystemRomPathes) // Then check system path
        if (Strings::Contains(rawPath.ToString(), sRootTag)) // Ignore absolute path
        {
          Path systemPath(Strings::Replace(rawPath.ToString(), sRootTag, main.ToString()));
          if (systemPath.Exists())
          {
            //LOG(LogDebug) << "[SystemManager] " << systemPath.ToString() << " found.";
            result = RomStructure::Empty;
            if (HasFileWithExt(systemPath, mAllDeclaredSystemExtensionSet))
            {
              { LOG(LogInfo) << "[SystemManager] " << systemPath.ToString() << " identified as a valid rom folder"; }
              outputRomPath = main;
              return RomStructure::Filled;
            }
            else { LOG(LogInfo) << "[SystemManager] " << systemPath.ToString() << " is a valid but empty rom folder"; }
          }
        }
  }

  return result;
}

void SystemManager::InitializeMountPoints()
{
  { LOG(LogDebug) << "[SystemManager] Initialize mount points"; }

  Path romPath;
  for(const DeviceMount& root : mMountPointMonitoring.MountPoints())
    if (CheckMountPoint(root, romPath) == RomStructure::Filled)
    {
      mMountPoints.push_back(romPath);
      { LOG(LogDebug) << "[SystemManager] Got: " << romPath.ToString() << " as a valid rom folder";  }
    }
}

bool SystemManager::CreateRomFoldersIn(const DeviceMount& device)
{
  Path shareUpgrade(sShareUpgradeRomRoot);
  { LOG(LogDebug) << "[SystemManager] Initializing USB device rom structure: " << device.MountPoint().ToString();  }
  Path::PathList romFolders = shareUpgrade.GetDirectoryContent();
  // Create share folder
  bool error = device.ReadOnly();
  if (!error)
    for(const Path& romFolder : romFolders)
      if (romFolder.IsDirectory())
      {
        Path destination = device.MountPoint() / "recalbox/roms" / romFolder.Filename();
        if (error = !destination.CreatePath(); error) break;
        // Copy
        Path::PathList templates = romFolder.GetDirectoryContent();
        for(const Path& file : templates)
          if (file.Extension() == ".txt")
            if (Strings::StartsWith(file.Filename(),LEGACY_STRING("_")))
              Files::SaveFile(destination / file.Filename(), Files::LoadFile(file));
      }

  return !error;
}

FileData* SystemManager::LookupGameByFilePath(const std::string& filePath)
{
  for (const SystemData* system : GetAllSystemList())
  {
    FileData* result = system->MasterRoot().LookupGameByFilePath(filePath);
    if (result != nullptr) return result;
  }
  return nullptr;
}

int SystemManager::GameCount()
{
  int result = 0;
  for(const SystemData* system : mAllSystemVector)
    result += system->GameCount();
  return result;
}
