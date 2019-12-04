#pragma once

#include "FileSystemWatcher.h"
#include "IFileSystemWatcherNotification.h"

class NotifierBuilder
{
  public:
    NotifierBuilder()
      : mInotify(),
        mEventObserver(nullptr),
        mUnexpectedEventObserver(nullptr)
    {
    }



    void CheckAndDispatch();

    void stop() { mInotify.stop(); }

    NotifierBuilder& watchPathRecursively(const Path& path);

    NotifierBuilder& watchFile(const Path& file);

    NotifierBuilder& unwatchFile(const Path& file);

    NotifierBuilder& ignoreFileOnce(const Path& file);

    NotifierBuilder& ignoreFile(const Path& file);

    NotifierBuilder& onEvents(EventType event, IFileSystemWatcherNotification*);

    NotifierBuilder& onUnexpectedEvent(IFileSystemWatcherNotification*);

  private:
    FileSystemWatcher mInotify;
    IFileSystemWatcherNotification* mEventObserver;
    IFileSystemWatcherNotification* mUnexpectedEventObserver;
};

NotifierBuilder BuildNotifier() { return {}; }

