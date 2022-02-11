//
// Created by bkg2k on 04/12/2019.
//

#include <utils/Log.h>
#include <RecalboxConf.h>
#include <utils/hash/Md5.h>
#include <utils/Zip.h>
#include <utils/locale/LocaleHelper.h>
#include <utils/Files.h>
#include "ScreenScraperEngine.h"

ScreenScraperEngine::ScreenScraperEngine()
  : mRunner(this, "Scraper-ssfr", false)
  , mEngines
    {
      Engine(this), Engine(this), Engine(this), Engine(this), Engine(this),
      Engine(this), Engine(this), Engine(this), Engine(this), Engine(this),
      Engine(this), Engine(this), Engine(this), Engine(this), Engine(this)
    }
  , mAllocatedEngines(0)
  , mMethod(ScrappingMethod::All)
  , mNotifier(nullptr)
  , mDiskMinimumFree(0)
  , mLanguage(Languages::Unknown)
  , mRegion(Regions::GameRegions::Unknown)
  , mMainImage(ScreenScraperEnums::ScreenScraperImageType::MixV1)
  , mThumbnailImage(ScreenScraperEnums::ScreenScraperImageType::None)
  , mVideo(ScreenScraperEnums::ScreenScraperVideoType::None)
  , mWantMarquee(false)
  , mWantWheel(false)
  , mWantManual(false)
  , mWantMaps(false)
  , mWantP2K(false)
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
{
}

void ScreenScraperEngine::Initialize()
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
  mMethod = ScrappingMethod::All;
  mNotifier = nullptr;

  // Reset live stats
  mTotal = 0;
  mCount = 0;

    // Credentials
  mLogin = Strings::Trim(RecalboxConf::Instance().GetScreenScraperLogin());
  mPassword = Strings::Trim(RecalboxConf::Instance().GetScreenScraperPassword());

  // Language & region
  mRegion = RecalboxConf::Instance().GetScreenScraperRegion();
  mLanguage = LanguagesTools::GetScrapingLanguage();

  // Medias
  mMainImage = RecalboxConf::Instance().GetScreenScraperMainMedia();
  mThumbnailImage = RecalboxConf::Instance().GetScreenScraperThumbnail();
  mVideo = RecalboxConf::Instance().GetScreenScraperVideo();
  mWantMarquee = RecalboxConf::Instance().GetScreenScraperWantMarquee();
  mWantWheel   = RecalboxConf::Instance().GetScreenScraperWantWheel();
  mWantManual  = RecalboxConf::Instance().GetScreenScraperWantManual();
  mWantMaps    = RecalboxConf::Instance().GetScreenScraperWantMaps();
  mWantP2K     = RecalboxConf::Instance().GetScreenScraperWantP2K();
  mMd5Set = MutexSet{};
}

bool ScreenScraperEngine::RunOn(ScrappingMethod method, FileData& singleGame, INotifyScrapeResult* notifyTarget,
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


bool ScreenScraperEngine::RunOn(ScrappingMethod method, const SystemManager::SystemList& systemList,
                                INotifyScrapeResult* notifyTarget, long long diskMinimumFree)
{
  { LOG(LogInfo) << "[ScreenScraper] Starting new multi-system scraping session..."; }

  mNotifier = notifyTarget;
  mMethod = method;
  mDiskMinimumFree = diskMinimumFree;

  // Get screenscraper's thread
  ScreenScraperApis apis(this);
  int threadCount = apis.GetUserInformation().mThreads;
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

bool ScreenScraperEngine::ThreadPoolRunJob(FileData*& feed)
{
  int engineIndex = ObtainEngine();
  if (engineIndex >= 0)
  {
    // Process the scrape in this thread, allocated and fired automatically by the threadpool
    { LOG(LogDebug) << "[ScreenScraper] Got engine #" << engineIndex << " for " << feed->RomPath().ToString(); }
    Engine& engine = mEngines[engineIndex];
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
        case ScrapeResult::FatalError: mStatErrors++; break;
        case ScrapeResult::QuotaReached: Abort(false); mSender.Call((int)ScrapeResult::QuotaReached); break;
        case ScrapeResult::DiskFull: Abort(false); mSender.Call((int)ScrapeResult::DiskFull); break;
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

void ScreenScraperEngine::ReceiveSyncCallback(const SDL_Event& event)
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
        break;
      }
    }
  }
}

