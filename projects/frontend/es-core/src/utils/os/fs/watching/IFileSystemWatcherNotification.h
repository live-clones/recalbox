#pragma once

#include <utils/os/fs/Path.h>
#include <utils/datetime/DateTime.h>
#include "EventType.h"
#include <utils/storage/Set.h>

class IFileSystemWatcherNotification
{
  public:
    virtual void FileSystemWatcherNotification(EventType event, const Path& path, const DateTime& time) = 0;

    bool IsFileIgnored(const std::string& path)
    {
      bool isIgnored = mIgnoredFiles.contains(path);
      if(isIgnored) {
        mIgnoredFiles.erase(path);
      }

      return isIgnored;
    }

  protected:
    HashSet<std::string> mIgnoredFiles;
};