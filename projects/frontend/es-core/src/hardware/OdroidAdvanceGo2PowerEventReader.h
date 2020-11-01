//
// Created by bkg2k on 01/11/2020.
//
#pragma once

#include <utils/os/system/Thread.h>

class OdroidAdvanceGo2PowerEventReader : private Thread
{
  public:
    //! Constructor
    OdroidAdvanceGo2PowerEventReader();

    ~OdroidAdvanceGo2PowerEventReader();

    //! Start reading the power events
    void StartReader();

    //! Stop reading power events
    void StopReader();

  private:
    //! Event to read
    static constexpr const char* sInputEventPath = "/dev/input/event0";
    //! Power key code
    static constexpr int sPowerKeyCode = 116;
    //! File handle
    int mFileHandle;

    //! Break the thread
    void Break() override;

    //! Event reader
    void Run() override;
};
