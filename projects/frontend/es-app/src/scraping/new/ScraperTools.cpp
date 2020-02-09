//
// Created by bkg2k on 09/02/2020.
//
#include "ScraperTools.h"

ScraperNameOptions ScraperTools::Clamp(int option)
{
  if (option < (int)ScraperNameOptions::GetFromScraper) return ScraperNameOptions::GetFromScraper;
  if (option > (int)ScraperNameOptions::GetFromFilenameUndecorated) return ScraperNameOptions::GetFromFilenameUndecorated;
  return (ScraperNameOptions)option;
}
