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
  { LOG(LogDebug) << "[OdroidAdvanceGo] Headphone driver requested to start."; }
  Start("OAG2Jack");
}

void OdroidAdvanceGo2JackEventReader::StopReader()
{
  { LOG(LogDebug) << "[OdroidAdvanceGo] Headphone driver requested to stop."; }
  Stop();
}

void OdroidAdvanceGo2JackEventReader::Break()
{
  if (mFileHandle >= 0)
  {
    { LOG(LogDebug) << "[OdroidAdvanceGo] Breaking headphone thread."; }
    mFileHandle = -1;
    }
}

void OdroidAdvanceGo2JackEventReader::Run()
{
  { LOG(LogInfo) << "[OdroidAdvanceGo] Running background headphone driver running."; }
  while(IsRunning())
  {
    mFileHandle = open(sInputEventPath, O_RDONLY);
    if (mFileHandle < 0)
    {
      { LOG(LogError) << "[OdroidAdvanceGo] Error opening " << sInputEventPath << ". Retry in 5s..."; }
      sleep(5);
      continue;
    }

    while(IsRunning())
    {
      // Poll
      struct pollfd poller { .fd = mFileHandle, .events = POLLIN, .revents = 0 };
      if (poll(&poller, 1, 100) != 1 || (poller.revents & POLLIN) == 0) continue;

      // Read event
      struct input_event event {};
      if (read(mFileHandle, &event, sizeof(event)) != sizeof(event))
      {
        close(mFileHandle);
        // Error while the file handle is ok means a true read error
        if (mFileHandle >= 0)
        {
          { LOG(LogError) << "[OdroidAdvanceGo] Error reading " << sInputEventPath << ". Retrying"; }
          continue;
        }
        // If file handle NOK, we're instructed to quit
        { LOG(LogInfo) << "[OdroidAdvanceGo] Headphone driver ordered to stop."; }
        break;
      }

      // Power button pressed?
      if ((event.type == sHeadphoneInsertType) && (event.code == sHeadphoneInsertCode))
        mSender.Send(BoardType::OdroidAdvanceGo, (event.value == 0) ? MessageTypes::HeadphoneUnplugged : MessageTypes::HeadphonePluggedIn);
    }
  }
}
