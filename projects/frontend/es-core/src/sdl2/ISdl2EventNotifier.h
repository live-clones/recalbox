//
// Created by bkg2k on 02/09/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <SDL.h>

class ISdl2EventNotifier
{
  public:
    /*!
     * @brief Received a raw SDL2 event
     * @param event SDL2 event
     */
    virtual void Sdl2EventReceived(const SDL_Event& event) = 0;

    /*!
     * @brief Instruct the SDL2 loop to send or not the event.
     * @return If the implementation returns true, it will receive events. If false, it will receive nothing
     */
    virtual bool Sdl2EventIsActive() { return true; }
 };
