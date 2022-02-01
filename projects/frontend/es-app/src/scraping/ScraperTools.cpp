//
// Created by bkg2k on 09/02/2020.
//
#include "ScraperTools.h"

ScraperNameOptions ScraperTools::Clamp(int option)
{
  if (option < (int) ScraperNameOptions::GetFromScraper) return ScraperNameOptions::GetFromScraper;
  if (option > (int) ScraperNameOptions::GetFromFilenameUndecorated)
    return ScraperNameOptions::GetFromFilenameUndecorated;
  return (ScraperNameOptions) option;
}
ScraperNameOptions ScraperTools::ScraperNameOptionsFromString(const std::string& menu)
{
  if (menu == "scraper") return ScraperNameOptions::GetFromScraper;
  if (menu == "filename") return ScraperNameOptions::GetFromFilename;
  return ScraperNameOptions::GetFromFilenameUndecorated;
}

const std::string& ScraperTools::ScraperNameOptionsFromEnum(ScraperNameOptions scraperNameOptions)
{
  switch (scraperNameOptions)
  {
    case ScraperNameOptions::GetFromScraper:
    {
      static std::string sScraper = "scraper";
      return sScraper;
    }

    case ScraperNameOptions::GetFromFilename:
    {
      static std::string sFileName = "filename";
      return sFileName;
    }
    case ScraperNameOptions::GetFromFilenameUndecorated:
    {
      break;
    }
  }
  static std::string sFileName = "filenameUndecorated";
  return sFileName;
}