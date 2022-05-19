//
// Created by bkg2k on 10/05/22.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include "PatronAuthenticationResult.h"

class IPatreonNotification
{
  public:
    //! Required default destructor
    virtual ~IPatreonNotification() = default;

    /*!
     * @brief Notify of the current user status
     * @param result Authentication result (user status)
     * @param level Patron level
     * @param patreonName Patreon name
     */
    virtual void PatreonState(PatronAuthenticationResult result, int level, const std::string& patreonName) = 0;
};
