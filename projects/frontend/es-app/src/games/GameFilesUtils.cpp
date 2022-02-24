//
// Created by gugue_u on 12/05/2021.
//

#include <vector>
#include <utils/os/fs/Path.h>
#include <utils/Files.h>
#include <systems/SystemData.h>
#include <utils/locale/LocaleHelper.h>
#include "GameFilesUtils.h"
#include "FileData.h"
#include "utils/cplusplus/StaticLifeCycleControler.h"

std::string GameFilesUtils::GetGameDeletetext(FileData& game) {
    int gameFilesCount = 1 + (int)GetGameSubFiles(game).size();
    int mediaFilesCount = (int)GetMediaFiles(game).size();
    int extraFilesCount = (int)GetGamExtraFiles(game).size();
    int saveFilesCount = (int)GetGameSaveFiles(game).size();

    std::string text = game.DisplayName().append("\n\n")
    .append(_("GAME FILES (ROM | DISK IMAGE)"))
    .append(": x")
    .append(Strings::ToString(gameFilesCount))
    .append("\n");

    if(mediaFilesCount > 0)
        text.append(_("MEDIA FiLES"))
        .append(": x")
        .append(Strings::ToString((int)GetMediaFiles(game).size()))
        .append("\n");

    if(extraFilesCount > 0)
        text.append(_("CONF | PATCHES FILES"))
        .append(": x")
        .append(Strings::ToString((int)GetGamExtraFiles(game).size()))
        .append("\n");

    if(saveFilesCount)
        text.append(_("SAVES FILES"))
        .append(": x")
        .append(Strings::ToString((int)GetGameSaveFiles(game).size()))
        .append("\n");

    return text;
}


HashSet<std::string> GameFilesUtils::GetGameSubFiles(FileData& game)
{
  HashSet<std::string> list;
  if (game.IsGame())
  {
    ExtractUselessFiles(game.FilePath(), list);
  }
  return list;
}

