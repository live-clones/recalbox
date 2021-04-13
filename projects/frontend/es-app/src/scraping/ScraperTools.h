//
// Created by bkg2k on 09/02/2020.
//
#pragma once

#include <string>

//! Scraped name options
enum class ScraperNameOptions
{
  GetFromScraper,             //!< Get name from scraper (default)
  GetFromFilename,            //!< Keep the raw filename
  GetFromFilenameUndecorated, //!< Keep the raw filename, but remove decorations, text inside ()[]
};

class ScraperTools
{
  public:
    /*!
     * @brief Check and cap name option
     * @param option Original option
     * @return Clamped option
     */
    static ScraperNameOptions Clamp(int option);

    static ScraperNameOptions ScraperNameOptionsFromString(const std::string& scraperNameOptions);
    static const std::string& ScraperNameOptionsFromEnum(ScraperNameOptions scraperNameOptions);
};
