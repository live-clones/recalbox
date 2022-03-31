//
// Created by bkg2k on 17/03/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "ScraperSeamless.h"
#include <systems/SystemManager.h>

// TODO
// - [X] Fix crash when stopping the scraper
// - [X] Fix freeze on server errors
// - [X] Use Recalbox endpoints on regular scraper also
// - [X] Move recalbox credential in configuration file
// - [X] Refresh game name in the list
// - [X] force videos normalisées
// - [X] Add stat querystring to media download
// - [X] Background scraping
// - [X] Freeze background scrapper when running games
// - [X] menu d'utilisation disque ?
// - [X] popup si plus beaucoup de place
// - [X] Don't scrape ReadOnly-roots
// - [X] Don't start scraping if no system is selected
// - [ ] Remove new scraping methods & use timestamp in regular scraper too
// - [ ] menu pour wiper les videos si plus de place
// - [ ] enlever le loading... quand l'image est chargée
// - [ ] Round robin/failover via DNS

ScraperSeamless::ScraperSeamless()
  : StaticLifeCycleControler<ScraperSeamless>("SeemlessScrape")
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
  // Authentication is done on every request
  // But we do a pre-authent to not run the background scraper on non-registered users
  if ((mAuthenticated = Authenticate()))
  {
    // Run the thread pool for texture loading
    mPool.Run(sScrapingEngineCount, true);

    { LOG(LogInfo) << "[SeamlessScraping] SeamlessScraper started"; }
  }
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
  bool result = false;
  std::string output;
  Http authenticate;

  authenticate.SetBasicAuth(GetLogin(), GetPassword());
  if (authenticate.Execute(mEndPoints.GetUserInfoUrl(Strings::Empty, Strings::Empty), output))
  {
    result = authenticate.GetLastHttpResponseCode() == 200;
    { LOG(LogInfo) << "[SeamlessScraping] " << (result ? "Successfully" : "Not") << " authenticated"; }
  }
  else { LOG(LogInfo) << "[SeamlessScraping] Authentication request failed"; }

  return result;
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
  if (mAuthenticated && !game->TopAncestor().ReadOnly())
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
