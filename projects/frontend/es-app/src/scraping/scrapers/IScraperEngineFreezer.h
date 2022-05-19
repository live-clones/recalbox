//
// Created by bkg2k on 26/03/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

class IScraperEngineFreezer
{
  public:
    /*!
     * @brief Implementation should return true to freeze scrapping engines
     * of a background running scraper.
     * @return True to freeze scraper engines, false to let them scrape
     */
    virtual bool MustFreeze() = 0;
};