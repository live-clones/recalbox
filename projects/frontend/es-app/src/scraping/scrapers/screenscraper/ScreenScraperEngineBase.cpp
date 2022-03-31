//
// Created by bkg2k on 04/12/2019.
//

#include <utils/Log.h>
#include <RecalboxConf.h>
#include <utils/hash/Md5.h>
#include <utils/locale/LocaleHelper.h>
#include <scraping/scrapers/screenscraper/ScreenScraperEngineBase.h>
#include <scraping/ScraperSeamless.h>

ScreenScraperEngineBase::ScreenScraperEngineBase(IEndPointProvider& endpoint, IScraperEngineFreezer* freezer)
  : mRunner(this, "Scraper-ssfr", false)
  , mEngines
    {
      ScreenScraperSingleEngine(this, &endpoint, nullptr),
      ScreenScraperSingleEngine(this, &endpoint, nullptr),
      ScreenScraperSingleEngine(this, &endpoint, nullptr),
      ScreenScraperSingleEngine(this, &endpoint, nullptr),
      ScreenScraperSingleEngine(this, &endpoint, nullptr),
      ScreenScraperSingleEngine(this, &endpoint, nullptr),
      ScreenScraperSingleEngine(this, &endpoint, nullptr),
      ScreenScraperSingleEngine(this, &endpoint, nullptr),
      ScreenScraperSingleEngine(this, &endpoint, nullptr),
      ScreenScraperSingleEngine(this, &endpoint, nullptr),
      ScreenScraperSingleEngine(this, &endpoint, nullptr),
      ScreenScraperSingleEngine(this, &endpoint, nullptr),
      ScreenScraperSingleEngine(this, &endpoint, nullptr),
      ScreenScraperSingleEngine(this, &endpoint, nullptr),
      ScreenScraperSingleEngine(this, &endpoint, nullptr)
    }
  , mAllocatedEngines(0)
  , mMethod(ScrapingMethod::All)
  , mEndPoint(endpoint)
  , mNotifier(nullptr)
  , mDiskMinimumFree(0)
  , mTotal(0)
  , mCount(0)
  , mStatScraped(0)
  , mStatNotFound(0)
  , mStatErrors(0)
  , mTextInfo(0)
  , mImages(0)
  , mVideos(0)
  , mMediaSize(0)
  , mSender(this)
  , mFreezeInterface(freezer)
{
}

void ScreenScraperEngineBase::Initialize()
{
  // Resets stats
  mStatScraped = 0;
  mStatNotFound = 0;
  mStatErrors = 0;
  mTextInfo = 0;
  mImages = 0;
  mVideos = 0;
  mMediaSize = 0;

  // Reset engines
  mAllocatedEngines = 0;
  mMethod = ScrapingMethod::All;
  mNotifier = nullptr;

  // Reset live stats
  mTotal = 0;
  mCount = 0;

  ResetConfiguration();
}

bool ScreenScraperEngineBase::RunOn(ScrapingMethod method, FileData& singleGame, INotifyScrapeResult* notifyTarget,
                                    long long diskMinimumFree)
{
  { LOG(LogInfo) << "[ScreenScraper] Starting new single game scraping session..."; }

  mNotifier = notifyTarget;
  mMethod = method;
  mDiskMinimumFree = diskMinimumFree;

  // Get screenscraper's thread
  mDatabaseMessage = (_("PLEASE VISIT")).append(" HTTP://WWW.SCREENSCRAPER.FR");
  // Feed threadpool
  mRunner.PushFeed(&singleGame);
  mTotal = mRunner.PendingJobs();
  // Run!
  mRunner.Run(1, true);

  return false;
}


bool ScreenScraperEngineBase::RunOn(ScrapingMethod method, const SystemManager::SystemList& systemList,
                                    INotifyScrapeResult* notifyTarget, long long diskMinimumFree)
{
  { LOG(LogInfo) << "[ScreenScraper] Starting new multi-system scraping session..."; }

  mNotifier = notifyTarget;
  mMethod = method;
  mDiskMinimumFree = diskMinimumFree;

  // Get screenscraper's thread
  const ScreenScraperUser* user = mEndPoint.GetDirectUserObject();
  int threadCount = user != nullptr ? user->mThreads : ScreenScraperApis(this, &mEndPoint).GetUserInformation().mThreads;
  mDatabaseMessage = Strings::Replace(_N("ONLY 1 SCRAPPING ENGINE", "%i SCRAPPING ENGINES", threadCount), "%i", Strings::ToString(threadCount))
                     .append(" - ")
                     .append(_("PLEASE VISIT"))
                     .append(" HTTP://WWW.SCREENSCRAPER.FR");
  // Feed threadpool
  for(SystemData* system : systemList)
    for(FileData* game : system->getAllGames())
      mRunner.PushFeed(game);
  mTotal = mRunner.PendingJobs();
  // Run!
  mRunner.Run(threadCount, true);

  return false;
}

