//
// Created by bkg2k on 24/11/18.
//

#include "FolderData.h"
#include "utils/Log.h"
#include "systems/SystemData.h"
#include "GameNameMapManager.h"
#include "GameFilesUtils.h"
#include <algorithm>
#include <utils/Files.h>

#define CastFolder(f) ((FolderData*)(f))

FolderData::~FolderData()
{
  for (FileData* fd : mChildren)
  {
    delete fd;
  }
  mChildren.clear();
}

void FolderData::AddChild(FileData* file, bool lukeImYourFather)
{
  assert(file->Parent() == nullptr || !lukeImYourFather);

  mChildren.push_back(file);
  if (lukeImYourFather)
    file->SetParent(this);
}

void FolderData::RemoveChild(FileData* file)
{
  for (auto it = mChildren.begin(); it != mChildren.end(); it++)
    if(*it == file)
    {
      mChildren.erase(it);
      return;
    }
}

void FolderData::deleteChild(FileData* file)
{
  mDeletedChildren.insert(file->FilePath().ToString());
  RemoveChild(file);
}

static bool IsMatching(const std::string& fileWoExt, const std::string& extension, const std::string& extensionList)
{
  #define sFilesPrefix "files:"
  if (!Strings::StartsWith(extensionList, LEGACY_STRING(sFilesPrefix)))
  {
    // Seek in regular extensions
    int start = 0;
    while(start < (int)extensionList.size())
    {
      size_t extensionPos = extensionList.find(extension, start);
      if (extensionPos == std::string::npos) return false;
      const char* p = extensionList.data();
      char endChar = p[extensionPos + extension.size()];
      if (endChar == ' ' || endChar == 0) return true;
      start += (int)(extensionPos + extension.size());
    }
    return false;
  }

  // Seek complete files
  constexpr int sFilesPrefixLength = sizeof(sFilesPrefix) - 1;
  std::string file(fileWoExt); file.append(extension); file = Strings::ToLowerASCII(file);
  size_t filePos = extensionList.find(file);
  if (filePos == std::string::npos) return false;
  const char* p = extensionList.data();
  return ((filePos == sFilesPrefixLength) || (p[filePos - 1] == ' ')) &&
         ((filePos + file.size() == extensionList.size()) || (p[filePos + file.size()] == ' '));
}

