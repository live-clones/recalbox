//
// Created by bkg2k on 24/11/18.
//

#include "FolderData.h"
#include "utils/Log.h"
#include "systems/SystemData.h"
#include "GameNameMapManager.h"
#include <algorithm>

#define CastFolder(f) ((FolderData*)(f))

FolderData::~FolderData()
{
  for (FileData* fd : mChildren)
  {
    delete fd;
  }
  mChildren.clear();
}

void FolderData::addChild(FileData* file, bool lukeImYourFather)
{
  assert(file->getParent() == nullptr || !lukeImYourFather);

  mChildren.push_back(file);
  if (lukeImYourFather)
    file->setParent(this);
}

void FolderData::removeChild(FileData* file)
{
  for (auto it = mChildren.begin(); it != mChildren.end(); it++)
    if(*it == file)
    {
      mChildren.erase(it);
      return;
    }
}

static bool IsMatching(const std::string& fileWoExt, const std::string& extension, const std::string& extensionList)
{
  #define sFilesPrefix "files:"
  if (!Strings::StartsWith(extensionList, LEGACY_STRING(sFilesPrefix)))
  {
    // Seek in regular extensions
    size_t extensionPos = extensionList.find(extension);
    if (extensionPos == std::string::npos) return false;
    const char* p = extensionList.data();
    return (extensionPos + extension.size() == extensionList.size()) || (p[extensionPos + extension.size()] == ' ');
  }

  // Seek complete files
  constexpr int sFilesPrefixLength = sizeof(sFilesPrefix) - 1;
  std::string file(fileWoExt); file.append(extension);
  size_t filePos = extensionList.find(Strings::ToLowerASCII(fileWoExt) + extension);
  if (filePos == std::string::npos) return false;
  const char* p = extensionList.data();
  return ((filePos == sFilesPrefixLength) || (p[filePos - 1] == ' ')) &&
         ((filePos + file.size() == extensionList.size()) || (p[filePos + file.size()] == ' '));
}

void FolderData::populateRecursiveFolder(RootFolderData& root, const std::string& originalFilteredExtensions, FileData::StringMap& doppelgangerWatcher)
{
  const Path& folderPath = getPath();
  if (!folderPath.IsDirectory())
  {
    LOG(LogWarning) << "Error - folder with path \"" << folderPath.ToString() << "\" is not a directory!";
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
    {
      LOG(LogWarning) << "Skipping infinitely recursive symlink \"" << folderPath.ToString() << "\"";
      return;
    }
  }

  // Subsystem override
  std::string filteredExtensions = originalFilteredExtensions;
  if ((folderPath / ".system.cfg").Exists())
  {
    IniFile subSystem(folderPath / ".system.cfg");
    filteredExtensions = subSystem.AsString("extensions", originalFilteredExtensions);
  }

  // special system?
  bool hasFiltering = GameNameMapManager::HasFiltering(*getSystem());
  // No extension?
  bool noExtensions = filteredExtensions.empty();

  // Keep temporary object outside the loop to avoid construction/destruction and keep memory allocated AMAP
  Path::PathList items = folderPath.GetDirectoryContent();
  for (Path& filePath : items)
  {
    // Get file
    std::string stem = filePath.FilenameWithoutExtension();
    if (stem == "gamelist") continue; // Ignore gamelist.zip/xml
    if (stem.empty()) continue;

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
          if (GameNameMapManager::IsFiltered(*getSystem(), stem))
            continue; // MAME Bios or Machine
        }
        // Get the key for duplicate detection. MUST MATCH KEYS USED IN Gamelist.findOrCreateFile - Always fullpath
        if (doppelgangerWatcher.find(filePath.ToString()) == doppelgangerWatcher.end())
        {
          FileData* newGame = new FileData(filePath, root);
          newGame->Metadata().SetDirty();
          addChild(newGame, true);
          doppelgangerWatcher[filePath.ToString()] = newGame;
        }
        isLaunchableGame = true;
      }

      //add directories that also do not match an extension as folders
      if (!isLaunchableGame && filePath.IsDirectory())
      {
        FolderData* newFolder = new FolderData(filePath, root);
        newFolder->populateRecursiveFolder(root, filteredExtensions, doppelgangerWatcher);

        //ignore folders that do not contain games
        if (newFolder->hasChildren())
        {
          const std::string& key = newFolder->getPath().ToString();
          if (doppelgangerWatcher.find(key) == doppelgangerWatcher.end())
          {
            addChild(newFolder, true);
            doppelgangerWatcher[key] = newFolder;
          }
        }
        else
          delete newFolder;
      }
    }
  }
}

