//
// Created by gugue_u on 12/05/2021.
//

#include <vector>
#include <utils/os/fs/Path.h>
#include <utils/Files.h>
#include <systems/SystemData.h>
#include "GameFilesUtils.h"
#include "FileData.h"
#include "utils/cplusplus/StaticLifeCycleControler.h"
#include <views/ViewController.h>

HashSet<std::string> GameFilesUtils::GetGameSubFiles(FileData& game)
{
  HashSet<std::string> list;
  if (game.IsGame())
  {
    ExtractUselessFiles(game.RomPath(), list);
  }
  return list;
}

HashSet<std::string> GameFilesUtils::GetGameSaveFiles(FileData& game)
{
  HashSet<std::string> list;
  Path directory = Path("/recalbox/share/saves") / game.System().Name();

  if (game.IsGame())
  {
    for (const auto& path: directory.GetDirectoryContent())
    {
      if (path.FilenameWithoutExtension() == game.RomPath().FilenameWithoutExtension())
      {
        AddIfExist(path, list);
        // for next savestate screenshot feat
        AddIfExist(path.ChangeExtension(path.Extension() + ".png"), list);
      }
    }
    return list;
  }
  return list;
}

std::list<SaveState> GameFilesUtils::GetGameSaveStateFiles(FileData& game)
{
  std::list<SaveState> list;
  Path directory = Path("/recalbox/share/saves") / game.System().Name();

  if (game.IsGame())
  {
    for (const auto& path: directory.GetDirectoryContent())
    {
      if (path.FilenameWithoutExtension() == game.RomPath().FilenameWithoutExtension() && path.Extension().starts_with(".state"))
      {
        list.push_back(*new SaveState(path));
      }

      if (path.FilenameWithoutExtension() == game.RomPath().FilenameWithoutExtension() + ".state" && path.Extension() == (".auto"))
      {
        list.push_back(*new SaveState(path));
      }
    }

  }
  return list;
}

HashSet<std::string> GameFilesUtils::GetGameExtraFiles(FileData& fileData)
{
  HashSet<std::string> list;
  const Path path = fileData.RomPath();
  if (fileData.IsGame())
  {
    for (const auto& file: path.Directory().GetDirectoryContent())
    {
      if (file.FilenameWithoutExtension() == path.FilenameWithoutExtension())
      {
        std::string extension = Strings::ToLowerUTF8(file.Extension());

        if (Strings::Contains(extension, ".ups") || Strings::Contains(extension, ".bps") ||
            Strings::Contains(extension, ".ips"))
          list.insert(path.ToString());
      }
    }

    AddIfExist(fileData.P2KPath(), list);
    AddIfExist(fileData.RecalboxConfPath(), list);

    // retroarch override conf by game
    //core configs
    for (const Path& configCorePath: Path("/recalbox/share/system/.config/retroarch/config").GetDirectoryContent())
    {
      if (configCorePath.IsDirectory())
      {
        for (const Path& subPath: configCorePath.GetDirectoryContent())
        {
          if (subPath.IsFile() && subPath.FilenameWithoutExtension() == fileData.RomPath().FilenameWithoutExtension())
            AddIfExist(subPath, list);
        }
      }
    }

    //remap configs
    for (const Path& remapCorePath: Path("/recalbox/share/system/.config/retroarch/config/remaps/").GetDirectoryContent())
    {
      if (remapCorePath.IsDirectory())
      {
        for (const Path& subPath: remapCorePath.GetDirectoryContent())
        {
          if (subPath.IsFile() && subPath.FilenameWithoutExtension() == fileData.RomPath().FilenameWithoutExtension())
            AddIfExist(subPath, list);
        }
      }
    }
  }

  return list;
}


bool GameFilesUtils::HasAutoPatch(const FileData* fileData)
{
  std::list<Path> patches;
  if (!fileData->IsGame()) return false;

  Path path = fileData->RomPath();
  for (const auto& file: path.Directory().GetDirectoryContent())
    if (file.FilenameWithoutExtension() == path.FilenameWithoutExtension())
    {
      std::string extension = Strings::ToLowerUTF8(file.Extension());

      if (extension == ".ups" || extension ==".bps" ||extension == ".ips")
        return true;
    }

  return false;
}

