//
// Created by bkg2k on 04/12/2019.
//

#include <scraping/new/scrapers/screenscraper/ScreenScraperEngine.h>
#include <scraping/new/scrapers/thegamedb/TheGameDBEngine.h>
#include "ScraperFactory.h"

ScraperFactory::ScraperHolder ScraperFactory::mScrapers;

ScraperFactory::ScraperHolder::~ScraperHolder()
{
  for(auto scraper : mScrapers)
    delete scraper.second;
}

IScraperEngine* ScraperFactory::ScraperHolder::Get(ScraperFactory::ScraperType type)
{
  // Ensure valid type
  switch(type)
  {
    case ScraperType::ScreenScraper:
    case ScraperType::TheGameDB: break;
    default: type = ScraperType::ScreenScraper;
  }

  // Already created?
  auto scraper = mScrapers.find(type);
  if (scraper != mScrapers.end())
    return scraper->second;

  // Nope, create it
  IScraperEngine* result = nullptr;
  switch(type)
  {
    case ScraperType::ScreenScraper: result = new ScreenScraperEngine(); break;
    case ScraperType::TheGameDB: result = new TheGameDBEngine(); break;
  }
  mScrapers[type] = result;
  return result;
}

IScraperEngine* ScraperFactory::GetScraper(const std::string& scraperidentifier)
{
  // Default scraper
  ScraperType type = ScraperType::ScreenScraper;

  // Identify
  if (scraperidentifier == "Screenscraper") type = ScraperType::ScreenScraper;
  else if (scraperidentifier == "TheGamesDB") type = ScraperType::TheGameDB;

  // Get
  return mScrapers.Get(type);
}