int FolderData::getAllFoldersRecursively(FileData::List& to) const
{
  int gameCount = 0;
  for (FileData* fd : mChildren)
  {
    if (fd->isFolder())
    {
      to.push_back(fd);
      int position = (int)to.size(); // TOOD: Check if the insert is necessary
      if (CastFolder(fd)->getAllFoldersRecursively(to) > 1)
        to.insert(to.begin() + position, fd); // Include folders iif it contains more than one game.
    }
    else if (fd->isGame())
    {
      gameCount++;
    }
  }
  return gameCount;
}

FileData::List FolderData::getAllFolders() const
{
  FileData::List result;
  getAllFoldersRecursively(result);
  return result;
}

void FolderData::ClearSubChildList()
{
  for (int i = mChildren.size(); --i >= 0; )
  {
    FileData* fd = mChildren[i];
    if (fd->isFolder())
      CastFolder(fd)->ClearSubChildList();
    else
      mChildren[i] = nullptr;
  }
}

void FolderData::BuildDoppelgangerMap(FileData::StringMap& doppelganger, bool includefolder) const
{
  for (FileData* fd : mChildren)
  {
    if (fd->isFolder())
    {
      CastFolder(fd)->BuildDoppelgangerMap(doppelganger, includefolder);
      if (includefolder)
        doppelganger[fd->getPath().ToString()] = fd;
    }
    else
      doppelganger[fd->getPath().ToString()] = fd;
  }
}

int FolderData::getItemsRecursively(FileData::List& to, IFilter* filter, bool includefolders, bool includeadult) const
{
  int gameCount = 0;
  for (FileData* fd : mChildren)
  {
    if (fd->isFolder())
    {
      if (CastFolder(fd)->getItemsRecursively(to, filter, includefolders, includeadult) > 1)
        if (includefolders)
          to.push_back(fd); // Include folders iif it contains more than one game.
    }
    else if (fd->isGame())
    {
      if (filter->ApplyFilter(*fd) && (includeadult || !fd->Metadata().Adult()))
      {
        to.push_back(fd);
        gameCount++;
      }
    }
  }
  return gameCount;
}

int FolderData::getItemsRecursively(FileData::List& to, Filter includes, bool includefolders, bool includeadult) const
{
  int gameCount = 0;
  for (FileData* fd : mChildren)
  {
    if (fd->isFolder())
    {
      if (CastFolder(fd)->getItemsRecursively(to, includes, includefolders, includeadult) > 1)
        if (includefolders)
          to.push_back(fd); // Include folders iif it contains more than one game.
    }
    else if (fd->isGame())
    {
      Filter current = Filter::None;
      if (fd->Metadata().Hidden()) current |= Filter::Hidden;
      if (fd->Metadata().Favorite()) current |= Filter::Favorite;
      if (current == 0) current = Filter::Normal;
      if ((current & includes) != 0 && (includeadult || !fd->Metadata().Adult()))
      {
        to.push_back(fd);
        gameCount++;
      }
    }
  }
  return gameCount;
}

int FolderData::countItemsRecursively(Filter includes, bool includefolders, bool includeadult) const
{
  int result = 0;
  for (FileData* fd : mChildren)
  {
    if (fd->isFolder())
    {
      int subCount = CastFolder(fd)->countItemsRecursively(includes, includefolders, includeadult);
      result += subCount;
      if (subCount > 1)
        if (includefolders)
          result++; // Include folders iif it contains more than one game.
    }
    else if (fd->isGame())
    {
      Filter current = Filter::None;
      if (fd->Metadata().Hidden()) current |= Filter::Hidden;
      if (fd->Metadata().Favorite()) current |= Filter::Favorite;
      if (current == 0) current = Filter::Normal;
      if ((current & includes) != 0 && (includeadult || !fd->Metadata().Adult()))
        result++;
    }
  }
  return result;
}

bool FolderData::hasGame() const 
{
  for(FileData* fd : mChildren)
  {
    if (fd->isGame() || (fd->isFolder() && CastFolder(fd)->hasGame()))
      return true;
  }
  return false;
}

bool FolderData::hasVisibleGame() const
{
  for (FileData* fd : mChildren)
  {
    if ( (fd->isGame() && !fd->Metadata().Hidden()) || (fd->isFolder() && CastFolder(fd)->hasVisibleGame()))
      return true;
  }
  return false;
}

bool FolderData::hasVisibleGameWithVideo() const
{
    for (FileData* fd : mChildren)
    {
        if ( ((fd->isGame() && !fd->Metadata().Hidden()) && !fd->Metadata().VideoAsString().empty() &&
        fd->Metadata().Video().Exists()) || (fd->isFolder() && CastFolder(fd)->hasVisibleGameWithVideo()))
            return true;
    }
    return false;
}

