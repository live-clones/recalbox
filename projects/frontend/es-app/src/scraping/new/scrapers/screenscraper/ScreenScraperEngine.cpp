//
// Created by bkg2k on 04/12/2019.
//

#include "ScreenScraperEngine.h"

void ScreenScraperEngine::Initialize()
{

}

bool ScreenScraperEngine::RunOn(ScrappingMethod method, const SystemManager::SystemList& systemList,
                                INotifyScrapeResult* notifyTarget)
{
  return false;
}

bool ScreenScraperEngine::Abort()
{
  return false;
}