int ScreenScraperEngine::ObtainEngine()
{
  int result = -1;
  while(result < 0)
  {
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

void ScreenScraperEngine::RecycleEngine(int index)
{
  if (mEngineMutex.Lock())
  {
    mAllocatedEngines &= ~(1 << index);
    mEngineMutex.UnLock();
    mEngineSignal.Fire();
  }
}

void ScreenScraperEngine::Engine::Initialize(bool noabort)
{
  mRunning = false;
  if (!noabort)
    mAbortRequest = false;
  mQuotaReached = false;
  mTextInfo = 0;
  mImages = 0;
  mVideos = 0;
  mMediaSize = 0;
}

ScrapeResult ScreenScraperEngine::Engine::Scrape(ScrappingMethod method, FileData& game, MutexSet& md5Set)
{
  // Start
  mRunning = true;

  // Result
  ScrapeResult result = ScrapeResult::NotScraped;

  // Check if the game needs to be scraped
  if (NeedScrapping(method, game))
    // Dummy loop, only there to be exited from everywhere
    for(;mRunning && !mAbortRequest;)
    {
      // Default return status
      ScreenScraperApis::Game gameResult;
      gameResult.mResult = ScrapeResult::NotFound;

      { LOG(LogDebug) << "[ScreenScraper] Start scraping data for " << game.RomPath().ToString(); }
      if (mAbortRequest) break;

      // Get file size
      long long size = game.RomPath().Size();
      if (size <= 0) break;
      if (mAbortRequest) break;

      // Zip request
      switch (result = RequestZipGameInfo(gameResult, game, size))
      {
        case ScrapeResult::QuotaReached:
        case ScrapeResult::DiskFull:
        case ScrapeResult::FatalError: mRunning = false; return result; // General abort
        case ScrapeResult::NotScraped:
        case ScrapeResult::Ok: break;
        case ScrapeResult::NotFound:
        {
          // Normal file request
          switch (result = RequestGameInfo(gameResult, game, size))
          {
            case ScrapeResult::QuotaReached:
            case ScrapeResult::DiskFull:
            case ScrapeResult::FatalError: mRunning = false; return result; // General abort
            case ScrapeResult::NotFound:
            case ScrapeResult::NotScraped:
            case ScrapeResult::Ok: break;
          }
        }
      }
      if (mAbortRequest) break;

      // Something found?
      if (gameResult.mResult == ScrapeResult::Ok)
      {
        // Store text data
        StoreTextData(method, gameResult, game);
        if (mAbortRequest) break;

        // Request and store media
        switch (DownloadAndStoreMedia(method, gameResult, game, md5Set))
        {
          case ScrapeResult::QuotaReached:
          case ScrapeResult::DiskFull:
          case ScrapeResult::FatalError: mRunning = false; return result; // General abort
          case ScrapeResult::NotFound:
          case ScrapeResult::NotScraped:
          case ScrapeResult::Ok: break;
        }
        if (mAbortRequest) break;
      }

      break;
    }

  // Stop
  mRunning = false;
  return result;
}

std::string ScreenScraperEngine::Engine::ComputeMD5(const Path& path)
{
  FILE* f = fopen(path.ToChars(), "rb");
  if (f != nullptr)
  {
    MD5 md5;
    char buffer[1 << 20]; // 1Mb buffer
    for(int read = 0; (read = fread(buffer, 1, sizeof(buffer), f)) > 0; )
      md5.update(buffer, read);
    fclose(f);
    md5.finalize();
    return md5.hexdigest();
  }
  return std::string();
}

ScrapeResult ScreenScraperEngine::Engine::RequestGameInfo(ScreenScraperApis::Game& result, const FileData& game, long long size)
{
  // Get MD5
  Path path(game.RomPath());
  std::string md5 = (size < sMaxMd5Calculation) ? ComputeMD5(path) : std::string();
  { LOG(LogDebug) << "[ScreenScraper] MD5 of " << path.ToString() << " : " << md5; }

  // Get crc32
  std::string crc32;
  if (game.Metadata().RomCrc32() != 0) game.Metadata().RomCrc32AsString();

  if (!mAbortRequest)
  {
    int ssid = game.System().Descriptor().ScreenScaperID();
    // Call!
    result = mCaller.GetGameInformation(ssid, path, crc32, md5, size);
  }

  return result.mResult;
}

ScrapeResult ScreenScraperEngine::Engine::RequestZipGameInfo(ScreenScraperApis::Game& result, const FileData& game, long long size)
{
  Path path(game.RomPath());
  if (Strings::ToLowerASCII(path.Extension()) == ".zip")
  {
    Zip zip(path);
    if (zip.Count() == 1) // Ignore multi-file archives
    {
      // Get real name
      Path filePath = zip.FileName(0);

      // Get MD5
      std::string md5 = zip.Md5(0);
      { LOG(LogDebug) << "[ScreenScraper] MD5 of " << filePath.ToString() << " [" << path.ToString() << "] : " << md5; }

      // Get crc32
      int crc32i = zip.Crc32(0);
      std::string crc32 = Strings::ToHexa(crc32i, 8);

      // Lookup system
      if (!mAbortRequest)
      {
        // Convert platform ID
        int ssid = game.System().Descriptor().ScreenScaperID();

        // Call!
        result = mCaller.GetGameInformation(ssid, filePath, crc32, md5, size);
      }
    }
  }

  return result.mResult;
}

bool ScreenScraperEngine::Engine::NeedScrapping(ScrappingMethod method, FileData& game)
{
  const Path rootMediaPath = game.TopAncestor().RomPath() / "media";
  const std::string gameFile = game.RomPath().FilenameWithoutExtension();
  switch(method)
  {
    case ScrappingMethod::All: return true;
    case ScrappingMethod::IncompleteKeep:
    case ScrappingMethod::IncompleteOverwrite:
    {
      // Any missing text info = scrape
      if (game.Metadata().Name().empty()) return true;
      if (game.Metadata().Description().empty()) return true;
      if (game.Metadata().ReleaseDateEpoc() == 0) return true;
      if (game.Metadata().Developer().empty()) return true;
      if (game.Metadata().Publisher().empty()) return true;
      if (game.Metadata().Publisher().empty()) return true;
      if (game.Metadata().GenreId() == GameGenres::None) return true;
      if (game.Metadata().RomCrc32() == 0) return true;
      if (game.Metadata().Region() == 0) return true;

      // Check required and missing media
      if (mConfiguration.GetImageType() != ScreenScraperEnums::ScreenScraperImageType::None)
        if (game.Metadata().Image().IsEmpty()) return true;
      if (mConfiguration.GetThumbnailType() != ScreenScraperEnums::ScreenScraperImageType::None)
        if (game.Metadata().Thumbnail().IsEmpty()) return true;
      if (mConfiguration.GetVideo() != ScreenScraperEnums::ScreenScraperVideoType::None)
        if (game.Metadata().Video().IsEmpty()) return true;
      if (mConfiguration.GetWantMarquee())
      {
        Path marqueePath = rootMediaPath / "marquees";
        if ((!(marqueePath / (gameFile + ".jpg")).Exists()) &&
            (!(marqueePath / (gameFile + ".png")).Exists())) return true;
      }
      if (mConfiguration.GetWantWheel())
      {
        Path wheelPath = rootMediaPath / "wheels";
        if ((!(wheelPath / (gameFile + ".jpg")).Exists()) &&
            (!(wheelPath / (gameFile + ".png")).Exists())) return true;
      }
      if (mConfiguration.GetWantManual())
      {
        Path manuelPath = rootMediaPath / "manuals";
        if (!(manuelPath / (gameFile + ".pdf")).Exists()) return true;
      }
      if (mConfiguration.GetWantMaps())
      {
        Path mapPath = rootMediaPath / "maps";
        if ((!(mapPath / (gameFile + ".jpg")).Exists()) &&
            (!(mapPath / (gameFile + ".png")).Exists())) return true;
      }
      if (mConfiguration.GetWantP2K())
      {
        if (!game.P2KPath().Exists()) return true;
      }
      // TODO: Add more media checks here
      return false;
    }
    default: break;
  }

  // Unknown method
  { LOG(LogError) << "[ScreenScraper] Unknown scraping method"; }
  return false;
}

void
ScreenScraperEngine::Engine::StoreTextData(ScrappingMethod method, const ScreenScraperApis::Game& sourceData, FileData& game)
{
  // Name always scraped
  if (!sourceData.mName.empty())
  {
    game.Metadata().SetName(sourceData.mName);
    mTextInfo++;
  }
  // Store data only if they are not empty and not scraped if method is IncompleteKeep
  if (!sourceData.mSynopsis.empty())
    if (game.Metadata().Description().empty() || method != ScrappingMethod::IncompleteKeep)
    {
      game.Metadata().SetDescription(sourceData.mSynopsis);
      mTextInfo++;
    }
  if (!sourceData.mPublisher.empty())
    if (game.Metadata().Publisher().empty() || method != ScrappingMethod::IncompleteKeep)
    {
      game.Metadata().SetPublisher(sourceData.mPublisher);
      mTextInfo++;
    }
  if (!sourceData.mDeveloper.empty())
    if (game.Metadata().Developer().empty() || method != ScrappingMethod::IncompleteKeep)
    {
      game.Metadata().SetDeveloper(sourceData.mDeveloper);
      mTextInfo++;
    }
  if (!sourceData.mPlayers.empty())
    if (game.Metadata().PlayerRange() == 0 || method != ScrappingMethod::IncompleteKeep)
    {
      game.Metadata().SetPlayersAsString(sourceData.mPlayers);
      mTextInfo++;
    }
  if (sourceData.mReleaseDate.ToEpochTime() != 0)
    if (game.Metadata().ReleaseDateEpoc() == 0 || method != ScrappingMethod::IncompleteKeep)
    {
      game.Metadata().SetReleaseDate(sourceData.mReleaseDate);
      mTextInfo++;
    }
  if (sourceData.mRating != 0.0f)
    if (game.Metadata().Rating() == 0.0f || method != ScrappingMethod::IncompleteKeep)
    {
      game.Metadata().SetRating(sourceData.mRating);
      mTextInfo++;
    }
  if (!sourceData.mGenre.empty())
    if (game.Metadata().Genre().empty() || method != ScrappingMethod::IncompleteKeep)
    {
      game.Metadata().SetGenre(sourceData.mGenre);
      mTextInfo++;
    }
  if (sourceData.mGenreId !=  GameGenres::None)
    if (game.Metadata().GenreId() == GameGenres::None || method != ScrappingMethod::IncompleteKeep)
    {
      game.Metadata().SetGenreId(sourceData.mGenreId);
      game.Metadata().SetAdult(sourceData.mAdult);
      mTextInfo++;
    }
  if (!sourceData.mRegion.empty())
    if (game.Metadata().Region() == 0 || method != ScrappingMethod::IncompleteKeep)
    {
      game.Metadata().SetRegionAsString(sourceData.mRegion);
      mTextInfo++;
    }
  if (!sourceData.mCrc.empty()) // Always set CRC if not empty
  {
    game.Metadata().SetRomCrc32AsString(sourceData.mCrc);
    mTextInfo++;
  }

  // Store P2k file
  if (!sourceData.mP2k.empty())
  {
    Path p2kPath = game.P2KPath();
    if (!p2kPath.Exists() || method != ScrappingMethod::IncompleteKeep)
    {
      Files::SaveFile(p2kPath, sourceData.mP2k);
      mTextInfo++;
    }
  }
}

ScrapeResult ScreenScraperEngine::Engine::DownloadMedia(const Path& AbsoluteImagePath, FileData& game,
  const std::string& media, SetPathMethodType pathSetter, MutexSet& md5Set, const std::string& mediaType)
{
  md5Set.mMutex.Lock();
  bool mediaIsPresent = md5Set.mSet.contains(AbsoluteImagePath.ToString());
  if (!mediaIsPresent)
  {
    md5Set.mSet.insert(AbsoluteImagePath.ToString());
  }
  md5Set.mMutex.UnLock();
  if(mediaIsPresent)
  {
    if (pathSetter != nullptr)
      (game.Metadata().*pathSetter)(AbsoluteImagePath);
    return ScrapeResult::Ok;
  }
  if (!media.empty())
  {
    long long size = 0;

    switch(mCaller.GetMedia(media, AbsoluteImagePath, size))
    {
      case ScrapeResult::Ok:
      {
        if (pathSetter != nullptr)
          (game.Metadata().*pathSetter)(AbsoluteImagePath);
        if(mediaType == "images" || mediaType == "thumbnails" || mediaType == "wheels" ||
        mediaType == "maps" || mediaType == "marquees")
          mImages++;
        if(mediaType == "videos")
          mVideos++;
        mMediaSize += size;
        break;
      }
      case ScrapeResult::NotScraped: break;
      case ScrapeResult::NotFound: { LOG(LogError) << "[ScreenScraper] Missing media!"; break; }
      case ScrapeResult::DiskFull: return ScrapeResult::DiskFull;
      case ScrapeResult::QuotaReached: return ScrapeResult::QuotaReached;
      case ScrapeResult::FatalError: break;
    }
  }
  return ScrapeResult::Ok;
}

ScrapeResult
ScreenScraperEngine::Engine::DownloadAndStoreMedia(ScrappingMethod method, const ScreenScraperApis::Game& sourceData,
                                                   FileData& game, MutexSet& md5Set)
{
  bool ok = false;
  Path path(game.RomPath());
  const Path rootFolder(game.TopAncestor().RomPath());
  const Path relativePath = path.MakeRelative(rootFolder, ok);
  const std::string gameName = ok ? (relativePath.Directory() / path.FilenameWithoutExtension()).ToString()
                                  : path.FilenameWithoutExtension();
  const Path mediaFolder = rootFolder / "media";

  // Main image
  if (!game.Metadata().Image().Exists() || method != ScrappingMethod::IncompleteKeep)
  {
    Path ThumbnailsAbsoluteImagePath = mediaFolder / "images" / (sourceData.mScreenScraperName + ' ' + sourceData.MediaSources.mImageMd5 + '.' + sourceData.MediaSources.mImageFormat);
    ScrapeResult result = DownloadMedia(ThumbnailsAbsoluteImagePath, game,
      sourceData.MediaSources.mImage, &MetadataDescriptor::SetImagePath, md5Set, "images");
    if (result != ScrapeResult::Ok || mAbortRequest) return result;
  }

  // Thumbnail image
  Path ThumbnailsAbsoluteImagePath = mediaFolder / "thumbnails" / (sourceData.mScreenScraperName + ' ' + sourceData.MediaSources.mThumbnailMd5 + '.' + sourceData.MediaSources.mThumbnailFormat);
  if (!game.Metadata().Thumbnail().Exists() || method != ScrappingMethod::IncompleteKeep)
  {
    ScrapeResult result = DownloadMedia(ThumbnailsAbsoluteImagePath, game,
      sourceData.MediaSources.mThumbnail, &MetadataDescriptor::SetThumbnailPath,md5Set, "thumbnails");
    if (result != ScrapeResult::Ok || mAbortRequest) return result;
  }

  // Video
  Path VideoAbsoluteVideoPath = mediaFolder / "videos" / (sourceData.mScreenScraperName + ' ' + sourceData.MediaSources.mVideoMd5 + '.' + sourceData.MediaSources.mVideoFormat);
  if (!game.Metadata().Video().Exists() || method != ScrappingMethod::IncompleteKeep)
  {
    ScrapeResult result = DownloadMedia(VideoAbsoluteVideoPath, game,
      sourceData.MediaSources.mVideo, &MetadataDescriptor::SetVideoPath, md5Set, "videos");
    if (result != ScrapeResult::Ok || mAbortRequest) return result;
  }

  Path MarqueeAbsoluteImagePath = mediaFolder / "marquees" / (sourceData.mScreenScraperName + ' ' + sourceData.MediaSources.mMarqueeMd5 + '.' + sourceData.MediaSources.mMarqueeFormat);
  if (!MarqueeAbsoluteImagePath.Exists() || method != ScrappingMethod::IncompleteKeep)
  {
    ScrapeResult result = DownloadMedia(MarqueeAbsoluteImagePath, game,
      sourceData.MediaSources.mMarquee, nullptr, md5Set, "marquees");
    if (result != ScrapeResult::Ok || mAbortRequest) return result;
  }

  Path WheelAbsoluteImagePath = mediaFolder / "wheels" / (sourceData.mScreenScraperName + ' ' + sourceData.MediaSources.mWheelMd5 + '.' + sourceData.MediaSources.mWheelFormat);
  if (!WheelAbsoluteImagePath.Exists() || method != ScrappingMethod::IncompleteKeep)
  {
    ScrapeResult result = DownloadMedia(WheelAbsoluteImagePath, game,
      sourceData.MediaSources.mWheel, nullptr, md5Set, "wheels");
    if (result != ScrapeResult::Ok || mAbortRequest) return result;
  }

  Path ManualAbsoluteImagePath = mediaFolder / "manuals" / (sourceData.mScreenScraperName + ' ' + sourceData.MediaSources.mManualMd5 + '.' + sourceData.MediaSources.mManualFormat);
  if (!ManualAbsoluteImagePath.Exists() || method != ScrappingMethod::IncompleteKeep)
  {
    ScrapeResult result = DownloadMedia(ManualAbsoluteImagePath, game,
      sourceData.MediaSources.mManual, nullptr, md5Set, "manuals");
    if (result != ScrapeResult::Ok || mAbortRequest) return result;
  }

  Path MapAbsoluteImagePath = mediaFolder / "maps" / (sourceData.mScreenScraperName + ' ' + sourceData.MediaSources.mMapsMd5 + '.' + sourceData.MediaSources.mMapsFormat);
  if (!MapAbsoluteImagePath.Exists() || method != ScrappingMethod::IncompleteKeep)
  {
    ScrapeResult result = DownloadMedia(MapAbsoluteImagePath, game,
      sourceData.MediaSources.mMaps, nullptr, md5Set, "maps");
    if (result != ScrapeResult::Ok || mAbortRequest) return result;
  }

  // TODO: Add more image download & save here

  return ScrapeResult::Ok;
}