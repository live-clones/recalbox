//
// Created by bkg2k on 03/11/2020.
//

#include "HardwareMessageSender.h"

HardwareMessageSender::HardwareMessageSender(IHardwareNotifications& notificationInterface)
  : mNotificationInterface(notificationInterface)
  , mSender(this)
{
}

void HardwareMessageSender::ReceiveSyncCallback(const SDL_Event& event)
{
  // Extract parameters
  HardwareMessage* message = (HardwareMessage*)event.user.data1;

  // Process
  ProcessMessage(message);
}

void HardwareMessageSender::ProcessMessage(HardwareMessage* message)
{
  // Call target interface accordingly
  switch(message->Type())
  {
    case MessageTypes::None: break;
    case MessageTypes::HeadphonePluggedIn:
    {
      { LOG(LogDebug) << "[Hardware] Headphones plugged!"; }
      mNotificationInterface.HeadphonePluggedIn(message->Board());
      break;
    }
    case MessageTypes::HeadphoneUnplugged:
    {
      { LOG(LogDebug) << "[Hardware] Headphones unplugged!"; }
      mNotificationInterface.HeadphoneUnplugged(message->Board());
      break;
    }
    case MessageTypes::PowerButtonPressed:
    {
      { LOG(LogDebug) << "[Hardware] Power button pressed for " << message->Millisecond() << "ms."; }
      mNotificationInterface.PowerButtonPressed(message->Board(), message->Millisecond());
      break;
    }
    case MessageTypes::VolumeDownPressed:
    {
      { LOG(LogDebug) << "[Hardware] Volume down button pressed."; }
      mNotificationInterface.VolumeDecrease(message->Board(), 0.10f);
      break;
    }
    case MessageTypes::VolumeUpPressed:
    {
      { LOG(LogDebug) << "[Hardware] Volume up button pressed."; }
      mNotificationInterface.VolumeIncrease(message->Board(), 0.10f);
      break;
    }
    case MessageTypes::Resume:
    {
      { LOG(LogDebug) << "[Hardware] Hardware exited from suspend mode."; }
      mNotificationInterface.Resume(message->Board());
      break;
    }
    default: break;
  }

  // Recycle message
  mMessages.Recycle(message);
}

void HardwareMessageSender::Send(BoardType boardType, MessageTypes messageType)
{
  HardwareMessage* message = mMessages.Obtain()->Build(messageType, boardType);
  if (mNotificationInterface.IsApplicationRunning())
    // Synchronize message with the application main thread
    mSender.Call((int)messageType, (void*)message);
  else
    // Application is busy, send message directly
    ProcessMessage(message);
}

void HardwareMessageSender::Send(BoardType boardType, MessageTypes messageType, int milliseconds)
{
  HardwareMessage* message = mMessages.Obtain()->Build(messageType, boardType, milliseconds);
  if (mNotificationInterface.IsApplicationRunning())
    // Synchronize message with the application main thread
    mSender.Call((int)messageType, (void*)message);
  else
    // Application is busy, send message directly
    ProcessMessage(message);
}

