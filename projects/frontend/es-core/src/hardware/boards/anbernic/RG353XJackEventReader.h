//
// Created by bkg2k on 02/11/2020.
//
#pragma once

#include <utils/os/system/Thread.h>
#include <hardware/messaging/HardwareMessageSender.h>

class RG353XJackEventReader : private Thread
{
  public:
    //! Constructor
    explicit RG353XJackEventReader(HardwareMessageSender& messageSender);

    //! Destructor
    ~RG353XJackEventReader() override;

    //! Start reading the power events
    void StartReader();

    //! Stop reading power events
    void StopReader();

  private:
    //! Event to read
    static constexpr const char* sInputEventPath = "/dev/input/event1";
    //! Headphone insert event type
    static constexpr int sHeadphoneInsertType = 5;
    //! Headphone insert event code
    static constexpr int sHeadphoneInsertCode = 2;
    //! Message sender
    HardwareMessageSender& mSender;
    //! File handle
    int mFileHandle;

    //! Break the thread
    void Break() override;

    //! Event reader
    void Run() override;
};
