//
// Created by Supernature2k on 07/02/2022.
//
#pragma once

#include <utils/os/system/Thread.h>
#include <hardware/messaging/HardwareMessageSender.h>

class Pi400PowerEventReader : private Thread
{
public:
    //! Constructor
    explicit Pi400PowerEventReader(HardwareMessageSender& messageSender);

    ~Pi400PowerEventReader() override;

    //! Start reading the power events
    void StartReader();

    //! Stop reading power events
    void StopReader();

private:
    friend class Pi400Board;

    //! Wait for specific button state
    enum class WaitFor
    {
        Press,   //!< Waiting for a press event
        Release, //!< Waiting for a release event
        Ignore,  //!< Ignore next event
    };

    //! Event to read
    //! On Pi400 internal keyboard EventId can change depending on what is connected (mouse, pad)
    //! Using symlink from /by-id/ instead
    static constexpr const char* sInputEventPath = "/dev/input/by-id/usb-_Raspberry_Pi_Internal_Keyboard-event-kbd";
    //! Power key code (Fn + F10)
    static constexpr int sPowerKeyCode = 116;

    //! Message sender
    HardwareMessageSender& mSender;
    //! File handle
    int mFileHandle;
    //! Wait event
    WaitFor mWaitFor;

    //! Break the thread
    void Break() override;

    //! Event reader
    void Run() override;

    //! Suspend
    void Suspend();
};