Path GameFilesUtils::GetSubDirPriorityPatch(const FileData* fileData)
{
  if (!fileData->IsGame()) return Path("");

  Path romPath = fileData->RomPath();

  Path folder = romPath.Directory() / romPath.FilenameWithoutExtension().append("-patches");
  Path::PathList subDirPatches = folder.GetDirectoryContent();

  for (const auto& file: subDirPatches)
  {
    std::string extension = Strings::ToLowerUTF8(file.Extension());

    if (extension == ".ups")
      return file;
  }
  for (const auto& file: subDirPatches)
  {
    std::string extension = Strings::ToLowerUTF8(file.Extension());

    if (extension == ".bps")
      return file;
  }
  for (const auto& file: subDirPatches)
  {
    std::string extension = Strings::ToLowerUTF8(file.Extension());

    if (extension == ".ips")
      return file;
  }

  if (subDirPatches.size() == 1)
  for (const auto& file: subDirPatches)
  {
    std::string extension = Strings::ToLowerUTF8(file.Extension());

    if (extension == ".ups" || extension == ".bps" || extension == ".ips")
      return file;
  }

  return Path("");
}



std::list<Path> GameFilesUtils::GetSoftPatches(const FileData* fileData)
{
  std::list<Path> patches;
  if (!fileData->IsGame()) return patches;

  Path path = fileData->RomPath();
  Path folder = path.Directory() / path.FilenameWithoutExtension().append("-patches");
  for (const auto& file: folder.GetDirectoryContent())
  {
    std::string extension = Strings::ToLowerUTF8(file.Extension());

    if (extension == ".ups" || extension ==".bps" ||extension == ".ips")
      patches.insert(patches.begin(), file);
  }

  for (const auto& file: path.Directory().GetDirectoryContent())
    if (file.FilenameWithoutExtension() == path.FilenameWithoutExtension())
    {
      std::string extension = Strings::ToLowerUTF8(file.Extension());

      if (extension == ".ups" || extension ==".bps" ||extension == ".ips")
        patches.insert(patches.begin(), file);
    }

  return patches;
}

HashSet<std::string> GameFilesUtils::GetMediaFiles(FileData& fileData)
{
  HashSet<std::string> list;
  if (fileData.Metadata().Image().Exists())
  {
    list.insert(fileData.Metadata().Image().ToString());
  }
  if (fileData.Metadata().Video().Exists())
  {
    list.insert(fileData.Metadata().Video().ToString());
  }
  if (fileData.Metadata().Thumbnail().Exists())
  {
    list.insert(fileData.Metadata().Thumbnail().ToString());
  }

  return list;
}

bool GameFilesUtils::IsMediaShared(FileData& fileData, const Path& mediaPath)
{

  for (const auto& other: fileData.System().getAllGames())
  {
    if (fileData.AreRomEqual(*other))
    {
      continue;
    }
    if (mediaPath == other->Metadata().Image())
      return true;
    if (mediaPath == other->Metadata().Thumbnail())
      return true;
    if (mediaPath == other->Metadata().Video())
      return true;
  }
  return false;
}

void GameFilesUtils::ExtractUselessFiles(const Path& path, HashSet<std::string>& list)
{
  const std::string extension = path.Extension();
  if (extension == ".cue")
  {
    ExtractUselessFilesFromCue(path, list);
    return;
  }
  if (extension == ".ccd")
  {
    ExtractUselessFilesFromCcd(path, list);
    return;
  }
  if (extension == ".gdi" && path.Size() <= sMaxGdiFileSize)
  {
    ExtractUselessFilesFromGdi(path, list);
    return;
  }
  if (extension == ".m3u")
  {
    ExtractUselessFilesFromM3u(path, list);
    return;
  }
}

void GameFilesUtils::ExtractUselessFilesFromCue(const Path& path, HashSet<std::string>& list)
{
  std::string file = Files::LoadFile(path);
  for (const std::string& line: Strings::Split(file, '\n'))
    if (Strings::Contains(line, "FILE") && Strings::Contains(line, "BINARY"))
    {
      Path newfile = path.Directory() / ExtractFileNameFromLine(line);
      AddIfExist(newfile, list);
    }
}

void GameFilesUtils::ExtractUselessFilesFromCcd(const Path& path, HashSet<std::string>& list)
{
  AddIfExist(path.ChangeExtension(".cue"), list);
  AddIfExist(path.ChangeExtension(".bin"), list);
  AddIfExist(path.ChangeExtension(".sub"), list);
  AddIfExist(path.ChangeExtension(".img"), list);
}

void GameFilesUtils::ExtractUselessFilesFromM3u(const Path& path, HashSet<std::string>& list)
{
  std::string file = Files::LoadFile(path);
  for (std::string line: Strings::Split(file, '\n'))
  {
    if (line.empty()) continue;

    line = Strings::Trim(line, "\r");
    Path newfile = path.Directory() / Path(line);
    AddIfExist(newfile, list);

    ExtractUselessFiles(newfile, list);
  }
}

void GameFilesUtils::ExtractUselessFilesFromGdi(const Path& path, HashSet<std::string>& list)
{
  std::string file = Files::LoadFile(path);
  for (const std::string& line: Strings::Split(file, '\n'))
  {
    AddIfExist(Path(ExtractFileNameFromLine(line)), list);
  }
}

