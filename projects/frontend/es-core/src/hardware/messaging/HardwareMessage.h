//
// Created by bkg2k on 03/11/2020.
//
#pragma once

#include "hardware/BoardType.h"
#include "MessageTypes.h"

class HardwareMessage
{
  public:
    //! Message Type
    MessageTypes mType;
    //! Board Type
    BoardType mBoardType;
    //! Time
    int mMilliseconds;
};
