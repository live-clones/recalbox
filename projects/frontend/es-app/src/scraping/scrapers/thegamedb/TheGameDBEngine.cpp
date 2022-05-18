//
// Created by bkg2k on 04/12/2019.
//

#include "TheGameDBEngine.h"

void TheGameDBEngine::Initialize()
{

}

bool TheGameDBEngine::RunOn(ScrapingMethod method, const SystemManager::SystemList& systemList,
                            INotifyScrapeResult* notifyTarget, long long diskMinimumFree)
{
  (void)method;
  (void)systemList;
  (void)notifyTarget;
  (void)diskMinimumFree;

  return false;
}

bool TheGameDBEngine::RunOn(ScrapingMethod method, FileData& singleGame, INotifyScrapeResult* notifyTarget,
                            long long diskMinimumFree)
{
  (void)method;
  (void)singleGame;
  (void)notifyTarget;
  (void)diskMinimumFree;

  return false;
}

bool TheGameDBEngine::Abort(bool waitforcompletion)
{
  (void)waitforcompletion;
  return false;
}

bool TheGameDBEngine::IsRunning() const
{
  return false;
}
