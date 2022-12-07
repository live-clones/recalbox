#include "SystemData.h"
#include <systems/SystemManager.h>
#include "audio/AudioManager.h"
#include "utils/Log.h"
#include "games/GameFilesUtils.h"
#include <emulators/run/NetPlayData.h>
#include <RecalboxConf.h>
#include <RootFolders.h>
#include <usernotifications/NotificationManager.h>
#include <utils/Strings.h>
#include <utils/Files.h>
#include <themes/ThemeException.h>
#include <utils/Zip.h>

SystemData::SystemData(SystemManager& systemManager, const SystemDescriptor& descriptor, Properties properties, FileSorts::Sorts fixedSort)
  : mSystemManager(systemManager),
    mDescriptor(descriptor),
    mRootOfRoot(mRootOfRoot, RootFolderData::Ownership::None, RootFolderData::Types::None, Path(), *this),
    mProperties(properties),
    mFixedSort(fixedSort)
{
}

void SystemData::populateFolder(RootFolderData& root, FileData::StringMap& doppelgangerWatcher)
{
  { LOG(LogInfo) << "[Gamelist] " << root.System().FullName() << ": Searching games/roms in " << root.RomPath().ToString() << "..."; }

  try
  {
    std::string ignoreList(1, ','); ignoreList.append(mDescriptor.IgnoredFiles()).append(1, ',');
    root.PopulateRecursiveFolder(root, Strings::ToLowerASCII(mDescriptor.Extension()), ignoreList, doppelgangerWatcher);
  }
  catch (std::exception& ex)
  {
    { LOG(LogError) << "[Gamelist] Reading folder \"" << root.RomPath().ToString() << "\" has raised an error!"; }
    { LOG(LogError) << "[Gamelist] Exception: " << ex.what(); }
  }
}

Path SystemData::getGamelistPath(const RootFolderData& root, bool forWrite)
{
  bool zip = RecalboxConf::Instance().AsBool("emulationstation.zippedgamelist", false);
  Path filePath = root.RomPath() / (zip ? "gamelist.zip" : "gamelist.xml");

  if (forWrite) // Write mode, ensure folder exist
  {
    if (!filePath.Directory().Exists())
      filePath.Directory().CreatePath();
  }
  else if (!filePath.Exists()) // Read mode. Try selected mode first, the fallback to the other mode
  {
    Path otherFilePath = root.RomPath() / (zip ? "gamelist.xml" : "gamelist.zip");
    if (otherFilePath.Exists()) return otherFilePath;
  }

  return filePath;
}

Path SystemData::getThemePath() const
{
  // where we check for themes, in order:
  // 1. [SYSTEM_PATH]/theme.xml
  // 2. system theme from currently selected theme set [CURRENT_THEME_PATH]/[SYSTEM]/theme.xml
  // 3. default system theme from currently selected theme set [CURRENT_THEME_PATH]/theme.xml

  // try system theme in theme sets
  Path localThemePath = ThemeData::getThemeFromCurrentSet(mDescriptor.ThemeFolder());
  if (localThemePath.Exists()) return localThemePath;

  // not system theme, try default system theme in theme set
  localThemePath = localThemePath.Directory().Directory() / "theme.xml";
  if (localThemePath.Exists()) return localThemePath;

  // none of the above, try default
  localThemePath = localThemePath.Directory() / "default/theme.xml";
  if (localThemePath.Exists()) return localThemePath;

  // No luck...
  return Path::Empty;
}

void SystemData::loadTheme()
{
  Path path = getThemePath();
  if (!path.Exists()) // no theme available for this platform
    return;

  try
  {
    mTheme.loadFile(ThemeFolder(), path);
  }
  catch (ThemeException& e)
  {
    { LOG(LogError) << "[Theme] " << e.what(); }
    mTheme.loadFile(ThemeFolder(), path);
  }
}