HashSet<std::string> GameFilesUtils::GetGameSaveFiles(FileData& game)
{
  HashSet<std::string> list;
  Path directory = Path("/recalbox/share/saves") / game.System().Name();

  if (game.IsGame())
  {
    for (const auto& path : directory.GetDirectoryContent())
    {
      if (path.FilenameWithoutExtension() == game.FilePath().FilenameWithoutExtension())
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

HashSet<std::string> GameFilesUtils::GetGamExtraFiles(FileData& fileData)
{
  HashSet<std::string> list;
  const Path& path = fileData.FilePath();
  if (fileData.IsGame())
  {
    for (const auto& file: path.Directory().GetDirectoryContent())
    {
      if (file.FilenameWithoutExtension() == path.FilenameWithoutExtension())
      {
        std::string extension = Strings::ToLowerUTF8(file.Extension());

        if (Strings::Contains(extension, ".ups") || Strings::Contains(extension, ".bps") ||
            Strings::Contains(extension, ".ips"))
          AddIfExist(file, list);
      }
    }

    AddIfExist(fileData.P2KPath(), list);
    AddIfExist(fileData.RecalboxConfPath(), list);
  }

  return list;
}

HashSet<std::string> GameFilesUtils::GetMediaFiles(FileData& fileData)
{
  HashSet<std::string> list;
  if (fileData.Metadata().Image().Exists())
  {
      list.insert(fileData.Metadata().Image().ToString());
  }
    if (fileData.Metadata().Video().Exists()) {
        list.insert(fileData.Metadata().Video().ToString());
    }
    if (fileData.Metadata().Thumbnail().Exists()) {
        list.insert(fileData.Metadata().Thumbnail().ToString());
    }

  return list;
}

bool GameFilesUtils::IsMediaShared(FileData& fileData, const Path& mediaPath)
{

  for (const auto& other : fileData.System().getGames())
  {
    if (fileData.FilePath() == other->FilePath())
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
  for (const std::string& line : Strings::Split(file, '\n'))
    if (Strings::Contains(line, "FILE") && Strings::Contains(line, "BINARY"))
    {
      Path file = path.Directory() / ExtractFileNameFromLine(line);
      AddIfExist(file, list);

      AddIfExist(path.ChangeExtension(".sbi"), list);

    }
}

void GameFilesUtils::ExtractUselessFilesFromCcd(const Path& path, HashSet<std::string>& list)
{
  std::string file = path.FilenameWithoutExtension();
  AddIfExist(Path(file + ".cue"), list);
  AddIfExist(Path(file + ".bin"), list);
  AddIfExist(Path(file + ".sub"), list);
  AddIfExist(Path(file + ".img"), list);

  AddIfExist(path.ChangeExtension(".sbi"), list);
}

void GameFilesUtils::ExtractUselessFilesFromM3u(const Path& path, HashSet<std::string>& list)
{
  std::string file = Files::LoadFile(path);
  for (std::string line : Strings::Split(file, '\n'))
  {
    if (line.empty()) continue;

    line = Strings::Trim(line, "\r");
    Path file = path.Directory() / Path(line);
    AddIfExist(file, list);
    AddIfExist(file.ChangeExtension(".sbi"), list);

    ExtractUselessFiles(file, list);
  }
}

void GameFilesUtils::ExtractUselessFilesFromGdi(const Path& path, HashSet<std::string>& list)
{
  std::string file = Files::LoadFile(path);
  for (const std::string& line : Strings::Split(file, '\n'))
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
  for (const std::string& word : Strings::Split(line, ' ', true))
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
  files.insert(fileData.FilePath().ToString());

  for (const auto& path : GetGamExtraFiles(fileData))
  {
    files.insert(path);
  }

  for (const auto& path : GetGameSaveFiles(fileData))
  {
    files.insert(path);
  }

  for (const auto& path : GetGameSubFiles(fileData))
  {
    files.insert(path);
  }

  DeleteSelectedFiles(fileData, files, mediaFiles);
}

void GameFilesUtils::DeleteSelectedFiles(FileData& fileData, HashSet<std::string>& paths, HashSet<std::string>& mediaPaths)
{
  { LOG(LogDebug) << "[DELETE] Begin delete of \"" << fileData.Name() << "\" deletion"; }

  bool mainFileDeleted = false;
  Path gamePath = fileData.FilePath();
  Path root = fileData.TopAncestor().FilePath();
  for (const auto& path : paths)
  {
    if (path== gamePath.ToString())
      mainFileDeleted = true;
    Path(path).Delete();
    { LOG(LogDebug) << "[DELETE] Game file" << path << " has been deleted"; }
  }

  bool mediaIsDirty = false;
  for (const auto& mediaPath : mediaPaths)
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

    } else { LOG(LogDebug) << "[DELETE] Game media file not exist or is shared" << mediaPath; }

  }

  // remmove fileData
  if (mainFileDeleted)
  {
    FolderData* folder = fileData.Parent();
    folder->deleteChild(&fileData);
    DeleteFoldersRecIfEmpty(folder);

  } else if (mediaIsDirty)
  {
    //clean gamelist metadata
    fileData.Metadata().SetDirty();
  }
}

void GameFilesUtils::DeleteFoldersRecIfEmpty(FolderData* folderData)
{
  if (folderData->IsRoot() || folderData->HasChildren())
  {
    { LOG(LogDebug) << "[DELETE] Directory " << folderData->FilePath().ToString() << " folder is not empty or root, it cannot be deleted"; }
    return;
  }

  FolderData* parent = folderData->Parent();
  Path currentFolder = folderData->FilePath();
  currentFolder.Delete();
  parent->RemoveChild(folderData);
  { LOG(LogDebug) << "[DELETE] Directory " << currentFolder.ToString() << " is now empty and have been deleted"; }

  DeleteFoldersRecIfEmpty(parent);

}