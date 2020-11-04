//
// Created by bkg2k on 02/11/2020.
//

#include "OdroidAdvanceGo2JackEventReader.h"
#include <utils/Log.h>
#include <linux/input.h>
#include <fcntl.h>
#include <poll.h>
#include <hardware/IBoardInterface.h>

OdroidAdvanceGo2JackEventReader::OdroidAdvanceGo2JackEventReader(HardwareMessageSender& messageSender)
  : mSender(messageSender)
  , mFileHandle(0)
{
}

OdroidAdvanceGo2JackEventReader::~OdroidAdvanceGo2JackEventReader()
{
  StopReader();
}

void OdroidAdvanceGo2JackEventReader::StartReader()
{
  LOG(LogDebug) << "[OdroidAdvanceGo2] Headphone driver requested to start.";
  Start("OAG2Jack");
}

void OdroidAdvanceGo2JackEventReader::StopReader()
{
  LOG(LogDebug) << "[OdroidAdvanceGo2] Headphone driver requested to stop.";
  Stop();
}

void OdroidAdvanceGo2JackEventReader::Break()
{
  if (mFileHandle >= 0)
  {
    LOG(LogDebug) << "[OdroidAdvanceGo2] Breaking headphone thread.";
    // Close handle to force the 'read' to fail and exit
    int fd = mFileHandle;
    mFileHandle = -1;
    close(fd);
  }
}

void OdroidAdvanceGo2JackEventReader::Run()
{
  LOG(LogInfo) << "[OdroidAdvanceGo2] Running background headphone driver running.";
  while(IsRunning())
  {
    mFileHandle = open(sInputEventPath, O_RDONLY);
    if (mFileHandle < 0)
    {
      LOG(LogError) << "[OdroidAdvanceGo2] Error opening " << sInputEventPath << ". Retry in 5s...";
      sleep(5);
      continue;
    }

    while(IsRunning())
    {
      // Poll
      struct pollfd poller { .fd = mFileHandle, .events = POLLIN, .revents = 0 };
      if (poll(&poller, 1, 1000) != 1 || (poller.revents & POLLIN) == 0) continue;

      // Read event
      struct input_event event {};
      if (read(mFileHandle, &event, sizeof(event)) != sizeof(event))
      {
        // Error while the file handle is ok means a true read error
        if (mFileHandle >= 0)
        {
          LOG(LogError) << "[OdroidAdvanceGo2] Error reading " << sInputEventPath << ". Retrying";
          close(mFileHandle);
          continue;
        }
        // If file handle NOK, we're instructed to quit
        LOG(LogInfo) << "[OdroidAdvanceGo2] Headphone driver ordered to stop.";
        break;
      }

      // Power button pressed?
      if ((event.type == sHeadphoneInsertType) && (event.code == sHeadphoneInsertCode))
      {
        if (event.value == 1) mSender.Send(BoardType::OdroidAdvanceGo2, MessageTypes::HeadphoneUnplugged);
        else if (event.value == 0) mSender.Send(BoardType::OdroidAdvanceGo2, MessageTypes::HeadphonePluggedIn);
      }
    }
  }
}