FileData* SystemData::LookupOrCreateGame(RootFolderData& topAncestor, const Path& rootPath, const Path& path, ItemType type, FileData::StringMap& doppelgangerWatcher) const
{
  if (!path.StartWidth(rootPath))
  {
    { LOG(LogError) << "[Gamelist] File path \"" << path.ToString() << "\" is outside system path \"" <<  rootPath.ToString() << "\""; }
    return nullptr;
  }

  bool isVirtual = IsVirtual();

  int itemStart = rootPath.ItemCount();
  int itemLast = path.ItemCount() - 1;
  FolderData* treeNode = &topAncestor;
  for (int itemIndex = itemStart; itemIndex <= itemLast; ++itemIndex)
  {
    // Get the key for duplicate detection. MUST MATCH KEYS USED IN populateRecursiveFolder.populateRecursiveFolder - Always fullpath
    std::string key = path.UptoItem(itemIndex);
    FileData* item = (doppelgangerWatcher.find(key) != doppelgangerWatcher.end()) ? doppelgangerWatcher[key] : nullptr;

    // Some ScummVM folder/games may create inconsistent folders
    if (!treeNode->IsFolder()) return nullptr;

    // Last path component?
    if (itemIndex == itemLast)
    {
      if (type == ItemType::Game) // Final file
      {
        FileData* game = item;
        if (game == nullptr && !isVirtual)
        {
          // Add final game
          game = new FileData(path, topAncestor);
          doppelgangerWatcher[key] = game;
          treeNode->AddChild(game, true);
        }
        // Virtual systems use the doppleganger map in a reverse way:
        // Game to insert are already in the map
        // and are added as parent-less game in the virtual system
        else if (game != nullptr && isVirtual)
        {
          // Add existing game & remove from doppleganger
          treeNode->AddChild(game, false);
          doppelgangerWatcher.erase(key);
        }
        return game;
      }
      else // Final folder (scraped obviously)
      {
        FolderData* folder = (FolderData*) item;
        if (folder == nullptr)
        {
          // Create missing folder in both case, virtual or not
          folder = new FolderData(Path(key), topAncestor);
          doppelgangerWatcher[key] = folder;
          treeNode->AddChild(folder, true);
        }
        return folder;
      }
    }
    else // Intermediate path
    {
      FolderData* folder = (FolderData*) item;
      if (folder == nullptr)
      {
        // Create missing folder in both case, virtual or not
        folder = new FolderData(Path(key), topAncestor);
        doppelgangerWatcher[key] = folder;
        treeNode->AddChild(folder, true);
      }
      treeNode = folder;
    }
  }

  return nullptr;
}

void SystemData::ParseGamelistXml(RootFolderData& root, FileData::StringMap& doppelgangerWatcher, bool forceCheckFile)
{
  try
  {
    Path xmlpath = getGamelistPath(root, false);
    if (!xmlpath.Exists()) return;

    XmlDocument gameList;
    if (Strings::ToLowerASCII(xmlpath.Extension()) == ".zip")
    {
      Zip zip(xmlpath);
      if (zip.Count() != 1)
      {
        { LOG(LogError) << "[Gamelist] Invalid zipped gamelist: More than one file in the archive!"; }
        return;
      }
      Path gamelist(zip.FileName(0));
      if (gamelist.Filename() != "gamelist.xml")
      {
        { LOG(LogError) << "[Gamelist] Invalid zipped gamelist: No gamelist.xml found!"; }
        return;
      }
      std::string content = zip.Content(0);
      XmlResult result = gameList.load_string(content.data());
      if (!result)
      {
        { LOG(LogError) << "[Gamelist] Could not parse " << xmlpath.ToString() << " file!"; }
        return;
      }
    }
    else
    {
      XmlResult result = gameList.load_file(xmlpath.ToChars());
      if (!result)
      {
        { LOG(LogError) << "[Gamelist] Could not parse " << xmlpath.ToString() << " file!"; }
        return;
      }
    }

    std::string ignoreList(1, ','); ignoreList.append(mDescriptor.IgnoredFiles()).append(1, ',');

    const Path relativeTo(root.RomPath());
    XmlNode games = gameList.child("gameList");
    HashSet<std::string> blacklist{};

    if (games != nullptr)
    {
      // build game subfiles blacklist
      std::string extensions = mDescriptor.Extension();
      if (GameFilesUtils::ContainsMultiDiskFile(extensions))
        for (const XmlNode fileNode: games.children())
        {
          Path path = relativeTo / Xml::AsString(fileNode, "path", "");
          GameFilesUtils::ExtractUselessFiles(path, blacklist);
        }

      for (const XmlNode fileNode: games.children())
      {
        ItemType type = ItemType::Empty;
        std::string name = fileNode.name();
        if (name == "game") type = ItemType::Game;
        else if (name == "folder") type = ItemType::Folder;
        else continue; // Unknown node

        Path path = relativeTo / Xml::AsString(fileNode, "path", "");
        if (forceCheckFile)
          if (!path.Exists())
            continue;

        // Force to hide ignored files
        const std::string fileName = path.Filename();
        int p = (int)ignoreList.find(fileName);
        if (p != (int)std::string::npos)
          if (p > 0 && ignoreList[p-1] == ',')
            if (ignoreList[p + fileName.length()] == ',')
              continue;

        if (blacklist.contains(path.ToString()))
          continue;

        FileData* file = LookupOrCreateGame(root, relativeTo, path, type, doppelgangerWatcher);
        if (file == nullptr)
        {
          { LOG(LogError) << "[Gamelist] Error finding/creating FileData for \"" << path.ToString() << "\", skipping."; }
          continue;
        }

        // load the metadata
        file->Metadata().Deserialize(fileNode, relativeTo);
      }
    }
  }
  catch (std::exception& ex)
  {
    { LOG(LogError) << "[Gamelist] System \"" << Name() << "\" has raised an error while reading its gamelist.xml!"; }
    { LOG(LogError) << "[Gamelist] Exception: " << ex.what(); }
  }
}