void FolderData::PopulateRecursiveFolder(RootFolderData& root, const std::string& originalFilteredExtensions, FileData::StringMap& doppelgangerWatcher)
{
  const Path& folderPath = FilePath();
  if (!folderPath.IsDirectory())
  {
    { LOG(LogWarning) << "[FolderData] Error - folder with path \"" << folderPath.ToString() << "\" is not a directory!"; }
    return;
  }

  // media folder?
  if (folderPath.FilenameWithoutExtension() == "media")
    return;

  //make sure that this isn't a symlink to a thing we already have
  if (folderPath.IsSymLink())
  {
    // if this symlink resolves to somewhere that's at the beginning of our path, it's gonna recurse
    Path canonical = folderPath.ToCanonical();
    if (folderPath.ToString().compare(0, canonical.ToString().size(), canonical.ToChars()) == 0)
    { LOG(LogWarning) << "[FolderData] Skipping infinitely recursive symlink \"" << folderPath.ToString() << "\""; return; }
  }

  // Subsystem override
  std::string filteredExtensions = originalFilteredExtensions;
  if ((folderPath / ".system.cfg").Exists())
  {
    IniFile subSystem(folderPath / ".system.cfg", false);
    filteredExtensions = subSystem.AsString("extensions", originalFilteredExtensions);
  }

  // special system?
  bool hasFiltering = GameNameMapManager::HasFiltering(System());
  // No extension?
  bool noExtensions = filteredExtensions.empty();

  // Keep temporary object outside the loop to avoid construction/destruction and keep memory allocated AMAP
  Path::PathList items = folderPath.GetDirectoryContent();

  HashSet<std::string> blacklist{};
  bool containsMultiDiskFile = GameFilesUtils::ContainsMultiDiskFile(filteredExtensions);
  if (containsMultiDiskFile)
    for(const auto& itemPath : items)
      GameFilesUtils::ExtractUselessFiles(itemPath, blacklist);

  for (Path& filePath : items)
  {
    // Get file
    std::string stem = filePath.FilenameWithoutExtension();
    if (stem == "gamelist") continue; // Ignore gamelist.zip/xml
    if (stem.empty()) continue;

    if (containsMultiDiskFile && blacklist.contains(filePath.ToString())) continue;

    // and Extension
    std::string extension = Strings::ToLowerASCII(filePath.Extension());

    //fyi, folders *can* also match the extension and be added as games - this is mostly just to support higan
    //see issue #75: https://github.com/Aloshi/EmulationStation/issues/75
    bool isLaunchableGame = false;
    if (!filePath.IsHidden())
    {
      if ((noExtensions && filePath.IsFile()) ||
          (!extension.empty() && IsMatching(stem, extension, filteredExtensions)))
      {
        if (hasFiltering)
        {
          if (GameNameMapManager::IsFiltered(System(), stem))
            continue; // MAME Bios or Machine
        }
        // Get the key for duplicate detection. MUST MATCH KEYS USED IN Gamelist.findOrCreateFile - Always fullpath
        if (doppelgangerWatcher.find(filePath.ToString()) == doppelgangerWatcher.end())
        {
          FileData* newGame = new FileData(filePath, root);
          newGame->Metadata().SetDirty();
          AddChild(newGame, true);
          doppelgangerWatcher[filePath.ToString()] = newGame;
        }
        isLaunchableGame = true;
      }

      //add directories that also do not match an extension as folders
      if (!isLaunchableGame && filePath.IsDirectory())
      {
        FolderData* newFolder = new FolderData(filePath, root);
        newFolder->PopulateRecursiveFolder(root, filteredExtensions, doppelgangerWatcher);

        //ignore folders that do not contain games
        if (newFolder->HasChildren())
        {
          const std::string& key = newFolder->FilePath().ToString();
          if (doppelgangerWatcher.find(key) == doppelgangerWatcher.end())
          {
            AddChild(newFolder, true);
            doppelgangerWatcher[key] = newFolder;
          }
        }
        else
          delete newFolder;
      }
    }
  }
}

void FolderData::ExtractUselessFiles(const Path::PathList& items, FileSet& blacklist)
{
  for (const Path& filePath : items)
  {
    const std::string extension = filePath.Extension();
    if(extension == ".cue")
    {
      ExtractUselessFilesFromCue(filePath, blacklist);
      continue;
    }
    if(extension == ".ccd")
    {
      ExtractUselessFilesFromCcd(filePath, blacklist);
      continue;
    }
    if(extension == ".gdi" && filePath.Size() <= sMaxGdiFileSize)
    {
      ExtractUselessFilesFromGdi(filePath, blacklist);
      continue;
    }
    if(extension == ".m3u")
    {
      ExtractUselessFilesFromM3u(filePath, blacklist);
      continue;
    }
  }
}

void FolderData::ExtractUselessFilesFromCue(const Path& path, FileSet& list)
{
  std::string file = Files::LoadFile(path);
  for(const std::string& line : Strings::Split(file, '\n'))
    if (Strings::Contains(line, "FILE") && Strings::Contains(line, "BINARY"))
    {
      ExtractFileNameFromLine(line, list);
    }
}

void FolderData::ExtractUselessFilesFromCcd(const Path& path, FileSet& list)
{
  std::string file = path.FilenameWithoutExtension();
  list.insert(file + ".cue");
  list.insert(file + ".bin");
  list.insert(file + ".sub");
  list.insert(file + ".img");
}

void FolderData::ExtractUselessFilesFromM3u(const Path& path, FileSet& list)
{
  std::string file = Files::LoadFile(path);
  for(std::string line : Strings::Split(file, '\n'))
  {
    line = Strings::Trim(line, "\r");
    list.insert(line);
  }
}