std::string GameFilesUtils::ExtractFileNameFromLine(const std::string& line)
{
  // 1 check file name between double quotes
  const size_t strBegin = line.find_first_of('\"') + 1;
  const size_t strEnd = line.find_last_of('\"');

  std::string string = line.substr(strBegin, strEnd - strBegin);
  if (Strings::Contains(string, "."))
  {
    return string;
  }

  // 2 check every words separated by space that contains dot
  for (const std::string& word: Strings::Split(line, ' ', true))
  {
    if (Strings::Contains(word, "."))
      return word;
  }
  return "";
}

void GameFilesUtils::AddIfExist(const Path& path, HashSet<std::string>& list)
{
  if (path.Exists())
    list.insert(path.ToString());
}

void GameFilesUtils::DeleteAllFiles(FileData& fileData)
{

  HashSet<std::string> files;
  HashSet<std::string> mediaFiles = GetMediaFiles(fileData);
  files.insert(fileData.RomPath().ToString());

  for (const auto& path: GetGameExtraFiles(fileData))
  {
    files.insert(path);
  }

  for (const auto& path: GetGameSaveFiles(fileData))
  {
    files.insert(path);
  }

  for (const auto& path: GetGameSubFiles(fileData))
  {
    files.insert(path);
  }

  DeleteSelectedFiles(fileData, files, mediaFiles);
}

void
GameFilesUtils::DeleteSelectedFiles(FileData& fileData, HashSet<std::string>& paths, HashSet<std::string>& mediaPaths)
{
  SystemData& systemData = fileData.System();
  { LOG(LogDebug) << "[DELETE] Begin delete of \"" << fileData.Name() << "\" deletion"; }
  if (paths.empty() && mediaPaths.empty())
  {
    { LOG(LogDebug) << "[DELETE] no file to delete for game " << fileData.Name(); }
    return;
  }

  bool mainFileDeleted = false;
  Path gamePath = fileData.RomPath();
  Path root = fileData.TopAncestor().RomPath();
  for (const auto& path : paths)
  {
    if (path == gamePath.ToString())
      mainFileDeleted = true;
    Path(path).Delete();
    { LOG(LogDebug) << "[DELETE] Game file" << path << " has been deleted"; }
  }

  bool mediaIsDirty = false;
  for (const auto& mediaPath: mediaPaths)
  {
    Path path = Path(mediaPath);
    if (path == fileData.Metadata().Image())
    {
      fileData.Metadata().SetImagePath(Path::Empty);
      mediaIsDirty = true;
    }
    if (path == fileData.Metadata().Video().ToString())
    {
      fileData.Metadata().SetVideoPath(Path::Empty);
      mediaIsDirty = true;
    }
    if (path == fileData.Metadata().Thumbnail().ToString())
    {
      fileData.Metadata().SetThumbnailPath(Path::Empty);
      mediaIsDirty = true;
    }

    if (path.Exists() && !IsMediaShared(fileData, path))
    {
      Path(mediaPath).Delete();
      { LOG(LogDebug) << "[DELETE] Game media file" << mediaPath << " has been deleted"; }

    }
    else
    { LOG(LogDebug) << "[DELETE] Game media file not exist or is shared" << mediaPath; }

  }

  // remmove fileData
  if (mainFileDeleted)
  {
    FolderData* folder = fileData.Parent();
    folder->deleteChild(&fileData);
    DeleteFoldersRecIfEmpty(folder);

  }
  else if (mediaIsDirty)
  {
    //clean gamelist metadata
    fileData.Metadata().SetDirty();
  }

  if (!systemData.HasVisibleGame())
  {
    { LOG(LogDebug) << "[DELETE] System " << systemData.Name() << " has no more visible games"; }
    ViewController::Instance().getSystemListView().RemoveCurrentSystem();
    SystemData* prev = ViewController::Instance().getSystemListView().Prev();
    ViewController::Instance().goToSystemView(prev);
  }

  systemData.UpdateGamelistXml();
}

void GameFilesUtils::DeleteFoldersRecIfEmpty(FolderData* folderData)
{
  if (folderData->IsRoot() || folderData->HasChildren())
  {
    { LOG(LogDebug) << "[DELETE] Directory " << folderData->RomPath().ToString() << " folder is not empty or root, it cannot be deleted"; }
    return;
  }

  FolderData* parent = folderData->Parent();
  Path currentFolder = folderData->RomPath();
  currentFolder.Delete();
  parent->RemoveChild(folderData);
  { LOG(LogDebug) << "[DELETE] Directory " << currentFolder.ToString() << " is now empty and have been deleted"; }

  DeleteFoldersRecIfEmpty(parent);
}

