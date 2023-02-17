//
// Created by bkg2k on 13/02/2021.
// Modified by davidb2111 for the RG353x series of boards
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#pragma once

#include <utils/os/system/Thread.h>
#include <hardware/messaging/HardwareMessageSender.h>

class RG353XVolumeReader : private Thread
{
  public:
    //! Constructor
    explicit RG353XVolumeReader(HardwareMessageSender& messageSender);

    //! Destructor
    ~RG353XVolumeReader() override;

    //! Start reading the power events
    void StartReader();

    //! Stop reading power events
    void StopReader();

    //! hotkey is pressed
    void HotkeyPressed() { mHotkeyPressed = true; };
    void HotkeyReleased() { mHotkeyPressed = false; };

  private:
    //! Event to read
    static constexpr const char* sInputEventPath = "/dev/input/by-path/platform-gpio-keys-event";
    //! Volume button up
    static constexpr int sVolumeUp = 115;
    //! Volume button down
    static constexpr int sVolumeDown = 114;
    //! Message sender
    HardwareMessageSender& mSender;
    //! File handle
    int mFileHandle;

    //! Break the thread
    void Break() override;

    //! Event reader
    void Run() override;

    bool mHotkeyPressed = false;
};