void FolderData::ExtractUselessFilesFromGdi(const Path& path, FileSet& list)
{
  std::string file = Files::LoadFile(path);
  for(const std::string& line : Strings::Split(file, '\n'))
  {
    ExtractFileNameFromLine(line, list);
  }
}

void FolderData::ExtractFileNameFromLine(const std::string& line, FileSet& list)
{
  // 1 check file name between double quotes
  const size_t strBegin = line.find_first_of('\"') + 1;const size_t strEnd   = line.find_last_of('\"');

  std::string string = line.substr(strBegin , strEnd - strBegin);
  if(Strings::Contains(string, "."))
  {
    list.insert(string);
    return;
  }
  
  // 2 check every words separated by space that contains dot
  for(const std::string& word : Strings::Split(line, ' ', true))
  {
    if (Strings::Contains(word, "."))
      list.insert(word);
  }
}

int FolderData::getAllFoldersRecursively(FileData::List& to) const
{
  int gameCount = 0;
  for (FileData* fd : mChildren)
  {
    if (fd->IsFolder())
    {
      to.push_back(fd);
      int position = (int)to.size(); // TOOD: Check if the insert is necessary
      if (CastFolder(fd)->getAllFoldersRecursively(to) > 1)
        to.insert(to.begin() + position, fd); // Include folders iif it contains more than one game.
    }
    else if (fd->IsGame()) gameCount++;
  }
  return gameCount;
}

FileData::List FolderData::GetAllFolders() const
{
  FileData::List result;
  getAllFoldersRecursively(result);
  return result;
}

void FolderData::ClearSubChildList()
{
  for (int i = (int)mChildren.size(); --i >= 0; )
  {
    const FileData* fd = mChildren[i];
    if (fd->IsFolder())
      CastFolder(fd)->ClearSubChildList();
    else
      mChildren[i] = nullptr;
  }
}

void FolderData::BuildDoppelgangerMap(FileData::StringMap& doppelganger, bool includefolder) const
{
  for (FileData* fd : mChildren)
  {
    if (fd->IsFolder())
    {
      CastFolder(fd)->BuildDoppelgangerMap(doppelganger, includefolder);
      if (includefolder)
        doppelganger[fd->FilePath().ToString()] = fd;
    }
    else
      doppelganger[fd->FilePath().ToString()] = fd;
  }
}

bool FolderData::HasMissingHashRecursively()
{
  for (FileData* fd : mChildren)
    if (fd->IsFolder())
    {
      if (CastFolder(fd)->HasMissingHashRecursively())
        return true;
    }
    else if (fd->IsGame())
      if (fd->Metadata().RomCrc32() == 0)
        return true;
  return false;
}

void FolderData::CalculateMissingHashRecursively()
{
  for (FileData* fd : mChildren)
    if (fd->IsFolder())
      CastFolder(fd)->CalculateMissingHashRecursively();
    else if (fd->IsGame())
      if (fd->Metadata().RomCrc32() == 0)
        fd->CalculateHash();
}

int FolderData::getMissingHashRecursively(FileData::List& to) const
{
  int gameCount = 0;
  for (FileData* fd : mChildren)
  {
    if (fd->IsFolder())
      gameCount += CastFolder(fd)->getMissingHashRecursively(to);
    else if (fd->IsGame())
      if (fd->Metadata().RomCrc32() == 0)
      {
        to.push_back(fd);
        gameCount++;
      }
  }
  return gameCount;
}

int FolderData::getItemsRecursively(FileData::List& to, IFilter* filter, bool includefolders) const
{
  int gameCount = 0;
  for (FileData* fd : mChildren)
  {
    if (fd->IsFolder())
    {
      if (CastFolder(fd)->getItemsRecursively(to, filter, includefolders) > 1)
        if (includefolders)
          to.push_back(fd); // Include folders iif it contains more than one game.
    }
    else if (fd->IsGame())
    {
      if (filter->ApplyFilter(*fd))
      {
        to.push_back(fd);
        gameCount++;
      }
    }
  }
  return gameCount;
}

