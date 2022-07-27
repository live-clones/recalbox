//
// Created by bkg2k on 03/11/2020.
//

#include "HardwareMessageSender.h"

HardwareMessageSender::HardwareMessageSender(IHardwareNotifications& notificationInterface)
  : mNotificationInterface(notificationInterface)
  , mSender(*this)
{
}

void HardwareMessageSender::ReceiveSyncMessage(const HardwareMessage& message)
{
  // Process
  ProcessMessage(message);
}

void HardwareMessageSender::ProcessMessage(const HardwareMessage& message)
{
  // Call target interface accordingly
  switch(message.mType)
  {
    case MessageTypes::None: break;
    case MessageTypes::HeadphonePluggedIn:
    {
      { LOG(LogDebug) << "[Hardware] Headphones plugged!"; }
      mNotificationInterface.HeadphonePluggedIn(message.mBoardType);
      break;
    }
    case MessageTypes::HeadphoneUnplugged:
    {
      { LOG(LogDebug) << "[Hardware] Headphones unplugged!"; }
      mNotificationInterface.HeadphoneUnplugged(message.mBoardType);
      break;
    }
    case MessageTypes::PowerButtonPressed:
    {
      { LOG(LogDebug) << "[Hardware] Power button pressed for " << message.mMilliseconds << "ms."; }
      mNotificationInterface.PowerButtonPressed(message.mBoardType, message.mMilliseconds);
      break;
    }
    case MessageTypes::ResetButtonPressed:
    {
      { LOG(LogDebug) << "[Hardware] Reset button pressed"; }
      mNotificationInterface.ResetButtonPressed(message.mBoardType);
      break;
    }
    case MessageTypes::VolumeDownPressed:
    {
      { LOG(LogDebug) << "[Hardware] Volume down button pressed."; }
      mNotificationInterface.VolumeDecrease(message.mBoardType, 0.10f);
      break;
    }
    case MessageTypes::VolumeUpPressed:
    {
      { LOG(LogDebug) << "[Hardware] Volume up button pressed."; }
      mNotificationInterface.VolumeIncrease(message.mBoardType, 0.10f);
      break;
    }
    case MessageTypes::Resume:
    {
      { LOG(LogDebug) << "[Hardware] Hardware exited from suspend mode."; }
      mNotificationInterface.Resume(message.mBoardType);
      break;
    }
    default: break;
  }
}

void HardwareMessageSender::Send(BoardType boardType, MessageTypes messageType)
{
  HardwareMessage message { .mType = messageType, .mBoardType = boardType, .mMilliseconds = 0 };
  if (mNotificationInterface.IsApplicationRunning())
    // Synchronize message with the application main thread
    mSender.Send(message);
  else
    // Application is busy, send message directly
    ProcessMessage(message);
}

void HardwareMessageSender::Send(BoardType boardType, MessageTypes messageType, int milliseconds)
{
  HardwareMessage message { .mType = messageType, .mBoardType = boardType, .mMilliseconds = milliseconds };
  if (mNotificationInterface.IsApplicationRunning())
    // Synchronize message with the application main thread
    mSender.Send(message);
  else
    // Application is busy, send message directly
    ProcessMessage(message);
}

