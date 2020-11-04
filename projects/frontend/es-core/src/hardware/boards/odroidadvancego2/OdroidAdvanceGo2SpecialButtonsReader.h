//
// Created by bkg2k on 01/11/2020.
//
#pragma once

#include <utils/os/system/Thread.h>

class OdroidAdvanceGo2SpecialButtonsReader : private Thread
{
  public:
    //! Constructor
    OdroidAdvanceGo2SpecialButtonsReader();

    //! Start reading the power events
    void StartReader();

    //! Stop reading power events
    void StopReader();

  private:
    //! Break the thread
    void Break() override;

    //! Event reader
    void Run() override;
};