void SystemData::UpdateGamelistXml()
{
  //We do this by reading the XML again, adding changes and then writing it back,
  //because there might be information missing in our systemdata which would then miss in the new XML.
  //We have the complete information for every game though, so we can simply remove a game
  //we already have in the system from the XML, and then add it back from its GameData information...
  for(const RootFolderData* root : mRootOfRoot.SubRoots())
    if (!root->ReadOnly() && root->IsDirty())
      try
      {
        /*
         * Get all folder & games in a flat storage
         */
        FileData::List fileList = root->GetAllItemsRecursively(true, FileData::Filter::None);
        FileData::List folderList = root->GetAllFolders();
        // Nothing to process?
        if (fileList.empty() && !root->HasDeletedChildren())
          return;

        /*
         * Create gamelist
         */
        Path xmlReadPath = getGamelistPath(*root, false);
        XmlDocument document;
        XmlNode gameList = document.append_child("gameList");

        /*
         * Serialize folder and game nodes
         */
        Path rootPath = root->RomPath();
        for (const FileData* folder : folderList)
          folder->Metadata().Serialize(gameList, folder->RomPath(), rootPath);
        for (FileData* file : fileList)
        {
          Path path(file->RomPath());
          if (root->GetDeletedChildren().contains(path.ToString()))
            continue;
          file->Metadata().Serialize(gameList, path, rootPath);
          file->Metadata().UnsetDirty();
        }

        /*
         * Custom thread-safe writer
         */
        struct XmlWriter : public pugi::xml_writer
        {
          std::string mOutput;
          void write(const void* data, size_t size) override { mOutput.append((const char*)data, size); }
        }
        Writer;

        /*
         * Write the list.
         * At this point, we're sure at least one node has been updated (or added and updated).
         */
        Path xmlWritePath(getGamelistPath(*root, true));
        xmlWritePath.Directory().CreatePath();
        mSystemManager.AddWatcherIgnoredFiles(xmlWritePath.ToString());
        document.save(Writer);

        // Save
        if (Strings::ToLowerASCII(xmlWritePath.Extension()) == ".zip")
        {
          Zip zip(xmlWritePath, true);
          Path xmlTruePath = xmlWritePath.ChangeExtension(".xml");
          if (zip.Add(Writer.mOutput, xmlTruePath.Filename()))
          {
            xmlTruePath.Delete();
            { LOG(LogInfo) << "[Gamelist] Saved gamelist.zip for system " << FullName() << ". Updated items: " << fileList.size() << "/" << fileList.size(); }
          }
          else { LOG(LogError) << "[Gamelist] Failed to save " << xmlWritePath.ToString(); }
        }
        else
        {
          if (Files::SaveFile(xmlWritePath, Writer.mOutput))
          {
            xmlWritePath.ChangeExtension(".zip").Delete();
            { LOG(LogInfo) << "[Gamelist] Saved gamelist.xml for system " << FullName() << ". Updated items: " << fileList.size() << "/" << fileList.size(); }
          }
          else { LOG(LogError) << "[Gamelist] Failed to save " << xmlWritePath.ToString(); }
        }
      }
      catch (std::exception& e)
      {
        { LOG(LogError) << "[Gamelist] Something went wrong while saving " << FullName() << " : " << e.what(); }
      }
}

bool SystemData::IsAutoScrapable() const
{
  return (mProperties & Properties::GameInPng) != 0;
}

bool SystemData::IsFavorite() const
{
  return (mProperties & Properties::Favorite) != 0;
}

bool SystemData::IsPorts() const
{
  return (mProperties & Properties::Ports) != 0;
}

