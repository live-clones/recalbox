//
// Created by bkg2k on 10/05/22.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

class IPatreonNotification
{
  public:
    //! Required default destructor
    virtual ~IPatreonNotification() = default;

    /*!
     * @brief Notify of the current user status
     * @param state True when the user is a patron
     * @param patreonName Patreon name
     */
    virtual void PatreonState(bool state, const std::string& patreonName) = 0;
};
