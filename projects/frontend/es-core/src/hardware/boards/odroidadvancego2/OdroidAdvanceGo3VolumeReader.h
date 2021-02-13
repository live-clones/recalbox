//
// Created by bkg2k on 13/02/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#pragma once

#include <utils/os/system/Thread.h>
#include <hardware/messaging/HardwareMessageSender.h>

class OdroidAdvanceGo3VolumeReader : private Thread
{
  public:
    //! Constructor
    explicit OdroidAdvanceGo3VolumeReader(HardwareMessageSender& messageSender);

    //! Destructor
    ~OdroidAdvanceGo3VolumeReader() override;

    //! Start reading the power events
    void StartReader();

    //! Stop reading power events
    void StopReader();

  private:
    //! Event to read
    static constexpr const char* sInputEventPath = "/dev/input/event3";
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
};