bool ScreenScraperEngineBase::ThreadPoolRunJob(FileData*& feed)
{
  int engineIndex = ObtainEngine();
  if (engineIndex >= 0)
  {
    // Process the scrape in this thread, allocated and fired automatically by the threadpool
    { LOG(LogDebug) << "[ScreenScraper] Got engine #" << engineIndex << " for " << feed->FilePath().ToString(); }
    ScreenScraperSingleEngine& engine = mEngines[engineIndex];
    if (!engine.IsAborted())
    {
      engine.Initialize(true);
      switch(engine.Scrape(mMethod, *feed, mMd5Set))
      {
        case ScrapeResult::Ok:
        {
          mTextInfo += engine.StatsTextInfo();
          mImages += engine.StatsImages();
          mVideos += engine.StatsVideos();
          mMediaSize += engine.StatsMediaSize();
          mStatScraped++;
          break;
        }
        case ScrapeResult::NotScraped: break;
        case ScrapeResult::NotFound: mStatNotFound++; break;
        case ScrapeResult::FatalError: mSender.Call((int)ScrapeResult::FatalError, nullptr); mStatErrors++; break;
        case ScrapeResult::QuotaReached: mSender.Call((int)ScrapeResult::QuotaReached, nullptr); break;
        case ScrapeResult::DiskFull: mSender.Call((int)ScrapeResult::DiskFull, nullptr); break;
      }
      // Then, signal the main thread with the engine number.
      mSender.Call(feed);
    }
    // ... and recycle the Engine
    RecycleEngine(engineIndex);
    return true;
  }
  else { LOG(LogError) << "[ScreenScraper] No more engine available!"; }
  return false;
}

void ScreenScraperEngineBase::ReceiveSyncCallback(const SDL_Event& event)
{
  // Finally, we process the result in the main thread
  FileData* game = (FileData*)event.user.data1;
  if (game != nullptr)
  {
    // Processed
    mCount++;
    // Call completed game notification
    if (mNotifier != nullptr)
      mNotifier->GameResult(mCount, mTotal, game);
    // End of scraping?
    if (mCount == mTotal)
      if (mNotifier != nullptr)
        mNotifier->ScrapingComplete(ScrapeResult::Ok);
  }
  else
  {
    ScrapeResult error = (ScrapeResult)event.user.code;
    switch(error)
    {
      case ScrapeResult::Ok:
      case ScrapeResult::NotScraped:
      case ScrapeResult::NotFound:
      case ScrapeResult::QuotaReached:break;
      case ScrapeResult::DiskFull:
      case ScrapeResult::FatalError:
      {
        if (mNotifier != nullptr)
          mNotifier->ScrapingComplete(error);
        Abort(false);
        break;
      }
    }
  }
}

int ScreenScraperEngineBase::ObtainEngine()
{
  int result = -1;
  while(result < 0)
  {
    // Frozen?
    if (mFreezeInterface != nullptr)
      while(IsRunning() && mFreezeInterface->MustFreeze())
        usleep(2000000);

    // Look for a free engine
    if (mEngineMutex.Lock())
    {
      for (int i = sMaxEngines; --i >= 0;)
        if ((mAllocatedEngines & (1 << i)) == 0)
        {
          mAllocatedEngines |= (1 << i);
          result = i;
          break;
        }
      mEngineMutex.UnLock();
    }
    // Success!
    if (result >= 0) break;

    // Nothing free? Wait...
    if (IsRunning()) mEngineSignal.WaitSignal(200);
  }
  return result;
}

void ScreenScraperEngineBase::RecycleEngine(int index)
{
  if (mEngineMutex.Lock())
  {
    mAllocatedEngines &= ~(1 << index);
    mEngineMutex.UnLock();
    mEngineSignal.Fire();
  }
}
