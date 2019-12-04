#include "FileSystemWatcher.h"
#include "utils/Log.h"
#include "IFileSystemWatcherNotification.h"

#include <string>
#include <sys/epoll.h>
#include <fcntl.h>
#include <zconf.h>
#include <cstring>

FileSystemWatcher::FileSystemWatcher()
  : mEventMask(EventType::All),
    mThreadSleep(250),
    mIgnoredDirectories(),
    mOnceIgnoredDirectories(),
    mEventQueue(),
    mDirectorieMap(),
    mInotifyFd(0),
    mStopped(false),
    mEpollFd(0),
    mInotifyEpollEvent(),
    mStopPipeEpollEvent(),
    mEpollEvents(),
    mEventBuffer(),
    mStopPipeFd(),
    mPipeReadIdx(0),
    mPipeWriteIdx(1)
{
  if (pipe2(mStopPipeFd, O_NONBLOCK) == -1)
    LOG(LogError) << "Can't initialize stop pipe ! " << strerror(errno) << ".";

  mInotifyFd = inotify_init1(IN_NONBLOCK);
  if (mInotifyFd == -1)
    LOG(LogError) << "Can't initialize inotify ! " << strerror(errno) << ".";

  mEpollFd = epoll_create1(0);
  if (mEpollFd == -1)
    LOG(LogError) << "Can't initialize epoll ! " << strerror(errno) << ".";

  mInotifyEpollEvent.events = EPOLLIN | EPOLLET;
  mInotifyEpollEvent.data.fd = mInotifyFd;
  if (epoll_ctl(mEpollFd, EPOLL_CTL_ADD, mInotifyFd, &mInotifyEpollEvent) == -1)
    LOG(LogError) << "Can't add inotify filedescriptor to epoll ! " << strerror(errno) << ".";

  mStopPipeEpollEvent.events = EPOLLIN | EPOLLET;
  mStopPipeEpollEvent.data.fd = mStopPipeFd[mPipeReadIdx];
  if (epoll_ctl(mEpollFd, EPOLL_CTL_ADD, mStopPipeFd[mPipeReadIdx], &mStopPipeEpollEvent) == -1)
    LOG(LogError) << "Can't add pipe filedescriptor to epoll ! " << strerror(errno) << ".";
}

FileSystemWatcher::~FileSystemWatcher()
{
  epoll_ctl(mEpollFd, EPOLL_CTL_DEL, mInotifyFd, nullptr);
  epoll_ctl(mEpollFd, EPOLL_CTL_DEL, mStopPipeFd[mPipeReadIdx], nullptr);

  if (close(mInotifyFd) == 0)
    LOG(LogError) << "Error closing notify fd.";

  if (close(mEpollFd) == 0)
    LOG(LogError) << "Error closing poll fd.";

  close(mStopPipeFd[mPipeReadIdx]);
  close(mStopPipeFd[mPipeWriteIdx]);
}

void FileSystemWatcher::watchDirectoryRecursively(const Path& path)
{
  if (path.Exists())
  {
    watchFile(path);
    if (path.IsDirectory())
    {
      Path::PathList list = path.GetDirectoryContent();
      for (const Path& p : list)
        watchDirectoryRecursively(p);
    }
  }
  else
    LOG(LogError) << "Can´t watch Path! Path does not exist. Path: " + path.ToString();
}

void FileSystemWatcher::watchFile(const Path& filePath)
{
  if (filePath.Exists())
  {
    int wd = 0;
    if (!isIgnored(filePath))
      wd = inotify_add_watch(mInotifyFd, filePath.ToChars(), (int)mEventMask);

    if (wd == -1)
    {
      int Error = errno;
      if (Error == 28)
        LOG(LogError) << "Failed to watch! " << strerror(Error) << ". Please increase number of watches in "
                         "\"/proc/sys/fs/inotify/max_user_watches\".";

      LOG(LogError) << "Failed to watch! " << strerror(Error) << ". Path: " << filePath.ToString();
    }
    mDirectorieMap.insert({ wd, filePath });
  }
  else
    LOG(LogError) << "Can´t watch Path! Path does not exist. Path: " << filePath.ToString();
}