int FolderData::getItems(FileData::List& to, Filter includes, bool includefolders, bool includeadult) const
{
  int gameCount = 0;
  for (FileData* fd : mChildren)
  {
    if (fd->isFolder())
    {
      FolderData* folder = CastFolder(fd);
      // Seek for isolated file
      FileData* isolatedFile = nullptr;
      while((folder->mChildren.size() == 1) && folder->mChildren[0]->isFolder()) folder = CastFolder(folder->mChildren[0]);
      if (folder->mChildren.size() == 1)
      {
        FileData* item = folder->mChildren[0];
        if (item->isGame())
        {
          Filter current = Filter::None;
          if (item->Metadata().Hidden()) current |= Filter::Hidden;
          if (item->Metadata().Favorite()) current |= Filter::Favorite;
          if (current == 0) current = Filter::Normal;
          if ((current & includes) != 0 && (includeadult || !item->Metadata().Adult()))
            isolatedFile = item;
        }
      }
      if (isolatedFile != nullptr) to.push_back(isolatedFile);
      else
      if (includefolders)
        if (folder->countItems(includes, includefolders, includeadult) > 0) // Only add if it contains at leas one game
          to.push_back(fd);
    }
    else
    {
      Filter current = Filter::None;
      if (fd->Metadata().Hidden()) current |= Filter::Hidden;
      if (fd->Metadata().Favorite()) current |= Filter::Favorite;
      if (current == 0) current = Filter::Normal;
      if ((current & includes) != 0  && (includeadult || !fd->Metadata().Adult()))
      {
        to.push_back(fd);
        gameCount++;
      }
    }
  }
  return gameCount;
}

int FolderData::countItems(Filter includes, bool includefolders, bool includeadult) const
{
  int result = 0;
  for (FileData* fd : mChildren)
  {
    if (fd->isFolder())
    {
      FolderData* folder = CastFolder(fd);
      // Seek for isolated file
      FileData* isolatedFile = nullptr;
      while((folder->mChildren.size() == 1) && folder->mChildren[0]->isFolder()) folder = CastFolder(folder->mChildren[0]);
      if (folder->mChildren.size() == 1)
      {
        FileData* item = folder->mChildren[0];
        if (item->isGame())
        {
          Filter current = Filter::None;
          if (item->Metadata().Hidden()) current |= Filter::Hidden;
          if (item->Metadata().Favorite()) current |= Filter::Favorite;
          if (current == 0) current = Filter::Normal;
          if ((current & includes) != 0 && (includeadult || !item->Metadata().Adult()))
            isolatedFile = item;
        }
      }
      if (isolatedFile != nullptr) result++;
      else
        if (includefolders)
          if (folder->countItems(includes, includefolders, includeadult) > 0) // Only add if it contains at leas one game
            result++;
    }
    else if (fd->isGame())
    {
      Filter current = Filter::None;
      if (fd->Metadata().Hidden()) current |= Filter::Hidden;
      if (fd->Metadata().Favorite()) current |= Filter::Favorite;
      if (current == 0) current = Filter::Normal;
      if ((current & includes) != 0 && (includeadult || !fd->Metadata().Adult()))
        result++;
    }
  }
  return result;
}

bool FolderData::hasDetailedData() const
{
  for (FileData* fd : mChildren)
  {
    if (fd->isFolder())
    {
      FolderData* folder = CastFolder(fd);
      if (folder->hasDetailedData())
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
    std::string filename = path.empty() ? fd->getPath().ToString() : path + '/' + fd->getPath().Filename();

    if (fd->isFolder())
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
        filename = path.empty() ? fd->getPath().FilenameWithoutExtension() : path + '/' + fd->getPath().FilenameWithoutExtension();
        if (strcasecmp(filename.c_str(), item.c_str()) == 0)
          return fd;
      }
    }
  }
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
    if ((fd->isFolder()) && recurse)
    {
      if (Contains(fd, true)) return true;
    }
    if (fd == item) return true;
  }
  return false;
}

FileData::List FolderData::getFilteredItemsRecursively(IFilter* filter, bool includefolders, bool includeadult) const
{
  FileData::List result;
  result.reserve((unsigned long)countItemsRecursively(Filter::All, includefolders, includeadult)); // Allocate once
  getItemsRecursively(result, filter, includefolders, includeadult);
  result.shrink_to_fit();

  return result;
}

FileData::List FolderData::getFilteredItemsRecursively(Filter filters, bool includefolders, bool includeadult) const
{
  FileData::List result;
  result.reserve((unsigned long)countItemsRecursively(filters, includefolders, includeadult)); // Allocate once
  getItemsRecursively(result, filters, includefolders, includeadult);

  return result;
}

