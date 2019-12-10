//
// Created by bkg2k on 04/12/2019.
//
#pragma once

class INotifyScrapeResult
{
  public:
    /*!
     * @brief Notify a game has been scraped
     * @param index Game index
     * @param total Total game to scrape
     * @param result Result object
     */
    virtual void GameResult(int index, int total, const FileData* result) = 0;

    /*!
     * @brief Scraper site quota reached. Scrapping is being aborted immediately.
     */
    virtual void QuotaReached() = 0;
};
