//
// Created by bkg2k on 22/02/2020.
//

#include "PadToKeyboardManager.h"

PadToKeyboardManager::PadToKeyboardManager(const OrderedDevices& orderedDevices, const Path& rompath)
  : mConfiguration(orderedDevices),
    mMappingConfiguration(rompath),
    mPadConfiguration(),
    mPadReader(mPadConfiguration),
    mValid(false)
{
}

void PadToKeyboardManager::StartMapping()
{
  // Get pad => keyboard mapping - Checked at the very beginning to quit gracefully if no config is found
  if (!mMappingConfiguration.Valid())
  {
    LOG(LogInfo) << "No pad2keyb configuration.";
    return;
  }
  LOG(LogInfo) << "[Pad2Keyboard] user mapping loaded loaded.";

  // Get pad mapping from /recalbox/share/system/.emulationstation/es_input.cfg
  mPadConfiguration.Load(mConfiguration);
  if (!mPadConfiguration.Ready())
  {
    LOG(LogError) << "Cannot load pad configurations.";
    return;
  }
  LOG(LogInfo) << "[Pad2Keyboard] pad configuration loaded.";

  // Get pad event reader
  mPadReader.Open(mConfiguration);
  if (!mPadReader.Ready())
  {
    LOG(LogError) << "Cannot access pad.";
    return;
  }
  LOG(LogInfo) << "[Pad2Keyboard] Pad reader is ready.";

  // Create virtual keyboard device
  mKeyboardWriter.Open();
  if (!mKeyboardWriter.Ready())
  {
    LOG(LogError) << "Cannot create virtual keyboard.";
    return;
  }
  LOG(LogInfo) << "[Pad2Keyboard] Virtual keyboard is ready.";

  // Validate
  mValid = true;

  // Start thread
  Thread::Start("pad2key");
}

void PadToKeyboardManager::StopMapping()
{
  if (mValid)
  {
    mPadReader.Release();
    Thread::Join();
  }
}

void PadToKeyboardManager::Run()
{
  pthread_t currentThread = pthread_self();
  // We'll set the priority to the maximum.
  sched_param params {};
  params.sched_priority = sched_get_priority_max(SCHED_FIFO);
  if (pthread_setschedparam(currentThread, SCHED_FIFO, &params) != 0)
    LOG(LogWarning) << "Cannot set Pad2Keyboard thread at highest priority!";

  Pad::Event event {};
  while(mPadReader.GetEvent(event))
  {
    //LOG(LogWarning) << "Event read! " << (int)event.Item << " - " << (int)event.Pad << (int)event.On;
    VirtualKeyboard::Event keyboardEvent = mMappingConfiguration.Translate(event);
    //LOG(LogWarning) << "Translated! " << (int)keyboardEvent.Key << " - " << (int)keyboardEvent.Pressed;
    if (keyboardEvent.Key != 0) // Valid?
    {
      mKeyboardWriter.Write(keyboardEvent);
      //LOG(LogDebug) << "Key: " << keyboardEvent.Key << (keyboardEvent.Pressed ? " Pressed" : " Released");
    }
  }
}