bool SystemData::IsScreenshots() const
{
    return "Screenshots" == mDescriptor.FullName();
}

bool SystemData::IsVirtual() const
{
  return (mProperties & Properties::Virtual) != 0;
}

bool SystemData::IsSelfSorted() const
{
  return (mProperties & Properties::FixedSort) != 0;
}

bool SystemData::IsAlwaysFlat() const
{
  return (mProperties & Properties::AlwaysFlat) != 0;
}

bool SystemData::IsSearchable() const
{
  return (mProperties & Properties::Searchable) != 0;
}

void SystemData::BuildDoppelgangerMap(FileData::StringMap& doppelganger, bool includefolder) const
{
  for(const RootFolderData* root : mRootOfRoot.SubRoots())
    root->BuildDoppelgangerMap(doppelganger, includefolder);
}

void SystemData::UpdateLastPlayedGame(FileData& updated)
{
  // Update game
  updated.Metadata().SetLastPlayedNow();

  // Build the doppelganger map
  FileData::StringMap map;
  BuildDoppelgangerMap(map, false);
  // If the game is already here, exit
  Path path(updated.RomPath());
  if (map.contains(path.ToString())) return;

  // Prepare a one game map
  map.clear();
  map[path.ToString()] = &updated;
  // Add the virtual game
  RootFolderData* root = GetRootFolder(RootFolderData::Types::Virtual);
  if (root != nullptr)
    LookupOrCreateGame(*root, updated.TopAncestor().RomPath(), path, ItemType::Game, map);
}

RootFolderData* SystemData::GetRootFolder(RootFolderData::Types type)
{
  for(RootFolderData* rootFolder : mRootOfRoot.SubRoots())
    if (rootFolder->RootType() == type)
      return rootFolder;
  return nullptr;
}

RootFolderData* SystemData::GetRootFolder(const Path& root)
{
  for(RootFolderData* rootFolder : mRootOfRoot.SubRoots())
    if (rootFolder->RomPath() == root)
      return rootFolder;
  return nullptr;
}

RootFolderData& SystemData::CreateRootFolder(const Path& startpath, RootFolderData::Ownership childownership, RootFolderData::Types type)
{
  RootFolderData* newRoot = new RootFolderData(mRootOfRoot, childownership, type, startpath, *this);
  mRootOfRoot.AddSubRoot(newRoot);
  return *newRoot;
}

RootFolderData& SystemData::LookupOrCreateRootFolder(const Path& startpath, RootFolderData::Ownership childownership,
                                                    RootFolderData::Types type)
{
  RootFolderData* lookup = GetRootFolder(startpath);
  if (lookup != nullptr) return *lookup;

  return CreateRootFolder(startpath, childownership, type);
}

FolderData& SystemData::GetFavoriteRoot()
{
  if (!IsFavorite()) { LOG(LogError) << "[System] Virtual Root requested on NON-FAVORITE SYSTEM!"; }
  return LookupOrCreateRootFolder(Path(), RootFolderData::Ownership::None, RootFolderData::Types::Virtual);
}

bool SystemData::HasGame() const
{
  for(const RootFolderData* root : mRootOfRoot.SubRoots())
    if (root->HasGame())
      return true;
  return false;
}

int SystemData::GameCount() const
{
  int result = 0;
  for(const RootFolderData* root : mRootOfRoot.SubRoots())
    result += root->CountAll(false, FileData::Filter::None);
  return result;
}

int SystemData::GameAndFolderCount() const
{
  int result = 0;
  FileData::Filter excludes = Excludes();
  for(const RootFolderData* root : mRootOfRoot.SubRoots())
    result += root->CountAll(true, excludes);
  return result;
}

int SystemData::FavoritesCount() const
{
  int result = 0;
  FileData::Filter excludes = Excludes();
  for(const RootFolderData* root : mRootOfRoot.SubRoots())
    result += root->CountAllFavorites(false, excludes);
  return result;
}

int SystemData::HiddenCount() const
{
  int result = 0;
  FileData::Filter excludes = Excludes();
  for(const RootFolderData* root : mRootOfRoot.SubRoots())
    result += root->CountAllHidden(false, excludes);
  return result;
}

FileData::List SystemData::getFavorites() const
{
  FileData::Filter filter = FileData::Filter::Favorite;
  FileData::Filter excludes = Excludes();
  FileData::List result;
  for(const RootFolderData* root : mRootOfRoot.SubRoots())
    root->GetItemsRecursivelyTo(result, filter, excludes, false);
  return result;
}

