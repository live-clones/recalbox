//
// Created by bkg2k on 13/02/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "OdroidAdvanceGo3VolumeReader.h"
#include <utils/Log.h>
#include <linux/input.h>
#include <fcntl.h>
#include <poll.h>
#include <hardware/IBoardInterface.h>

OdroidAdvanceGo3VolumeReader::OdroidAdvanceGo3VolumeReader(HardwareMessageSender& messageSender)
  : mSender(messageSender)
  , mFileHandle(0)
{
}

OdroidAdvanceGo3VolumeReader::~OdroidAdvanceGo3VolumeReader()
{
  StopReader();
}

void OdroidAdvanceGo3VolumeReader::StartReader()
{
  LOG(LogDebug) << "[OdroidAdvanceGoSuper] Volume button driver requested to start.";
  Start("OAG3Vol");
}

void OdroidAdvanceGo3VolumeReader::StopReader()
{
  LOG(LogDebug) << "[OdroidAdvanceGoSuper] Volume button driver requested to stop.";
  Stop();
}

void OdroidAdvanceGo3VolumeReader::Break()
{
  if (mFileHandle >= 0)
  {
    LOG(LogDebug) << "[OdroidAdvanceGoSuper] Breaking Volume button thread.";
    // Close handle to force the 'read' to fail and exit
    int fd = mFileHandle;
    mFileHandle = -1;
    close(fd);
  }
}

void OdroidAdvanceGo3VolumeReader::Run()
{
  LOG(LogInfo) << "[OdroidAdvanceGo] Running background Volume button driver running.";
  while(IsRunning())
  {
    mFileHandle = open(sInputEventPath, O_RDONLY);
    if (mFileHandle < 0)
    {
      LOG(LogError) << "[OdroidAdvanceGo] Error opening " << sInputEventPath << ". Retry in 5s...";
      sleep(5);
      continue;
    }

    MessageTypes lastMessage = MessageTypes::None;

    while(IsRunning())
    {
      // Poll
      struct pollfd poller { .fd = mFileHandle, .events = POLLIN, .revents = 0 };
      if (poll(&poller, 1, 150) != 1 || (poller.revents & POLLIN) == 0)
      {
        if (lastMessage != MessageTypes::None)
          mSender.Send(BoardType::OdroidAdvanceGo, lastMessage);
        continue;
      }

      // Read event
      struct input_event event {};
      if (read(mFileHandle, &event, sizeof(event)) != sizeof(event))
      {
        // Error while the file handle is ok means a true read error
        if (mFileHandle >= 0)
        {
          LOG(LogError) << "[OdroidAdvanceGo] Error reading " << sInputEventPath << ". Retrying";
          close(mFileHandle);
          continue;
        }
        // If file handle NOK, we're instructed to quit
        LOG(LogInfo) << "[OdroidAdvanceGo] Volume button driver ordered to stop.";
        break;
      }

      // Volume down button pressed?
      if ((event.type == EV_KEY) && (event.code == sVolumeDown))
      {
        lastMessage = MessageTypes::None;
        if (event.value == 1)
          mSender.Send(BoardType::OdroidAdvanceGo, lastMessage = MessageTypes::VolumeDownPressed);
      }
      // Volume up button pressed?
      if ((event.type == EV_KEY) && (event.code == sVolumeUp))
      {
        lastMessage = MessageTypes::None;
        if (event.value == 1)
          mSender.Send(BoardType::OdroidAdvanceGo, lastMessage = MessageTypes::VolumeUpPressed);
      }
    }
  }
}