int FolderData::getItemsRecursively(FileData::List& to, Filter includes, Filter excludes, bool includefolders) const
{
  int gameCount = 0;
  for (FileData* fd : mChildren)
  {
    if (fd->IsFolder())
    {
      if (CastFolder(fd)->getItemsRecursively(to, includes, excludes, includefolders) > 1)
        if (includefolders)
          to.push_back(fd); // Include folders iif it contains more than one game.
    }
    else if (fd->IsGame())
    {
      if(IsFiltered(fd, includes, excludes))
      {
        to.push_back(fd);
        gameCount++;
      }
    }
  }
  return gameCount;
}

int FolderData::countItemsRecursively(IFilter* filter, bool includefolders) const
{
  int result = 0;
  for (FileData* fd : mChildren)
  {
    if (fd->IsFolder())
    {
      int subCount = CastFolder(fd)->countItemsRecursively(filter, includefolders);
      result += subCount;
      if (subCount > 1)
        if (includefolders)
          result++; // Include folders iif it contains more than one game.
    }
    else if (fd->IsGame())
    {
      if (filter->ApplyFilter(*fd))
        result++;
    }
  }
  return result;
}

int FolderData::countItemsRecursively(Filter includes, Filter excludes, bool includefolders) const
{
  int result = 0;
  for (FileData* fd : mChildren)
  {
    if (fd->IsFolder())
    {
      int subCount = CastFolder(fd)->countItemsRecursively(includes, excludes, includefolders);
      result += subCount;
      if (subCount > 1)
        if (includefolders)
          result++; // Include folders iif it contains more than one game.
    }
    else if (fd->IsGame())
    {
      if(IsFiltered(fd, includes, excludes))
        result++;
    }
  }
  return result;
}

int FolderData::countExcludesItemsRecursively(Filter includes, Filter excludes, bool includefolders) const
{
  int result = 0;
  for (FileData* fd : mChildren)
  {
    if (fd->IsFolder())
    {
      int subCount = CastFolder(fd)->countExcludesItemsRecursively(includes, excludes, includefolders);
      result += subCount;
      if (subCount > 1)
        if (includefolders)
          result++; // Include folders if it contains more than one game.
    }
    else if (fd->IsGame())
    {
      Filter currentIncludes = Filter::None;
      if (fd->Metadata().Favorite()) currentIncludes |= Filter::Favorite;
      if (currentIncludes == 0) currentIncludes = Filter::Normal;

      Filter currentExcludes = Filter::None;
      if (fd->Metadata().Hidden()) currentExcludes |= Filter::Hidden;
      if (!fd->Metadata().LatestVersion()) currentExcludes |= Filter::NotLatest;
      if (Strings::Contains(fd->FilePath().ToString(), "share_init"))
        currentExcludes |= Filter::PreInstalled;
      if(!fd->System().IncludeAdultGames() && fd->Metadata().Adult()) currentExcludes |= Filter::Adult;

      if ((currentIncludes & includes) == 0 || (currentExcludes & excludes) != 0)
        result++;
    }
  }
  return result;
}

bool FolderData::HasGame() const
{
  for(FileData* fd : mChildren)
  {
    if (fd->IsGame() || (fd->IsFolder() && CastFolder(fd)->HasGame()))
      return true;
  }
  return false;
}

bool FolderData::HasVisibleGame() const
{
  for (FileData* fd : mChildren)
  {
    if ( (fd->IsGame() && fd->IsDisplayable()) || (fd->IsFolder() && CastFolder(fd)->HasVisibleGame()))
      return true;
  }
  return false;
}

bool FolderData::HasSacrapableGame() const
{
  for (FileData* fd : mChildren)
  {
    if ( (fd->IsGame() && !fd->TopAncestor().PreInstalled()) || (fd->IsFolder() && CastFolder(fd)->HasSacrapableGame()))
      return true;
  }
  return false;
}

