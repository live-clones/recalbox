#include "NotifierBuilder.h"

NotifierBuilder& NotifierBuilder::watchPathRecursively(const Path& path)
{
  mInotify.watchDirectoryRecursively(path);
  return *this;
}

NotifierBuilder& NotifierBuilder::watchFile(const Path& file)
{
  mInotify.watchFile(file);
  return *this;
}

NotifierBuilder& NotifierBuilder::unwatchFile(const Path& file)
{
  mInotify.unwatchFile(file);
  return *this;
}

NotifierBuilder& NotifierBuilder::ignoreFileOnce(const Path& file)
{
  mInotify.ignoreFileOnce(file);
  return *this;
}

NotifierBuilder& NotifierBuilder::ignoreFile(const Path& file)
{
  mInotify.ignoreFile(file);
  return *this;
}

NotifierBuilder& NotifierBuilder::onEvents(const EventType events, IFileSystemWatcherNotification* eventObserver)
{
  mInotify.setEventMask(events);
  mEventObserver = eventObserver;

  return *this;
}

NotifierBuilder& NotifierBuilder::onUnexpectedEvent(IFileSystemWatcherNotification* eventObserver)
{
  mUnexpectedEventObserver = eventObserver;
  return *this;
}

void NotifierBuilder::CheckAndDispatch()
{
  FileSystemEvent fileSystemEvent;
  if (!mInotify.getNextEvent(fileSystemEvent))
    return;

  EventType currentEvent = (EventType)fileSystemEvent.mMask;

  if ((mInotify.getEventMask() & currentEvent) != 0)
  {
    if (mEventObserver != nullptr)
      mEventObserver->FileSystemWatcherNotification(currentEvent, fileSystemEvent.mPath, fileSystemEvent.mEventTime);
  }
  else if (mUnexpectedEventObserver != nullptr)
    mUnexpectedEventObserver->FileSystemWatcherNotification(currentEvent, fileSystemEvent.mPath, fileSystemEvent.mEventTime);
}