FileData::List FolderData::getAllItemsRecursively(bool includefolders, bool includeadult) const
{
  FileData::List result;
  result.reserve((unsigned long)countItemsRecursively(Filter::All, includefolders, includeadult)); // Allocate once
  getItemsRecursively(result, Filter::All, includefolders, includeadult);

  return result;
}

FileData::List FolderData::getAllDisplayableItemsRecursively(bool includefolders, bool includeadult) const
{
  FileData::List result;
  result.reserve((unsigned long)countItemsRecursively(Filter::Normal | Filter::Favorite, includefolders, includeadult)); // Allocate once
  getItemsRecursively(result, Filter::Normal | Filter::Favorite, includefolders, includeadult);

  return result;
}

FileData::List FolderData::getAllFavoritesRecursively(bool includefolders, bool includeadult) const
{
  FileData::List result;
  result.reserve((unsigned long)countItemsRecursively(Filter::Favorite, includefolders, includeadult)); // Allocate once
  getItemsRecursively(result, Filter::Favorite, includefolders, includeadult);

  return result;
}

FileData::List FolderData::getFilteredItems(Filter filters, bool includefolders, bool includeadult) const
{
  FileData::List result;
  result.reserve((unsigned long)countItems(filters, includefolders, includeadult)); // Allocate once
  getItems(result, filters, includefolders, includeadult);

  return result;
}

FileData::List FolderData::getAllItems(bool includefolders, bool includeadult) const
{
  FileData::List result;
  result.reserve((unsigned long)countItems(Filter::All, includefolders, includeadult)); // Allocate once
  getItems(result, Filter::All, includefolders, includeadult);

  return result;
}

FileData::List FolderData::getAllDisplayableItems(bool includefolders, bool includeadult) const
{
  FileData::List result;
  result.reserve((unsigned long)countItems(Filter::Normal | Filter::Favorite, includefolders, includeadult)); // Allocate once
  getItems(result, Filter::Normal | Filter::Favorite, includefolders, includeadult);

  return result;
}

FileData::List FolderData::getAllFavorites(bool includefolders, bool includeadult) const
{
  FileData::List result;
  result.reserve((unsigned long)countItems(Filter::Favorite, includefolders, includeadult)); // Allocate once
  getItems(result, Filter::Favorite, includefolders, includeadult);

  return result;
}

bool FolderData::IsDirty() const
{
  for (FileData* fd : mChildren)
  {
    if (fd->isFolder() && CastFolder(fd)->IsDirty())
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
    if (remaining <= 0) return;
    else if (fd->isFolder())
      CastFolder(fd)->FastSearch(context, text, results, remaining);
    else
    if (!fd->Metadata().Hidden())
    {
      int distance = 0;
      switch(context)
      {
        case FastSearchContext::Name       : distance = FastSearchText(text, fd->getName()); break;
        case FastSearchContext::Path       : distance = FastSearchText(text, fd->getPath().ToString()); break;
        case FastSearchContext::Description: distance = FastSearchText(text, fd->Metadata().Description()); break;
        case FastSearchContext::Developer  : distance = FastSearchText(text, fd->Metadata().Developer()); break;
        case FastSearchContext::Publisher  : distance = FastSearchText(text, fd->Metadata().Publisher()); break;
        case FastSearchContext::All        :
        {
          distance = FastSearchText(text, fd->getName());
          if (distance < 0) distance = FastSearchText(text, fd->getPath().ToString());
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

int FolderData::getFoldersRecursivelyTo(FileData::List& to) const
{
  if (isTopMostRoot())
  {
    int count = 0;
    for (const RootFolderData* root : ((RootFolderData*) this)->SubRoots())
      count += root->getAllFoldersRecursively(to);
    return count;
  }

  return getAllFoldersRecursively(to);
}

int FolderData::getItemsRecursivelyTo(FileData::List& to, FileData::Filter includes, bool includefolders, bool includeadult) const
{
  if (isTopMostRoot())
  {
    int count = 0;
    for (const RootFolderData* root : ((RootFolderData*) this)->SubRoots())
      count += root->getItemsRecursively(to, includes, includefolders, includeadult);
    return count;
  }

  return getItemsRecursively(to, includes, includefolders, includeadult);
}

int FolderData::getItemsTo(FileData::List& to, FileData::Filter includes, bool includefolders, bool includeadult) const
{
  if (isTopMostRoot())
  {
    int count = 0;
    for (const RootFolderData* root : ((RootFolderData*) this)->SubRoots())
      count += root->getItems(to, includes, includefolders, includeadult);
    return count;
  }

  return getItems(to, includes, includefolders, includeadult);
}

