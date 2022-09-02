//
// Created by davidb2111 on 31/08/2022.
//
#pragma once

#include <vector>
#include <string>

class IAudioNotification
{
  public:
    //! Destructor
    virtual ~IAudioNotification() = default;

    virtual void NotifyAudioChange() = 0;
};
