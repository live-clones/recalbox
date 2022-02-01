//
// Created by bkg2k on 19/11/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <string>
#include <SDL_joystick.h>

class AutoMapper
{
  public:
    /*!
     * @brief Build a new automapper class using the given joystick
     * @param joy Joystick to get ammping from
     */
    explicit AutoMapper(int joyIndex)
      : mSDLJoystickIndex(joyIndex)
    {
    }

    /*!
     * @brief Get mapping of the given joystick
     * @return SDL Mapping string
     */
    std::string GetSDLMapping();

  private:
    //! Joystick handle
    int mSDLJoystickIndex;

    /*!
     * @brief Check if the joystick is an XBox pad managed by the XBad driver
     * @return True if the the joystick is an XBox pad
     */
    bool IsXBox() const;

    /*!
     * @brief Build mapping from udev properties
     * @return SDL2 compatible mapping
     */
    std::string BuildMapping(const std::string& sdlMapping) const;
};