bool FolderData::HasVisibleGameWithVideo() const
{
    for (FileData* fd : mChildren)
    {
        if ( ((fd->IsGame() && !fd->IsDisplayable()) && !fd->Metadata().VideoAsString().empty() &&
        fd->Metadata().Video().Exists()) || (fd->IsFolder() && CastFolder(fd)->HasVisibleGameWithVideo()))
            return true;
    }
    return false;
}

int FolderData::getItems(FileData::List& to, Filter includes, Filter excludes, bool includefolders) const
{
  int gameCount = 0;
  for (FileData* fd : mChildren)
  {
    if (fd->IsFolder())
    {
      FolderData* folder = CastFolder(fd);
      // Seek for isolated file
      FileData* isolatedFile = nullptr;
      while((folder->mChildren.size() == 1) && folder->mChildren[0]->IsFolder()) folder = CastFolder(folder->mChildren[0]);
      if (folder->mChildren.size() == 1)
      {
        FileData* item = folder->mChildren[0];
        if (item->IsGame())
        {
          if(IsFiltered(fd, includes, excludes))
            isolatedFile = item;
        }
      }
      if (isolatedFile != nullptr) to.push_back(isolatedFile);
      else
      if (includefolders)
        if (folder->countItems(includes, excludes, includefolders ) > 0) // Only add if it contains at leas one game
          to.push_back(fd);
    }
    else
    {
      if(IsFiltered(fd, includes, excludes))
      {
        to.push_back(fd);
        gameCount++;
      }
    }
  }
  return gameCount;
}

int FolderData::countItems(Filter includes, Filter excludes, bool includefolders) const
{
  int result = 0;
  for (FileData* fd : mChildren)
  {
    if (fd->IsFolder())
    {
      FolderData* folder = CastFolder(fd);
      // Seek for isolated file
      FileData* isolatedFile = nullptr;
      while((folder->mChildren.size() == 1) && folder->mChildren[0]->IsFolder()) folder = CastFolder(folder->mChildren[0]);
      if (folder->mChildren.size() == 1)
      {
        FileData* item = folder->mChildren[0];
        if (item->IsGame())
        {
          if(IsFiltered(fd, includes, excludes))
            isolatedFile = item;
        }
      }
      if (isolatedFile != nullptr) result++;
      else
        if (includefolders)
          if (folder->countItems(includes, excludes, includefolders) > 0) // Only add if it contains at leas one game
            result++;
    }
    else if (fd->IsGame())
    {
      if(IsFiltered(fd, includes, excludes))
        result++;
    }
  }
  return result;
}

void FolderData::ParseAllItems(IParser& parser)
{
  for (FileData* fd : mChildren)
  {
    parser.Parse(*fd);
    if (fd->IsFolder())
    {
      FolderData* folder = CastFolder(fd);
      folder->ParseAllItems(parser);
    }
  }
}

bool FolderData::IsFiltered(FileData* fd, FileData::Filter includes, FileData::Filter excludes)
{
  Filter currentIncludes = Filter::None;
  if (fd->Metadata().Favorite())
    currentIncludes |= Filter::Favorite;
  if (currentIncludes == 0)
    currentIncludes = Filter::Normal;

  Filter currentExcludes = Filter::None;
  if (fd->Metadata().Hidden())
    currentExcludes |= Filter::Hidden;
  if (fd->Metadata().NoGame())
    currentExcludes |= Filter::NoGame;
  if (!fd->Metadata().LatestVersion())
    currentExcludes |= Filter::NotLatest;
  if (fd->TopAncestor().PreInstalled())
    currentExcludes |= Filter::PreInstalled;
  if (!fd->System().IncludeAdultGames() && fd->Metadata().Adult())

    currentExcludes |= Filter::Adult;

  return ((currentIncludes & includes) != 0 && (currentExcludes & excludes) == 0);
}