FileData::List SystemData::getGames() const
{
  FileData::Filter filter = FileData::Filter::Normal | FileData::Filter::Favorite;
  FileData::Filter excludes = Excludes();

  FileData::List result;
  for(const RootFolderData* root : mRootOfRoot.SubRoots())
  root->GetItemsRecursivelyTo(result, filter, excludes, false);
  return result;
}

FileData::List SystemData::getAllGames() const
{
  FileData::List result;
  for(const RootFolderData* root : mRootOfRoot.SubRoots())
    root->GetItemsRecursivelyTo(result, FileData::Filter::All, FileData::Filter::None, false);
  return result;
}

bool SystemData::HasVisibleGame() const
{
  for(const RootFolderData* root : mRootOfRoot.SubRoots())
    if (root->HasVisibleGame()) return true;

  return false;
}

bool SystemData::HasScrapableGame() const
{
  for(const RootFolderData* root : mRootOfRoot.SubRoots())
    if (root->HasSacrapableGame()) return true;

  return false;
}

Path::PathList SystemData::WritableGamelists()
{
  Path::PathList result;
  for(const RootFolderData* root : mRootOfRoot.SubRoots())
    if (root->Normal())
      result.push_back(getGamelistPath(*root, true));
  return result;
}

FileData::List SystemData::getFolders() const
{
  FileData::List result;
  for(const RootFolderData* root : mRootOfRoot.SubRoots())
    root->GetFoldersRecursivelyTo(result);
  return result;
}

FileData::List SystemData::getTopGamesAndFolders() const
{
  FileData::List result;
  FileData::Filter excludes = Excludes();
  for(const RootFolderData* root : mRootOfRoot.SubRoots())
    root->GetItemsTo(result, FileData::Filter::All, excludes, true);
  return result;
}

bool SystemData::IncludeAdultGames() const
{
  return !RecalboxConf::Instance().GetSystemFilterAdult(*this);
}

FileData::Filter SystemData::Excludes() const
{
  RecalboxConf& conf = RecalboxConf::Instance();

  // Default excludes filter
  FileData::Filter excludesFilter = FileData::Filter::None;
  // ignore hidden?
  if (!conf.GetShowHidden())
    excludesFilter |= FileData::Filter::Hidden;
  // ignore non Latest version
  if (conf.GetShowOnlyLatestVersion())
    excludesFilter |= FileData::Filter::NotLatest;
  // ignore pre installed games ?
  if (conf.GetGlobalHidePreinstalled())
    excludesFilter |= FileData::Filter::PreInstalled;
  // ignore adult games ?
  if(!IncludeAdultGames())
    excludesFilter |= FileData::Filter::Adult;
  // ignore no games ?
  if(conf.GetHideNoGames())
    excludesFilter |= FileData::Filter::NoGame;
  return excludesFilter;
}

void SystemData::LookupGames(FolderData::FastSearchContext context, const MetadataStringHolder::IndexAndDistance& index, FileData::List& games) const
{
  for(const RootFolderData* root : mRootOfRoot.SubRoots())
    switch(context)
    {
      case FolderData::FastSearchContext::Path: root->LookupGamesFromPath(index, games); break;
      case FolderData::FastSearchContext::Name: root->LookupGamesFromName(index, games); break;
      case FolderData::FastSearchContext::Description: root->LookupGamesFromDescription(index, games); break;
      case FolderData::FastSearchContext::Developer: root->LookupGamesFromDeveloper(index, games); break;
      case FolderData::FastSearchContext::Publisher: root->LookupGamesFromPublisher(index, games); break;
      case FolderData::FastSearchContext::All: root->LookupGamesFromAll(index, games); break;
      default: break;
    }
}

void SystemData::BuildFastSearchSeries(FolderData::FastSearchItemSerie& into, FolderData::FastSearchContext context) const
{
  for(const RootFolderData* root : mRootOfRoot.SubRoots())
    switch(context)
    {
      case FolderData::FastSearchContext::Path: root->BuildFastSearchSeriesPath(into); break;
      case FolderData::FastSearchContext::Name: root->BuildFastSearchSeriesName(into); break;
      case FolderData::FastSearchContext::Description: root->BuildFastSearchSeriesDescription(into); break;
      case FolderData::FastSearchContext::Developer: root->BuildFastSearchSeriesDeveloper(into); break;
      case FolderData::FastSearchContext::Publisher: root->BuildFastSearchSeriesPublisher(into); break;
      case FolderData::FastSearchContext::All:
      default: break;
    }
}
