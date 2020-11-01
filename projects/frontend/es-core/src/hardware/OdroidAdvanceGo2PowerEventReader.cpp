//
// Created by bkg2k on 01/11/2020.
//

#include "OdroidAdvanceGo2PowerEventReader.h"
#include <utils/Log.h>
#include <linux/input.h>
#include <fcntl.h>
#include <poll.h>
#include <utils/datetime/DateTime.h>
#include <MainRunner.h>

OdroidAdvanceGo2PowerEventReader::OdroidAdvanceGo2PowerEventReader()
  : mFileHandle(0)
{
}

OdroidAdvanceGo2PowerEventReader::~OdroidAdvanceGo2PowerEventReader()
{
  StopReader();
}

void OdroidAdvanceGo2PowerEventReader::StartReader()
{
  LOG(LogDebug) << "[OdroidAdvanceGo2] Power button manager requested to start.";
  Start("OAG2Power");
}

void OdroidAdvanceGo2PowerEventReader::StopReader()
{
  LOG(LogDebug) << "[OdroidAdvanceGo2] Power button manager requested to stop.";
  Stop();
}

void OdroidAdvanceGo2PowerEventReader::Break()
{
  if (mFileHandle >= 0)
  {
    LOG(LogDebug) << "[OdroidAdvanceGo2] Breaking power button thread.";
    // Close handle to force the 'read' to fail and exit
    int fd = mFileHandle;
    mFileHandle = -1;
    close(fd);
  }
}

void OdroidAdvanceGo2PowerEventReader::Run()
{
  LOG(LogInfo) << "[OdroidAdvanceGo2] Running background power button manager running.";
  while(IsRunning())
  {
    mFileHandle = open(sInputEventPath, O_RDONLY);
    if (mFileHandle < 0)
    {
      LOG(LogError) << "[OdroidAdvanceGo2] Error opening " << sInputEventPath << ". Retry in 5s...";
      sleep(5);
      continue;
    }

    DateTime reference(0LL);
    while(IsRunning())
    {
      // Poll
      struct pollfd poller { .fd = mFileHandle, .events = POLLIN, .revents = 0 };
      if (poll(&poller, 1, 1000) == 1 && (poller.revents & POLLIN) != 0)
      {
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
          LOG(LogInfo) << "[OdroidAdvanceGo2] Power event reader ordered to stop.";
          break;
        }

        // Power button
        if ((event.type == 1) && (event.code == sPowerKeyCode))
        {
          if (event.value != 0) reference = DateTime();
          else if (!reference.IsZero())
          {
            int elapsed = (int) ((DateTime() - reference).TotalMilliseconds());
            LOG(LogInfo) << "[OdroidAdvanceGo2] Power button pressed for " << elapsed << "ms";
            if (elapsed < 2000)
            {
              { LOG(LogInfo) << "[OdroidAdvanceGo2] SUSPEND!"; }
              system("/usr/sbin/pm-suspend"); // Suspend mode
            }
            else
            {
              { LOG(LogInfo) << "[OdroidAdvanceGo2] POWER OFF!"; }
              MainRunner::RequestQuit(MainRunner::ExitState::Shutdown, false);
            }
          }
        }
      }
    }
  }
}
