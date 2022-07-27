//
// Created by bkg2k on 27/07/22.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <scraping/scrapers/IScraperEngineStage.h>

class FileData;

/*!
 * @brief Simple synchronized message for internal scraper use
 */

struct ScrapeSeamlessMessage
{
  //! Game reference
  FileData* mGame;
  //! Scraping stage
  IScraperEngineStage::Stage mStage;
};