void FileSystemWatcher::unwatchFile(const Path& file)
{
  for(auto& dir : mDirectorieMap)
    if (dir.second == file)
      removeWatch(dir.first);
}

void FileSystemWatcher::removeWatch(int wd)
{
  if (inotify_rm_watch(mInotifyFd, wd) == -1)
    LOG(LogError) << "Failed to remove watch! " << strerror(errno) << ".";
}

bool FileSystemWatcher::getNextEvent(FileSystemEvent& fsevent)
{
  std::vector<FileSystemEvent> newEvents;

  while (mEventQueue.empty() && !mStopped)
  {
    int length = readEventsIntoBuffer();
    readEventsFromBuffer(length, newEvents);
    filterEvents(newEvents);
  }

  if (mStopped)
    return false;

  fsevent = mEventQueue.front();
  mEventQueue.pop();
  return true;
}

void FileSystemWatcher::stop()
{
  mStopped = true;
  sendStopSignal();
}

void FileSystemWatcher::sendStopSignal()
{
  std::vector<std::uint8_t> buf(1, 0);
  write(mStopPipeFd[mPipeWriteIdx], buf.data(), buf.size());
}

bool FileSystemWatcher::isIgnored(const Path& file)
{
  for (int i = 0; i < (int)mOnceIgnoredDirectories.size(); ++i)
  {
    size_t pos = file.ToString().find(mOnceIgnoredDirectories[i].ToString());
    if (pos != std::string::npos)
    {
      mOnceIgnoredDirectories.erase(mOnceIgnoredDirectories.begin() + i);
      return true;
    }
  }

  for (auto & mIgnoredDirectorie : mIgnoredDirectories)
  {
    size_t pos = file.ToString().find(mIgnoredDirectorie.ToString());
    if (pos != std::string::npos)
      return true;
  }

  return false;
}

ssize_t FileSystemWatcher::readEventsIntoBuffer()
{
  ssize_t length = 0;
  length = 0;
  int timeout = 0;
  int nFdsReady = epoll_wait(mEpollFd, mEpollEvents, MAX_EPOLL_EVENTS, timeout);

  if (nFdsReady == -1)
    return length;

  for (int n = 0; n < nFdsReady; ++n)
  {
    if (mEpollEvents[n].data.fd == mStopPipeFd[mPipeReadIdx]) break;

    length = read(mEpollEvents[n].data.fd, mEventBuffer, sizeof(mEventBuffer));
    if (length == -1 && errno == EINTR)
      break;
  }

  return length;
}

void FileSystemWatcher::readEventsFromBuffer(int length, std::vector<FileSystemEvent>& events)
{
  int i = 0;
  while (i < length)
  {
    inotify_event* event = ((struct inotify_event*) &mEventBuffer[i]);

    if ((event->mask & IN_IGNORED) != 0u)
    {
      i += (int)(EVENT_SIZE + event->len);
      mDirectorieMap.erase(event->wd);
      continue;
    }

    Path path = wdToPath(event->wd) / std::string(event->name);

    if (path.IsDirectory())
      event->mask |= IN_ISDIR;
    FileSystemEvent fsEvent(event->wd, (EventType)event->mask, path, DateTime());

    if (!fsEvent.mPath.Empty())
      events.push_back(fsEvent); // else Event is not complete --> ignore

    i += (int)(EVENT_SIZE + event->len);
  }
}

void FileSystemWatcher::filterEvents(std::vector<FileSystemEvent>& events)
{
  for (const FileSystemEvent& currentEvent : events)
    if (!isIgnored(currentEvent.mPath))
      mEventQueue.push(currentEvent);
}

