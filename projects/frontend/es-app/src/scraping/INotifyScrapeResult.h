//
// Created by bkg2k on 04/12/2019.
//
#pragma once

#include "ScrapeResult.h"

class INotifyScrapeResult
{
  public:
    /*!
     * @brief Notify a game has been scraped
     * @param index Game index
     * @param total Total game to scrape
     * @param result Result object
     */
    virtual void GameResult(int index, int total, FileData* result) = 0;

    /*!
     * @brief Notify the caller of scraping is being aborted immediately
     */
    virtual void ScrapingComplete(ScrapeResult reason) = 0;
};
