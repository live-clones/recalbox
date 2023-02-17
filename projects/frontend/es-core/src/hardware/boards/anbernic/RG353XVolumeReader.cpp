//
// Created by bkg2k on 13/02/2021.
// Modified by davidb2111 for the RG353x series of boards
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "RG353XVolumeReader.h"
#include <utils/Log.h>
#include <linux/input.h>
#include <fcntl.h>
#include <poll.h>
#include <hardware/IBoardInterface.h>

RG353XVolumeReader::RG353XVolumeReader(HardwareMessageSender& messageSender)
  : mSender(messageSender)
  , mFileHandle(0)
{
}

RG353XVolumeReader::~RG353XVolumeReader()
{
  StopReader();
}

void RG353XVolumeReader::StartReader()
{
  { LOG(LogDebug) << "[RG353X] Volume button driver requested to start."; }
  Start("RG353XVol");
}

void RG353XVolumeReader::StopReader()
{
  { LOG(LogDebug) << "[RG353X] Volume button driver requested to stop."; }
  Stop();
}

void RG353XVolumeReader::Break()
{
  if (mFileHandle >= 0)
  {
    { LOG(LogDebug) << "[RG353X] Breaking Volume button thread."; }
    mFileHandle = -1;
  }
}

void RG353XVolumeReader::Run()
{
  { LOG(LogInfo) << "[RG353X] Running background Volume button driver running."; }
  while(IsRunning())
  {
    mFileHandle = open(sInputEventPath, O_RDONLY);
    if (mFileHandle < 0)
    {
      { LOG(LogError) << "[RG353X] Error opening " << sInputEventPath << ". Retry in 5s..."; }
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
          { LOG(LogError) << "[RG353X] Error reading " << sInputEventPath << ". Retrying"; }
          continue;
        }
        // If file handle NOK, we're instructed to quit
        { LOG(LogInfo) << "[RG353X] Volume button driver ordered to stop."; }
        break;
      }
      // Volume down and hotkey buttons pressed?
      if (mHotkeyPressed)
      {
        if ((event.type == EV_KEY) && (event.code == sVolumeDown) && (event.value >= 1))
          mSender.Send(BoardType::RG353P, MessageTypes::BrightnessDownPressed);
        // Volume up button pressed?
        if ((event.type == EV_KEY) && (event.code == sVolumeUp) && (event.value >= 1))
          mSender.Send(BoardType::RG353P, MessageTypes::BrightnessUpPressed);
      }else // only volume button pressed
      {
        if ((event.type == EV_KEY) && (event.code == sVolumeDown) && (event.value >= 1))
          mSender.Send(BoardType::RG353P, MessageTypes::VolumeDownPressed);
        // Volume up button pressed?
        if ((event.type == EV_KEY) && (event.code == sVolumeUp) && (event.value >= 1))
          mSender.Send(BoardType::RG353P, MessageTypes::VolumeUpPressed);
      }
    }
  }
}
