//
// Created by gugue_u on 12/05/2021.
//


#include "FileData.h"
#include <utils/storage/Set.h>
#include <views/gamelist/IGameListView.h>

class GameFilesUtils
{
  public:
    static HashSet<std::string> GetGameSubFiles(FileData& game);
    static HashSet<std::string> GetGameExtraFiles(FileData& fileData);
    static HashSet<std::string> GetGameSaveFiles(FileData& game);
    static HashSet<std::string> GetMediaFiles(FileData& fileData);

    static bool ContainsMultiDiskFile(const std::string& extensions)
    {
      return Strings::Contains(extensions, ".m3u") || Strings::Contains(extensions, ".cue") ||
             Strings::Contains(extensions, ".ccd") || Strings::Contains(extensions, ".gdi");
    }

    static void ExtractUselessFiles(const Path& path, HashSet<std::string>& list);
    static void ExtractUselessFilesFromCue(const Path& path, HashSet<std::string>& list);
    static void ExtractUselessFilesFromCcd(const Path& path, HashSet<std::string>& list);
    static void ExtractUselessFilesFromM3u(const Path& path, HashSet<std::string>& list);
    static void ExtractUselessFilesFromGdi(const Path& path, HashSet<std::string>& list);
    static std::string ExtractFileNameFromLine(const std::string& line);
    static void AddIfExist(const Path& path, HashSet<std::string>& list);
    static constexpr int sMaxGdiFileSize = (10 << 10); // 10 Kb

    static void DeleteSelectedFiles(FileData& fileData, HashSet<std::string>&, HashSet<std::string>&);
    static void DeleteAllFiles(FileData& fileData);

    static bool IsMediaShared(FileData& system, const Path& mediaPath);

    static void DeleteFoldersRecIfEmpty(FolderData* folderData);
};


