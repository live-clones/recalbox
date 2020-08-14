//
// Created by bkg2k on 14/08/2020.
//
#pragma once

#include <string>

class NameFiltering
{
  public:
    /*!
     * @brief Filter some words and replace some others to make ALSA naming more user-friendly
     * @param source source string
     * @return filtered string
     */
    static std::string Filter(const std::string& source);
};
