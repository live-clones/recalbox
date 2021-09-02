//
// Created by bkg2k on 22/02/2020.
//

#include "PadToKeyboardManager.h"

PadToKeyboardManager::PadToKeyboardManager(const OrderedDevices& orderedDevices, const Path& rompath, Sdl2Runner& sdl2Runner)
  : mConfiguration(orderedDevices)
  , mMappingConfiguration(rompath)
  , mPadConfiguration()
  , mPadReader(mPadConfiguration)
  , mSdlRunner(sdl2Runner)
  , mValid(false)
{
}

void PadToKeyboardManager::StartMapping()
{
  // Get pad => keyboard mapping - Checked at the very beginning to quit gracefully if no config is found
  if (!mMappingConfiguration.Valid()) { { LOG(LogInfo) << "[Pad2Keyboard] No pad2keyb configuration."; } return; }
  { LOG(LogInfo) << "[Pad2Keyboard] user mapping loaded loaded."; }

  // Get pad mapping from ordered devices
  mPadConfiguration.Load(mConfiguration);
  if (!mPadConfiguration.Ready()) { { LOG(LogError) << "[Pad2Keyboard] Cannot load pad configurations."; } return; }
  { LOG(LogInfo) << "[Pad2Keyboard] pad configuration loaded."; }

  // Get pad event reader
  mPadReader.Open(mConfiguration);
  if (!mPadReader.Ready()) { { LOG(LogError) << "[Pad2Keyboard] Cannot access pad."; } return; }
  { LOG(LogInfo) << "[Pad2Keyboard] Pad reader is ready."; }

  // Create virtual keyboard device
  mKeyboardWriter.Open();
  if (!mKeyboardWriter.Ready()) { { LOG(LogError) << "[Pad2Keyboard] Cannot create virtual keyboard."; } return; }
  { LOG(LogInfo) << "[Pad2Keyboard] Virtual keyboard is ready."; }

  // Create virtual keyboard device
  mMouseWriter.Open();
  if (!mMouseWriter.Ready()) { { LOG(LogError) << "[Pad2Keyboard] Cannot create virtual mouse."; } return; }
  { LOG(LogInfo) << "[Pad2Keyboard] Virtual mouse is ready."; }

  // Register SDL2 events
  int Sdl2EventToRegister[] =
    {
      SDL_JOYHATMOTION,
      SDL_JOYBUTTONDOWN,
      SDL_JOYBUTTONUP,
      SDL_JOYAXISMOTION,
    };
  for(int event : Sdl2EventToRegister)
    mSdlRunner.Register(event, this);

  // Validate
  mValid = true;
}

void PadToKeyboardManager::StopMapping()
{
  mSdlRunner.UnregisterAll(this);
}

void PadToKeyboardManager::Sdl2EventReceived(const SDL_Event& sdlevent)
{
  mPadReader.PushSDL2Event(sdlevent);

  Pad::Event event {};
  VirtualKeyboard::EventList keyboardEventList {};
  VirtualMouse::Event mouseEvent {};
  while(mPadReader.PopPadEvent(event))
  {
    //{ LOG(LogWarning) << "Event read! " << (int)event.Item << " - " << (int)event.Pad << ':' << (int)event.On << ':' << event.Value << '-' << event.Analog; }
    switch(mMappingConfiguration.Translate(event, keyboardEventList, mouseEvent))
    {
      case MappingConfiguration::Types::None: break;
      case MappingConfiguration::Types::Keyboard:
      {
        //{ LOG(LogWarning) << "Translated keyboardList! " << (int)keyboardEventList.Count << " - " << keyboardEventList.Delay; }
        if (event.On)
          for(int i = 0; i < keyboardEventList.Count; ++i)
          {
            //{ LOG(LogWarning) << "Translated Keyboard! Press: " << (int)keyboardEventList.Events[i].Key << " - " << (int)keyboardEventList.Events[i].Pressed; }
            mKeyboardWriter.Write((keyboardEventList.Events[i]));
            if (keyboardEventList.Delay != 0 && i != (keyboardEventList.Count - 1)) Thread::Sleep(keyboardEventList.Delay);
          }
        else
          for(int i = keyboardEventList.Count; --i >= 0; )
          {
            //{ LOG(LogWarning) << "Translated Keyboard! Release: " << (int)keyboardEventList.Events[i].Key << " - " << (int)keyboardEventList.Events[i].Pressed; }
            mKeyboardWriter.Write((keyboardEventList.Events[i]));
            if (keyboardEventList.Delay != 0 && i != 0) Thread::Sleep(keyboardEventList.Delay);
          }
        break;
      }
      case MappingConfiguration::Types::KeyboardSequence:
      {
        //{ LOG(LogWarning) << "Translated keyboardList! " << (int)keyboardEventList.Count << " - " << keyboardEventList.Delay; }
        if (event.On) // Send sequences only on press
          for(int i = 0; i < keyboardEventList.Count; ++i)
          {
            //{ LOG(LogWarning) << "Translated Keyboard! Sequence: " << (int)keyboardEventList.Events[i].Key; }
            keyboardEventList.Events[i].Pressed = true;  mKeyboardWriter.Write((keyboardEventList.Events[i]));
            if (keyboardEventList.Delay != 0) Thread::Sleep(keyboardEventList.Delay);
            keyboardEventList.Events[i].Pressed = false; mKeyboardWriter.Write((keyboardEventList.Events[i]));
            if (keyboardEventList.Delay != 0 && i != (keyboardEventList.Count - 1)) Thread::Sleep(keyboardEventList.Delay);
          }
        break;
      }
      case MappingConfiguration::Types::MouseButton:
      case MappingConfiguration::Types::MouseMove:
      {
        //{ LOG(LogWarning) << "Translated Mouse! " << mouseEvent.X.Axis << " - " << mouseEvent.Y.Axis << " - " << (int)mouseEvent.ButtonsP << ':' << (int)mouseEvent.ButtonsR; }
        if (mouseEvent.IsMove()) mMouseWriter.PushEvent(mouseEvent); // Send non-move event immediately
        else mMouseWriter.Write(mouseEvent); // Mouse moves must be sustained
        break;
      }
    }

  }
}
