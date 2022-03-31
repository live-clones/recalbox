//
// Created by bkg2k on 04/12/2019.
//

#include <scraping/scrapers/thegamedb/TheGameDBEngine.h>
#include "ScraperFactory.h"
#include <scraping/scrapers/screenscraper/ScreenScraperEngineImplementation.h>
#include <scraping/scrapers/recalbox/RecalboxEngineImplementation.h>
#include <scraping/scrapers/IScraperEngineFreezer.h>
#include <scraping/ScraperSeamless.h>

ScraperFactory::~ScraperFactory()
{
  for(auto scraper : mScrapers)
    delete scraper.second;
}

IScraperEngine* ScraperFactory::Get(ScraperType type, IScraperEngineFreezer* freezer)
{
  // Ensure valid type
  switch(type)
  {
    case ScraperType::ScreenScraper:
    case ScraperType::Recalbox:
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
    case ScraperType::ScreenScraper: result = new ScreenScraperEngineImplementation(freezer); break;
    case ScraperType::Recalbox     : result = new RecalboxEngineImplementation(freezer); break;
    case ScraperType::TheGameDB    : result = new TheGameDBEngine(freezer); break;
  }
  mScrapers[type] = result;
  return result;
}

IScraperEngine* ScraperFactory::GetScraper(ScraperType scraper, IScraperEngineFreezer* freezer)
{
  // Get
  IScraperEngine* engine = Get(scraper, freezer);
  // (re)Initialize
  engine->Initialize();

  return engine;
}

const HashMap<ScraperType, std::string>& ScraperFactory::GetScraperList()
{
  static HashMap<ScraperType, std::string> _List =
  {
    { ScraperType::ScreenScraper, "ScreenScraper" },
    //{ ScraperType::TheGameDB, "TheGamesDB" },
  };
  static bool RecalboxChecked = false;
  if (!RecalboxChecked && ScraperSeamless::Instance().IsAuthenticated())
  {
    _List[ScraperType::Recalbox] = "Recalbox";
    RecalboxChecked = true;
  }
  return _List;
}

void ScraperFactory::ExtractFileNameUndecorated(FileData& game)
{
  std::string name = game.FilePath().FilenameWithoutExtension();

  // Remove (text)
  bool found = false;
  for(unsigned long pos = 0; (pos = name.find('(', pos)) != std::string::npos; )
  {
    unsigned long end = name.find(')', pos);
    if (end == std::string::npos) end = name.size() - 1;
    name.erase(pos, end - pos + 1);
    found = true;
  }

  // Remove [text]
  for(unsigned long pos = 0; (pos = name.find('(', pos)) != std::string::npos; )
  {
    unsigned long end = name.find(')', pos);
    if (end == std::string::npos) end = name.size() - 1;
    name.erase(pos, end - pos + 1);
    found = true;
  }

  if (found)
    name = Strings::Trim(name);

  game.Metadata().SetName(Strings::Trim(name));
}

void ScraperFactory::ExtractRegionFromFilename(FileData& game)
{
  Regions::GameRegions region = Regions::ExtractRegionsFromFileName(game.FilePath());
  if (region != Regions::GameRegions::Unknown)
    game.Metadata().SetRegion((int)region);
}