bool FolderData::HasDetailedData() const
{
  for (FileData* fd : mChildren)
  {
    if (fd->IsFolder())
    {
      FolderData* folder = CastFolder(fd);
      if (folder->HasDetailedData())
        return true;
    }
    else
    {
      const MetadataDescriptor& metadata = fd->Metadata();
      if (!metadata.Image().IsEmpty()) return true;
      if (!metadata.Thumbnail().IsEmpty()) return true;
      if (!metadata.Description().empty()) return true;
      if (!metadata.Publisher().empty()) return true;
      if (!metadata.Developer().empty()) return true;
      if (!metadata.Genre().empty()) return true;
    }
  }
  return false;
}

FileData* FolderData::LookupGame(const std::string& item, SearchAttributes attributes, const std::string& path) const
{
  // Recursively look for the game in subfolders too
  for (FileData* fd : mChildren)
  {
    std::string filename = path.empty() ? fd->FilePath().ToString() : path + '/' + fd->FilePath().Filename();

    if (fd->IsFolder())
    {
      FolderData* folder = CastFolder(fd);
      FileData* result = folder->LookupGame(item, attributes, path);
      if (result != nullptr)
        return result;
    }
    else
    {
      if ((attributes & SearchAttributes::ByHash) != 0)
        if (fd->Metadata().RomCrc32AsString() == item)
          return fd;
      if ((attributes & SearchAttributes::ByNameWithExt) != 0)
        if (strcasecmp(filename.c_str(), item.c_str()) == 0)
          return fd;
      if ((attributes & SearchAttributes::ByName) != 0)
      {
        filename = path.empty() ? fd->FilePath().FilenameWithoutExtension() : path + '/' + fd->FilePath().FilenameWithoutExtension();
        if (strcasecmp(filename.c_str(), item.c_str()) == 0)
          return fd;
      }
    }
  }
  return nullptr;
}

FileData* FolderData::LookupGameByFilePath(const std::string& filepath) const
{
  // Recursively look for the game in subfolders too
  for (FileData* fd : mChildren)
    if (fd->IsFolder())
    {
      FolderData* folder = CastFolder(fd);
      FileData* result = folder->LookupGameByFilePath(filepath);
      if (result != nullptr)
        return result;
    }
    else
      if (filepath == fd->FilePath().ToString())
        return fd;

  return nullptr;
}

FileData* FolderData::GetNextFavoriteTo(FileData* reference)
{
  // Look for position index. If not found, start from the begining
  int position = 0;
  for (int i = (int)mChildren.size(); --i >= 0; )
    if (mChildren[i] == reference)
    {
      position = i;
      break;
    }

  // Look forward
  for (int i = position; i < (int)mChildren.size(); i++)
    if (mChildren[i]->Metadata().Favorite())
      return mChildren[i];
  // Look backward
  for (int i = position; --i >= 0; )
    if (mChildren[i]->Metadata().Favorite())
      return mChildren[i];

  return nullptr;
}

void FolderData::Sort(FileData::List& items, FileData::Comparer comparer, bool ascending)
{
  if (items.size() > 1)
  {
    if (ascending)
      QuickSortAscending(items, 0, (int)items.size() - 1, comparer);
    else
      QuickSortDescending(items, 0, (int)items.size() - 1, comparer);
  }
}

void FolderData::QuickSortAscending(FileData::List& items, int low, int high, FileData::Comparer comparer)
{
  int Low = low, High = high;
  const FileData& pivot = *items[(Low + High) >> 1];
  do
  {
    while((*comparer)(*items[Low] , pivot) < 0) Low++;
    while((*comparer)(*items[High], pivot) > 0) High--;
    if (Low <= High)
    {
      FileData* Tmp = items[Low]; items[Low] = items[High]; items[High] = Tmp;
      Low++; High--;
    }
  }while(Low <= High);
  if (High > low) QuickSortAscending(items, low, High, comparer);
  if (Low < high) QuickSortAscending(items, Low, high, comparer);
}

