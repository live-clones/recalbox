//
// Created by bkg2k on 14/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

class IInputChange
{
  public:
    /*!
     * @brief Called by the InputManager right after the device list is refreshed
     * because of a pad added or removed
     */
    virtual void PadsAddedOrRemoved() = 0;
};
