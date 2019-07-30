//
// Created by bkg2k on 24/11/18.
//

#include "FolderData.h"
#include "Log.h"
#include "SystemData.h"
#include "MameNameMap.h"
#include "boost/filesystem.hpp"

namespace fs = boost::filesystem;

#define CastFolder(f) static_cast<FolderData*>(f)

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

void FolderData::populateRecursiveFolder(const std::string& filteredExtensions, SystemData* systemData, FileData::StringMap& doppelgangerWatcher)
{
  const fs::path& folderPath = getPath();
  if(!fs::is_directory(folderPath))
  {
    LOG(LogWarning) << "Error - folder with path \"" << folderPath << "\" is not a directory!";
    return;
  }

  const std::string folderStr = folderPath.generic_string();

  //make sure that this isn't a symlink to a thing we already have
  if(fs::is_symlink(folderPath))
  {
    //if this symlink resolves to somewhere that's at the beginning of our path, it's gonna recurse
    std::string s = fs::canonical(folderPath).generic_string();
    if (folderStr.compare(0, s.length(), s) == 0)
    {
      LOG(LogWarning) << "Skipping infinitely recursive symlink \"" << folderPath << "\"";
      return;
    }
  }

  // Arcade system?
  bool isArcade = systemData->hasPlatformId(PlatformIds::PlatformId::ARCADE) || systemData->hasPlatformId(PlatformIds::PlatformId::NEOGEO);
  // No extension?
  bool noExtensions = filteredExtensions.empty();

  // Keep temporary object outside the loop to avoid construction/destruction and keep memory allocated AMAP
  fs::path filePath;
  std::string extension, key, stem;
  for (fs::directory_iterator end, dir(folderPath); dir != end; ++dir)
  {
    filePath = (*dir).path();
    stem = filePath.stem().string();
    if (stem.empty()) continue;

    //this is a little complicated because we allow a list of extensions to be defined (delimited with a space)
    //we first get the extension of the file itself:
    extension = filePath.extension().string();

    //fyi, folders *can* also match the extension and be added as games - this is mostly just to support higan
    //see issue #75: https://github.com/Aloshi/EmulationStation/issues/75
    bool isLaunchableGame = false;
    bool isFile = !fs::is_directory(filePath);
    bool isHidden = filePath.has_filename() ? filePath.filename().string()[0] == '.' : false;
    if (!isHidden)
      if((noExtensions && isFile) || (!extension.empty() && filteredExtensions.find(extension) != std::string::npos)  /*(std::find(searchExtensions.begin(), searchExtensions.end(), extension) != searchExtensions.end())*/)
      {
        if (isArcade)
        {
          if (std::find(mameBioses.begin(), mameBioses.end(), stem) != mameBioses.end() ||
              std::find(mameDevices.begin(), mameDevices.end(), stem) != mameDevices.end()) continue; // MAME Bios or Machine
        }
        // Get the key for duplicate detection. MUST MATCH KEYS USED IN Gamelist.findOrCreateFile - Always fullpath
        key = filePath.string();
        if (doppelgangerWatcher.find(key) == doppelgangerWatcher.end())
        {
          FileData* newGame = new FileData(key, systemData);
          newGame->Metadata().SetDirty();
          addChild(newGame, true);
          doppelgangerWatcher[key] = newGame;
        }
        isLaunchableGame = true;
      }

    //add directories that also do not match an extension as folders
    if(!isLaunchableGame && fs::is_directory(filePath))
    {
      FolderData* newFolder = new FolderData(filePath.generic_string(), systemData);
      newFolder->populateRecursiveFolder(filteredExtensions, systemData, doppelgangerWatcher);

      //ignore folders that do not contain games
      if(newFolder->hasChildren())
      {
        const std::string& key = newFolder->getPath().string();
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

int FolderData::getItemsRecursively(FileData::List& to, Filter includes, bool includefolders) const
{
  int gameCount = 0;
  for (FileData* fd : mChildren)
  {
    if (fd->isFolder())
    {
      int position = (int)to.size(); // TOOD: Check if the insert is necessary
      if (CastFolder(fd)->getItemsRecursively(to, includes, includefolders) > 1)
        if (includefolders)
          to.insert(to.begin() + position, fd); // Include folders iif it contains more than one game.
    }
    else if (fd->isGame())
    {
      Filter current = Filter::None;
      if (fd->Metadata().Hidden()) current |= Filter::Hidden;
      if (fd->Metadata().Favorite()) current |= Filter::Favorite;
      if (current == 0) current = Filter::Normal;
      if ((current & includes) != 0)
      {
        to.push_back(fd);
        gameCount++;
      }
    }
  }
  return gameCount;
}

int FolderData::countItemsRecursively(Filter includes, bool includefolders) const
{
  int result = 0;
  for (FileData* fd : mChildren)
  {
    if (fd->isFolder())
    {
      int subCount = CastFolder(fd)->countItemsRecursively(includes, includefolders);
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
      if ((current & includes) != 0)
        result++;
    }
  }
  return result;
}

bool FolderData::hasGame() const 
{
  for (FileData* fd : mChildren)
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

int FolderData::getItems(FileData::List& to, Filter includes, bool includefolders) const
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
          if ((current & includes) != 0)
            isolatedFile = item;
        }
      }
      if (isolatedFile != nullptr) to.push_back(isolatedFile);
      else
      if (includefolders)
        if (folder->countItems(includes, includefolders) > 0) // Only add if it contains at leas one game
          to.push_back(fd);
    }
    else
    {
      Filter current = Filter::None;
      if (fd->Metadata().Hidden()) current |= Filter::Hidden;
      if (fd->Metadata().Favorite()) current |= Filter::Favorite;
      if (current == 0) current = Filter::Normal;
      if ((current & includes) != 0)
      {
        to.push_back(fd);
        gameCount++;
      }
    }
  }
  return gameCount;
}