void FolderData::QuickSortDescending(FileData::List& items, int low, int high, FileData::Comparer comparer)
{
  int Low = low, High = high;
  const FileData& pivot = *items[(Low + High) >> 1];
  do
  {
    while((*comparer)(*items[Low] , pivot) > 0) Low++;
    while((*comparer)(*items[High], pivot) < 0) High--;
    if (Low <= High)
    {
      FileData* Tmp = items[Low]; items[Low] = items[High]; items[High] = Tmp;
      Low++; High--;
    }
  }while(Low <= High);
  if (High > low) QuickSortDescending(items, low, High, comparer);
  if (Low < high) QuickSortDescending(items, Low, high, comparer);
}

bool FolderData::Contains(const FileData* item, bool recurse) const
{
  for (FileData* fd : mChildren)
  {
    if ((fd->IsFolder()) && recurse)
    {
      if (Contains(fd, true)) return true;
    }
    if (fd == item) return true;
  }
  return false;
}

FileData::List FolderData::GetFilteredItemsRecursively(IFilter* filter, bool includefolders) const
{
  FileData::List result;
  result.reserve((unsigned long)countItemsRecursively(Filter::All, System().Excludes(), includefolders)); // Allocate once
  getItemsRecursively(result, filter, includefolders);
  result.shrink_to_fit();

  return result;
}

FileData::List FolderData::GetFilteredItemsRecursively(Filter filters, Filter excludes, bool includefolders) const
{
  FileData::List result;
  result.reserve((unsigned long)countItemsRecursively(filters, excludes, includefolders)); // Allocate once
  getItemsRecursively(result, filters, excludes, includefolders);

  return result;
}

FileData::List FolderData::GetAllItemsRecursively(bool includefolders,Filter excludes) const
{
  FileData::List result;
  result.reserve((unsigned long)countItemsRecursively(Filter::All, excludes, includefolders)); // Allocate once
  getItemsRecursively(result, Filter::All, excludes, includefolders);

  return result;
}

FileData::List FolderData::GetAllDisplayableItemsRecursively(bool includefolders, Filter excludes) const
{
  FileData::List result;
  result.reserve((unsigned long)countItemsRecursively(Filter::Normal | Filter::Favorite, excludes, includefolders)); // Allocate once
  getItemsRecursively(result, Filter::Normal | Filter::Favorite, excludes, includefolders);

  return result;
}

FileData::List FolderData::GetAllFavoritesRecursively(bool includefolders, Filter excludes) const
{
  FileData::List result;
  result.reserve((unsigned long)countItemsRecursively(Filter::Favorite, excludes, includefolders)); // Allocate once
  getItemsRecursively(result, Filter::Favorite, excludes, includefolders);

  return result;
}

FileData::List FolderData::GetFilteredItems(Filter filters, Filter excludes, bool includefolders) const
{
  FileData::List result;
  result.reserve((unsigned long)countItems(filters, excludes, includefolders)); // Allocate once
  getItems(result, filters, excludes, includefolders);

  return result;
}

FileData::List FolderData::GetAllItems(bool includefolders, Filter excludes) const
{
  FileData::List result;
  result.reserve((unsigned long)countItems(Filter::All, excludes, includefolders)); // Allocate once
  getItems(result, Filter::All, excludes ,includefolders);

  return result;
}

FileData::List FolderData::GetAllDisplayableItems(bool includefolders, Filter excludes) const
{
  FileData::List result;
  result.reserve((unsigned long)countItems(Filter::Normal | Filter::Favorite, excludes, includefolders)); // Allocate once
  getItems(result, Filter::Normal | Filter::Favorite, excludes, includefolders);

  return result;
}

FileData::List FolderData::GetAllFavorites(bool includefolders, Filter excludes) const
{
  FileData::List result;
  result.reserve((unsigned long)countItems(Filter::Favorite, excludes, includefolders)); // Allocate once
  getItems(result, Filter::Favorite, excludes, includefolders);

  return result;
}

