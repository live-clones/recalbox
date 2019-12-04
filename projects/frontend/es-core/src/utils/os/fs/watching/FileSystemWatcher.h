/*! File watching, inspired from the Inotofy library from Erik Zenker */

#pragma once

#include <map>
#include <queue>
#include <sys/inotify.h>
#include <sys/epoll.h>
#include <vector>

#include "FileSystemEvent.h"
#include "IFileSystemWatcherNotification.h"

/*!
 * @eventMask
 *
 * IN_ACCESS         File was accessed (read) (*).
 * IN_ATTRIB         Metadata changed—for example, permissions,
 *                   timestamps, extended attributes, link count
 *                   (since Linux 2.6.25), UID, or GID. (*).
 * IN_CLOSE_WRITE    File opened for writing was closed (*).
 * IN_CLOSE_NOWRITE  File not opened for writing was closed (*).
 * IN_CREATE         File/directory created in watched directory(*).
 * IN_DELETE         File/directory deleted from watched directory(*).
 * IN_DELETE_SELF    Watched file/directory was itself deleted.
 * IN_MODIFY         File was modified (*).
 * IN_MOVE_SELF      Watched file/directory was itself moved.
 * IN_MOVED_FROM     Generated for the directory containing the old
 *                   filename when a file is renamed (*).
 * IN_MOVED_TO       Generated for the directory containing the new
 *                   filename when a file is renamed (*).
 * IN_OPEN           File was opened (*).
 * IN_ALL_EVENTS     macro is defined as a bit mask of all of the above
 *                   events
 * IN_MOVE           IN_MOVED_FROM|IN_MOVED_TO
 * IN_CLOSE          IN_CLOSE_WRITE | IN_CLOSE_NOWRITE
 *
 * See inotify manpage for more event details
 *
 */

class FileSystemWatcher
{
  public:
    static constexpr int MAX_EVENTS = 256;
    static constexpr int MAX_EPOLL_EVENTS = 1;
    static constexpr int EVENT_SIZE = (int) (sizeof(inotify_event));

    FileSystemWatcher();

    ~FileSystemWatcher();

    void watchDirectoryRecursively(const Path& path);

    void watchFile(const Path& file);

    void unwatchFile(const Path& file);

    void ignoreFileOnce(const Path& file) { mOnceIgnoredDirectories.push_back(file); }

    void ignoreFile(const Path& file) { mIgnoredDirectories.push_back(file); }

    void setEventMask(EventType eventMask) { mEventMask = eventMask; }

    EventType getEventMask() const { return mEventMask; }

    bool getNextEvent(FileSystemEvent& fsevent);

    void stop();

    bool hasStopped() const { return mStopped; }

  private:
    Path wdToPath(int wd)
    { return mDirectorieMap[wd]; }

    bool isIgnored(const Path& file);

    void removeWatch(int wd);

    ssize_t readEventsIntoBuffer();

    void readEventsFromBuffer(int length, std::vector<FileSystemEvent>& events);

    void filterEvents(std::vector<FileSystemEvent>& events);

    void sendStopSignal();

  private:
    EventType mEventMask;
    int mThreadSleep;
    Path::PathList mIgnoredDirectories;
    Path::PathList mOnceIgnoredDirectories;
    std::queue<FileSystemEvent> mEventQueue;
    std::map<int, Path> mDirectorieMap;
    int mInotifyFd;
    volatile bool mStopped;
    int mEpollFd;
    epoll_event mInotifyEpollEvent;
    epoll_event mStopPipeEpollEvent;
    epoll_event mEpollEvents[MAX_EPOLL_EVENTS];
    unsigned char mEventBuffer[MAX_EVENTS * (EVENT_SIZE + 16)];

    int mStopPipeFd[2];
    const int mPipeReadIdx;
    const int mPipeWriteIdx;
};
