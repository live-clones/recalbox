//
// Created by bkg2k on 22/01/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

class ISwitchComponent
{
  public:
    /*!
     * @brief Called when a switch component has been updated
     * @param id Component id
     * @param status New status
     */
    virtual void SwitchComponentChanged(int id, bool status) = 0;
};
