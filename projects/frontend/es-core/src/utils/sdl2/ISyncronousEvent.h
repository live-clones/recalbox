#include <SDL_events.h>

#pragma once

/*!
 * @brief Simple interface for Syncroneous callback
 */
class ISyncronousEvent
{
  public:
    virtual void ReceiveSyncCallback(const SDL_Event& event) = 0;
};