int FolderData::countItems(Filter includes, bool includefolders) const
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
          if ((current & includes) != 0)
            isolatedFile = item;
        }
      }
      if (isolatedFile != nullptr) result++;
      else
      if (includefolders)
        if (folder->countItems(includes, includefolders) > 0) // Only add if it contains at leas one game
          result++;
    }
    else if (fd->isGame())
    {
      Filter current = Filter::None;
      if (fd->Metadata().Hidden()) current |= Filter::Hidden;
      if (fd->Metadata().Favorite()) current |= Filter::Favorite;
      if (current == 0) current = Filter::Normal;
      if ((current & includes) != 0)
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
      if (!metadata.Image().empty()) return true;
      if (!metadata.Thumbnail().empty()) return true;
      if (!metadata.Description().empty()) return true;
      if (!metadata.Publisher().empty()) return true;
      if (!metadata.Developer().empty()) return true;
      if (!metadata.Genre().empty()) return true;
    }
  }
  return false;
}

FileData* FolderData::LookupGame(const std::string& item, SearchAttributes attributes, const std::string& path)
{
  // Recursively look for the game in subfolders too
  for (FileData* fd : mChildren)
  {
    std::string filename = path.empty() ? fd->getPath().filename().generic_string() : path + '/' + fd->getPath().filename().generic_string();

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
        filename = path.empty() ? fd->getPath().stem().generic_string() : path + '/' + fd->getPath().stem().generic_string();
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

void FolderData::Sort(FileData::List& items, int (*comparer)(FileData* const, FileData* const), bool ascending)
{
  if (items.size() > 1)
  {
    if (ascending)
      QuickSortAscending(items, 0, (int)items.size() - 1, comparer);
    else
      QuickSortDescending(items, 0, (int)items.size() - 1, comparer);
  }
}

void FolderData::QuickSortAscending(FileData::List& items, int low, int high, int (*comparer)(FileData* const, FileData* const))
{
  int Low = low, High = high;
  FileData* pivot = items[(Low + High) >> 1];
  do
  {
    while((*comparer)(items[Low] , pivot) < 0) Low++;
    while((*comparer)(items[High], pivot) > 0) High--;
    if (Low <= High)
    {
      FileData* Tmp = items[Low]; items[Low] = items[High]; items[High] = Tmp;
      Low++; High--;
    }
  }while(Low <= High);
  if (High > low) QuickSortAscending(items, low, High, comparer);
  if (Low < high) QuickSortAscending(items, Low, high, comparer);
}

void FolderData::QuickSortDescending(FileData::List& items, int low, int high, int (*comparer)(FileData* const, FileData* const))
{
  int Low = low, High = high;
  FileData* pivot = items[(Low + High) >> 1];
  do
  {
    while((*comparer)(items[Low] , pivot) > 0) Low++;
    while((*comparer)(items[High], pivot) < 0) High--;
    if (Low <= High)
    {
      FileData* Tmp = items[Low]; items[Low] = items[High]; items[High] = Tmp;
      Low++; High--;
    }
  }while(Low <= High);
  if (High > low) QuickSortDescending(items, low, High, comparer);
  if (Low < high) QuickSortDescending(items, Low, high, comparer);
}

bool FolderData::Contains(const FileData* item, bool recurse)
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

FileData::List FolderData::getFilteredItemsRecursively(Filter filters, bool includefolders) const
{
  FileData::List result;
  result.reserve((unsigned long)countItemsRecursively(filters, includefolders)); // Allocate once
  getItemsRecursively(result, filters, includefolders);

  return result;
}

FileData::List FolderData::getAllItemsRecursively(bool includefolders) const
{
  FileData::List result;
  result.reserve((unsigned long)countItemsRecursively(Filter::All, includefolders)); // Allocate once
  getItemsRecursively(result, Filter::All, includefolders);

  return result;
}

FileData::List FolderData::getAllDisplayableItemsRecursively(bool includefolders) const
{
  FileData::List result;
  result.reserve((unsigned long)countItemsRecursively(Filter::Normal | Filter::Favorite, includefolders)); // Allocate once
  getItemsRecursively(result, Filter::Normal | Filter::Favorite, includefolders);

  return result;
}

FileData::List FolderData::getAllFavoritesRecursively(bool includefolders) const
{
  FileData::List result;
  result.reserve((unsigned long)countItemsRecursively(Filter::Favorite, includefolders)); // Allocate once
  getItemsRecursively(result, Filter::Favorite, includefolders);

  return result;
}

FileData::List FolderData::getFilteredItems(Filter filters, bool includefolders) const
{
  FileData::List result;
  result.reserve((unsigned long)countItems(filters, includefolders)); // Allocate once
  getItems(result, filters, includefolders);

  return result;
}

FileData::List FolderData::getAllItems(bool includefolders) const
{
  FileData::List result;
  result.reserve((unsigned long)countItems(Filter::All, includefolders)); // Allocate once
  getItems(result, Filter::All, includefolders);

  return result;
}

FileData::List FolderData::getAllDisplayableItems(bool includefolders) const
{
  FileData::List result;
  result.reserve((unsigned long)countItems(Filter::Normal | Filter::Favorite, includefolders)); // Allocate once
  getItems(result, Filter::Normal | Filter::Favorite, includefolders);

  return result;
}

FileData::List FolderData::getAllFavorites(bool includefolders) const
{
  FileData::List result;
  result.reserve((unsigned long)countItems(Filter::Favorite, includefolders)); // Allocate once
  getItems(result, Filter::Favorite, includefolders);

  return result;
}
