//
// Created by bkg2k on 04/12/2019.
//

#include <scraping/scrapers/screenscraper/ScreenScraperEngine.h>
#include <scraping/scrapers/thegamedb/TheGameDBEngine.h>
#include "ScraperFactory.h"

ScraperFactory::~ScraperFactory()
{
  for(auto scraper : mScrapers)
    delete scraper.second;
}

IScraperEngine* ScraperFactory::Get(ScraperFactory::ScraperType type)
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
  // Get
  IScraperEngine* engine = Get(GetScraperType(scraperidentifier));
  // (re)Initialize
  engine->Initialize();

  return engine;
}

const std::vector<std::string>& ScraperFactory::GetScraperList()
{
  static std::vector<std::string> _List =
  {
    "Screenscraper",
    //"TheGamesDB",
  };
  return _List;
}

ScraperFactory::ScraperType ScraperFactory::GetScraperType(const std::string& scraperidentifier)
{
  // Default scraper
  ScraperType type = ScraperType::ScreenScraper;

  // Identify
  if (scraperidentifier == "Screenscraper") type = ScraperType::ScreenScraper;
  else if (scraperidentifier == "TheGamesDB") type = ScraperType::TheGameDB;

  return type;
}

void ScraperFactory::ExtractFileNameUndecorated(FileData& game)
{
  std::string name = game.RomPath().FilenameWithoutExtension();

  // Remove (text)
  for(unsigned long pos = 0; (pos = name.find('(', pos)) != std::string::npos; )
  {
    unsigned long end = name.find(')', pos);
    if (end != std::string::npos)
    {
      name.erase(pos, end - pos + 1);
      while(pos < name.size() && name[pos] == ' ') name.erase(pos, 1);
    }
  }

  // Remove [text]
  for(unsigned long pos = 0; (pos = name.find('(', pos)) != std::string::npos; )
  {
    unsigned long end = name.find(')', pos);
    if (end != std::string::npos)
    {
      name.erase(pos, end - pos + 1);
      while(pos < name.size() && name[pos] == ' ') name.erase(pos, 1);
    }
  }

  game.Metadata().SetName(Strings::Trim(name));
}

void ScraperFactory::ExtractRegionFromFilename(FileData& game)
{
  Regions::GameRegions region = Regions::ExtractRegionsFromFileName(game.RomPath());
  if (region != Regions::GameRegions::Unknown)
    game.Metadata().SetRegion((int)region);
}

