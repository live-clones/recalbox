//
// Created by bkg2k on 01/11/2020.
//
#pragma once

#include <utils/os/system/Thread.h>
#include <hardware/IBoardInterface.h>

class OdroidAdvanceGo2SpecialButtonsReader : private Thread
{
  public:
    //! Constructor
    explicit OdroidAdvanceGo2SpecialButtonsReader(IBoardInterface& boardInterface);

    //! Start reading the power events
    void StartReader();

    //! Stop reading power events
    void StopReader();

  private:
    //! Parent board interface
    IBoardInterface& mBoardInterface;

    //! Break the thread
    void Break() override;

    //! Event reader
    void Run() override;
};