bool FolderData::IsDirty() const
{
  if (HasDeletedChildren())
    return true;
  for (FileData* fd : mChildren)
  {
    if (fd->IsFolder() && CastFolder(fd)->IsDirty())
      return true;
    if (fd->Metadata().IsDirty())
      return true;
  }
  return false;
}

int FolderData::FastSearchText(const std::string& text, const std::string& into)
{
  const char* fdn = into.c_str();                        // Keep filedata name pointer for fast result computation
  const char* tts = text.c_str();                        // Keep text pointer for fast search reset
  int lmax = (int)into.size() - (int)text.size() + 1;    // MAximum byte to search in

  for(const char* p = fdn - 1; --lmax >= 0; ++p)         // Run through the game name, straight forward
    if ((*p | 0x20) == (*tts))                           // Try to catch the first char
      for (const char* s = tts, *ip = p; ; )             // Got it, run through both string
      {
        const char c = *(++s);
        if ((c != 0) && ((*(++ip) | 0x20) != c) ) break; // Chars are not equal, exit the inner loop
        if (c == 0) return (int)(p - fdn);               // Chars are equal, got a zero terminal? Found it!
      }
  return -1;                                             // Nothing found
}

void FolderData::FastSearch(FastSearchContext context, const std::string& text, ResultList& results, int& remaining) const
{
  for (FileData* fd : mChildren)
  {
    if (remaining <= 0) return;
    if (fd->IsFolder()) CastFolder(fd)->FastSearch(context, text, results, remaining);
    else if (fd->IsDisplayable())
    {
      int distance = 0;
      switch(context)
      {
        case FastSearchContext::Name       : distance = FastSearchText(text, fd->Name()); break;
        case FastSearchContext::Path       : distance = FastSearchText(text, fd->FilePath().ToString()); break;
        case FastSearchContext::Description: distance = FastSearchText(text, fd->Metadata().Description()); break;
        case FastSearchContext::Developer  : distance = FastSearchText(text, fd->Metadata().Developer()); break;
        case FastSearchContext::Publisher  : distance = FastSearchText(text, fd->Metadata().Publisher()); break;
        case FastSearchContext::All        :
        {
          distance = FastSearchText(text, fd->Name());
          if (distance < 0) distance = FastSearchText(text, fd->FilePath().ToString());
          if (distance < 0) distance = FastSearchText(text, fd->Metadata().Description());
          if (distance < 0) distance = FastSearchText(text, fd->Metadata().Developer());
          if (distance < 0) distance = FastSearchText(text, fd->Metadata().Publisher());
          break;
        }
      }
      if (distance >= 0)
        if (--remaining > 0)
          results.push_back(FastSearchItem(distance, fd));
    }
  }
}

int FolderData::GetFoldersRecursivelyTo(FileData::List& to) const
{
  if (IsTopMostRoot())
  {
    int count = 0;
    for (const RootFolderData* root : ((RootFolderData*) this)->SubRoots())
      count += root->getAllFoldersRecursively(to);
    return count;
  }

  return getAllFoldersRecursively(to);
}

int FolderData::GetItemsRecursivelyTo(FileData::List& to, FileData::Filter includes, FileData::Filter excludes, bool includefolders) const
{
  if (IsTopMostRoot())
  {
    int count = 0;
    for (const RootFolderData* root : ((RootFolderData*) this)->SubRoots())
      count += root->getItemsRecursively(to, includes, excludes, includefolders);
    return count;
  }

  return getItemsRecursively(to, includes, excludes, includefolders);
}

int FolderData::GetItemsTo(FileData::List& to, FileData::Filter includes, FileData::Filter excludes, bool includefolders) const
{
  if (IsTopMostRoot())
  {
    int count = 0;
    for (const RootFolderData* root : ((RootFolderData*) this)->SubRoots())
      count += root->getItems(to, includes, excludes, includefolders);
    return count;
  }

  return getItems(to, includes, excludes, includefolders);
}
