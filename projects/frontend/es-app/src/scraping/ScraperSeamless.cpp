//
// Created by bkg2k on 17/03/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "ScraperSeamless.h"
#include <systems/SystemManager.h>
#include <patreon/PatronInfo.h>

// TODO
// - [ ] menu pour wiper les videos si plus de place
// - [ ] enlever le loading... quand l'image est chargée
// - [ ] Round robin/failover via DNS

ScraperSeamless::ScraperSeamless()
  : StaticLifeCycleControler<ScraperSeamless>("SeamlessScrape")
  , mConfiguration(RecalboxConf::Instance())
  , mEngines
    {
      ScreenScraperSingleEngine(this, &mEndPoints, this), ScreenScraperSingleEngine(this, &mEndPoints, this),
      ScreenScraperSingleEngine(this, &mEndPoints, this), ScreenScraperSingleEngine(this, &mEndPoints, this),
      ScreenScraperSingleEngine(this, &mEndPoints, this), ScreenScraperSingleEngine(this, &mEndPoints, this),
      ScreenScraperSingleEngine(this, &mEndPoints, this), ScreenScraperSingleEngine(this, &mEndPoints, this)
    }
  , mBusyEngines(0)
  , mSender(this)
  , mPool(this, "SeamlessScrape", true)
{
  // Run the thread pool for texture loading
  mPool.Run(sScrapingEngineCount, true);

  { LOG(LogInfo) << "[SeamlessScraping] SeamlessScraper started"; }
}

FileData* ScraperSeamless::ThreadPoolRunJob(FileData*& feed)
{
  int engineIndex = AllocateEngine();
  if (engineIndex < 0)
  {
    { LOG(LogDebug) << "[SeamlessScraping] SError allocating engine! Scrape aborted!"; }
    return feed;
  }

  { LOG(LogDebug) << "[SeamlessScraping] Start scraping of " << feed->FilePath().ToString() << " using engine " << engineIndex; }

  // Get engine
  ScreenScraperSingleEngine& engine = mEngines[engineIndex];
  // Scrape!
  engine.Scrape(ScrapingMethod::CompleteAndKeepExisting, *feed, mProtectedSet);
  // Free engine so that another scrape can start now
  FreeEngine(engineIndex);

  { LOG(LogDebug) << "[SeamlessScraping] Stop scraping of " << feed->FilePath().ToString(); }

  return feed;
}

bool ScraperSeamless::Authenticate()
{
  return PatronInfo::Instance().IsPatron();
}

void ScraperSeamless::ReceiveSyncCallback(const SDL_Event& event)
{
  FileData* game = (FileData*)event.user.data1;
  IScraperEngineStage::Stage stage = (IScraperEngineStage::Stage)event.user.code;
  IScraperEngineStage* interface = Pop(game, stage == IScraperEngineStage::Stage::Completed);
  if (game != nullptr && interface != nullptr)
    interface->StageCompleted(game, stage);
}

void ScraperSeamless::StageCompleted(FileData* game, IScraperEngineStage::Stage stage)
{
  // Send result to main thread
  mSender.Call((int)stage, (void*)game);
}

void ScraperSeamless::Push(FileData* game, IScraperEngineStage* interface)
{
  if (Authenticate() && !game->TopAncestor().ReadOnly() && RecalboxConf::Instance().GetScraperAuto())
  {
    // Need to run scrape again?
    long long ts = (long long)game->Metadata().TimeStamp();
    if (ts == 0 || (DateTime() - DateTime(ts)).TotalDays() >= 30)
    {
      // Scrape
      Mutex::AutoLock locker(mRunningScrapesLoker);
      if (!mRunningScrapes.contains(game))
      {
        mPool.PushFeed(game);
        mRunningScrapes.insert(game, { SDL_GetTicks(), interface });
      }
    }
  }
}

IScraperEngineStage* ScraperSeamless::Pop(FileData* game, bool remove)
{
  Mutex::AutoLock locker(mRunningScrapesLoker);
  Information* result = mRunningScrapes.try_get(game);
  if (result != nullptr)
  {
    if (remove) mRunningScrapes.erase(game);
    return (*result).Interface;
  }
  { LOG(LogError) << "[SeamlessScraping] Key " << game->FilePath().ToString() << " not found in scraping map!"; }
  return nullptr;
}
