//
// Created by bkg2k on 17/03/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//

#include "ScreenScraperSingleEngine.h"
#include <utils/Zip.h>
#include <utils/hash/Md5.h>
#include <utils/Files.h>
#include <utils/Log.h>
#include <systems/SystemData.h>

std::string ScreenScraperSingleEngine::sImageSubFolder("images");
std::string ScreenScraperSingleEngine::sThumbnailSubFolder("thumbnails");
std::string ScreenScraperSingleEngine::sVideoSubFolder("videos");
std::string ScreenScraperSingleEngine::sWheelSubFolder("wheels");
std::string ScreenScraperSingleEngine::sMarqueeSubFolder("marquees");
std::string ScreenScraperSingleEngine::sManualSubFolder("manuals");
std::string ScreenScraperSingleEngine::sMapSubFolder("maps");

void ScreenScraperSingleEngine::Initialize(bool noabort)
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

ScrapeResult ScreenScraperSingleEngine::Scrape(ScrapingMethod method, FileData& game, ProtectedSet& md5Set)
{
  // Start
  mRunning = true;

  // Result
  ScrapeResult result = ScrapeResult::NotScraped;

  // Read only?
  if (game.TopAncestor().ReadOnly())
    return ScrapeResult::NotScraped;

  // Check if the game needs to be scraped
  if (NeedScraping(method, game))
    // Dummy loop, only there to be exited from everywhere
    for(;mRunning && !mAbortRequest;)
    {
      // Default return status
      ScreenScraperApis::Game gameResult;
      gameResult.mResult = ScrapeResult::NotFound;

      { LOG(LogDebug) << "[ScreenScraper] Start scraping data for " << game.FilePath().ToString(); }
      if (mAbortRequest) break;

      // Get file size
      long long size = game.FilePath().Size();
      if (size <= 0) break;
      if (mAbortRequest) break;

      // Zip request
      switch (result = RequestZipGameInfo(gameResult, game, size))
      {
        case ScrapeResult::QuotaReached:
        case ScrapeResult::DiskFull:
        case ScrapeResult::FatalError: mAbortRequest = true; break; // General abort
        case ScrapeResult::NotScraped:
        case ScrapeResult::Ok: break;
        case ScrapeResult::NotFound:
        {
          // Normal file request
          switch (result = RequestGameInfo(gameResult, game, size))
          {
            case ScrapeResult::QuotaReached:
            case ScrapeResult::DiskFull:
            case ScrapeResult::FatalError: mAbortRequest = false; break; // General abort
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
        if (mStageInterface != nullptr)
          mStageInterface->StageCompleted(&game, IScraperEngineStage::Stage::Text);
        if (mAbortRequest) break;

        // Request and store media
        switch (DownloadAndStoreMedia(method, gameResult, game, md5Set))
        {
          case ScrapeResult::QuotaReached:
          case ScrapeResult::DiskFull:
          case ScrapeResult::FatalError: mAbortRequest = true; break; // General abort
          case ScrapeResult::NotFound:
          case ScrapeResult::NotScraped:
          case ScrapeResult::Ok: break;
        }
        if (mAbortRequest) break;
      }

      break;
    }

  // EOS
  if (mStageInterface != nullptr)
    mStageInterface->StageCompleted(&game, IScraperEngineStage::Stage::Completed);

  // Stop
  game.Metadata().SetTimeStamp();
  mRunning = false;
  return result;
}

std::string ScreenScraperSingleEngine::ComputeMD5(const Path& path)
{
  FILE* f = fopen(path.ToChars(), "rb");
  if (f != nullptr)
  {
    MD5 md5;
    char buffer[1 << 20]; // 1Mb buffer
    for(int read = 0; (read = (int)fread(buffer, 1, sizeof(buffer), f)) > 0; )
      md5.update(buffer, read);
    fclose(f);
    md5.finalize();
    return md5.hexdigest();
  }
  return std::string();
}

ScrapeResult ScreenScraperSingleEngine::RequestGameInfo(ScreenScraperApis::Game& result, const FileData& game, long long size)
{
  // Get MD5
  std::string md5 = (size < sMaxMd5Calculation) ? ComputeMD5(game.FilePath()) : std::string();
  { LOG(LogDebug) << "[ScreenScraper] MD5 of " << game.FilePath().ToString() << " : " << md5; }

  // Get crc32
  std::string crc32;
  if (game.Metadata().RomCrc32() != 0) game.Metadata().RomCrc32AsString();

  // Call!
  if (!mAbortRequest)
    result = mCaller.GetGameInformation(game, crc32, md5, size);

  return result.mResult;
}

ScrapeResult ScreenScraperSingleEngine::RequestZipGameInfo(ScreenScraperApis::Game& result, const FileData& game, long long size)
{
  if (Strings::ToLowerASCII(game.FilePath().Extension()) == ".zip")
  {
    Zip zip(game.FilePath());
    if (zip.Count() == 1) // Ignore multi-file archives
    {
      // Get real name
      Path filePath = zip.FileName(0);

      // Get MD5
      std::string md5 = zip.Md5(0);
      { LOG(LogDebug) << "[ScreenScraper] MD5 of " << filePath.ToString() << " [" << game.FilePath().ToString() << "] : " << md5; }

      // Get crc32
      int crc32i = zip.Crc32(0);
      std::string crc32 = Strings::ToHexa(crc32i, 8);

      // Call!
      if (!mAbortRequest)
        result = mCaller.GetGameInformation(game, crc32, md5, size);
    }
  }

  return result.mResult;
}

bool ScreenScraperSingleEngine::NeedScraping(ScrapingMethod method, FileData& game)
{
  const Path rootMediaPath = game.TopAncestor().FilePath() / "media";
  const std::string gameFile = game.FilePath().FilenameWithoutExtension();
  switch(method)
  {
    case ScrapingMethod::All: return true;
    case ScrapingMethod::AllIfNoithingExists:
    {
      // Any missing text info = scrape
      if (!game.Metadata().Description().empty()) return false;
      if (game.Metadata().ReleaseDateEpoc() != 0) return false;
      if (!game.Metadata().Developer().empty()) return false;
      if (!game.Metadata().Publisher().empty()) return false;
      if (!game.Metadata().Publisher().empty()) return false;
      if (game.Metadata().GenreId() != GameGenres::None) return false;
      if (game.Metadata().RomCrc32() != 0) return false;
      if (game.Metadata().Region().HasRegion()) return false;

      // Check required and missing media
      if (mConfiguration.GetImageType() != ScreenScraperEnums::ScreenScraperImageType::None)
        if (!game.Metadata().Image().IsEmpty()) return false;
      if (mConfiguration.GetThumbnailType() != ScreenScraperEnums::ScreenScraperImageType::None)
        if (!game.Metadata().Thumbnail().IsEmpty()) return false;
      if (mConfiguration.GetVideo() != ScreenScraperEnums::ScreenScraperVideoType::None)
        if (!game.Metadata().Video().IsEmpty()) return false;
      if (mConfiguration.GetWantMarquee())
      {
        Path marqueePath = rootMediaPath / sMarqueeSubFolder;
        if ((marqueePath / (gameFile + ".jpg")).Exists() ||
            (marqueePath / (gameFile + ".png")).Exists()) return false;
      }
      if (mConfiguration.GetWantWheel())
      {
        Path wheelPath = rootMediaPath / sWheelSubFolder;
        if ((wheelPath / (gameFile + ".jpg")).Exists() ||
            (wheelPath / (gameFile + ".png")).Exists()) return false;
      }
      if (mConfiguration.GetWantManual())
      {
        Path manuelPath = rootMediaPath / sManualSubFolder;
        if ((manuelPath / (gameFile + ".pdf")).Exists()) return false;
      }
      if (mConfiguration.GetWantMaps())
      {
        Path mapPath = rootMediaPath / sMapSubFolder;
        if ((mapPath / (gameFile + ".jpg")).Exists() ||
            (mapPath / (gameFile + ".png")).Exists()) return false;
      }
      if (mConfiguration.GetWantP2K())
      {
        if (game.P2KPath().Exists()) return false;
      }
      // TODO: Add more media checks here
      return true;
    }
    case ScrapingMethod::CompleteAndKeepExisting:
    case ScrapingMethod::CompleteAndReplaceExisting:
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
      if (!game.Metadata().Region().HasRegion()) return true;

      // Check required and missing media
      if (mConfiguration.GetImageType() != ScreenScraperEnums::ScreenScraperImageType::None)
        if (game.Metadata().Image().IsEmpty()) return true;
      if (mConfiguration.GetThumbnailType() != ScreenScraperEnums::ScreenScraperImageType::None)
        if (game.Metadata().Thumbnail().IsEmpty()) return true;
      if (mConfiguration.GetVideo() != ScreenScraperEnums::ScreenScraperVideoType::None)
        if (game.Metadata().Video().IsEmpty()) return true;
      if (mConfiguration.GetWantMarquee())
      {
        Path marqueePath = rootMediaPath / sMarqueeSubFolder;
        if ((!(marqueePath / (gameFile + ".jpg")).Exists()) &&
            (!(marqueePath / (gameFile + ".png")).Exists())) return true;
      }
      if (mConfiguration.GetWantWheel())
      {
        Path wheelPath = rootMediaPath / sWheelSubFolder;
        if ((!(wheelPath / (gameFile + ".jpg")).Exists()) &&
            (!(wheelPath / (gameFile + ".png")).Exists())) return true;
      }
      if (mConfiguration.GetWantManual())
      {
        Path manuelPath = rootMediaPath / sManualSubFolder;
        if (!(manuelPath / (gameFile + ".pdf")).Exists()) return true;
      }
      if (mConfiguration.GetWantMaps())
      {
        Path mapPath = rootMediaPath / sMapSubFolder;
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
ScreenScraperSingleEngine::StoreTextData(ScrapingMethod method, const ScreenScraperApis::Game& sourceData, FileData& game)
{
  bool noKeep = method != ScrapingMethod::CompleteAndKeepExisting;
  // Name always scraped
  if (!sourceData.mName.empty())
  {
    game.Metadata().SetName(sourceData.mName);
    mTextInfo++;
  }
  // Store data only if they are not empty and not scraped if method is IncompleteKeep
  if (!sourceData.mSynopsis.empty())
    if (game.Metadata().Description().empty() || noKeep)
    {
      game.Metadata().SetDescription(sourceData.mSynopsis);
      mTextInfo++;
    }
  if (!sourceData.mPublisher.empty())
    if (game.Metadata().Publisher().empty() || noKeep)
    {
      game.Metadata().SetPublisher(sourceData.mPublisher);
      mTextInfo++;
    }
  if (!sourceData.mDeveloper.empty())
    if (game.Metadata().Developer().empty() || noKeep)
    {
      game.Metadata().SetDeveloper(sourceData.mDeveloper);
      mTextInfo++;
    }
  if (!sourceData.mPlayers.empty())
    if (game.Metadata().PlayerRange() == 0 || noKeep)
    {
      game.Metadata().SetPlayersAsString(sourceData.mPlayers);
      mTextInfo++;
    }
  if (sourceData.mReleaseDate.ToEpochTime() != 0)
    if (game.Metadata().ReleaseDateEpoc() == 0 || noKeep)
    {
      game.Metadata().SetReleaseDate(sourceData.mReleaseDate);
      mTextInfo++;
    }
  if (sourceData.mRating != 0.0f)
    if (game.Metadata().Rating() == 0.0f || noKeep)
    {
      game.Metadata().SetRating(sourceData.mRating);
      mTextInfo++;
    }
  if (!sourceData.mGenre.empty())
    if (game.Metadata().Genre().empty() || noKeep)
    {
      game.Metadata().SetGenre(sourceData.mGenre);
      mTextInfo++;
    }
  if (sourceData.mGenreId !=  GameGenres::None)
    if (game.Metadata().GenreId() == GameGenres::None || noKeep)
    {
      game.Metadata().SetGenreId(sourceData.mGenreId);
      game.Metadata().SetAdult(sourceData.mAdult);
      mTextInfo++;
    }
  if (!sourceData.mRegion.empty())
    if (!game.Metadata().Region().HasRegion() || noKeep)
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
    if (!p2kPath.Exists() || noKeep)
    {
      Files::SaveFile(p2kPath, sourceData.mP2k);
      mTextInfo++;
    }
  }
}

ScrapeResult ScreenScraperSingleEngine::DownloadMedia(const Path& AbsoluteImagePath, FileData& game,
                                                      const std::string& media, SetPathMethodType pathSetter, ProtectedSet& md5Set, MediaType mediaType)
{
  bool mediaIsPresent = md5Set.Exists(AbsoluteImagePath.ToString());
  if (mediaIsPresent)
  {
    if (pathSetter != nullptr)
      (game.Metadata().*pathSetter)(AbsoluteImagePath);
    return ScrapeResult::Ok;
  }
  if (!media.empty())
  {
    long long size = 0;

    switch(mCaller.GetMedia(game, media, AbsoluteImagePath, size))
    {
      case ScrapeResult::Ok:
      {
        if (pathSetter != nullptr)
          (game.Metadata().*pathSetter)(AbsoluteImagePath);
        if(mediaType == MediaType::Image) mImages++;
        if(mediaType == MediaType::Video) mVideos++;
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
ScreenScraperSingleEngine::DownloadAndStoreMedia(FileData& game, bool noKeep, const Path& target, const std::string& subPath,
                                                 const std::string& name, MediaType mediaType, SetPathMethodType pathSetter,
                                                 const ScreenScraperApis::Game::MediaUrl::Media& mediaSource, ProtectedSet& md5Set)
{
  Path path = target / subPath / std::string(name).append(1, ' ').append(mediaSource.mMd5).append(1, '.').append(mediaSource.mFormat);
  bool exists = path.Exists();
  if (!exists || noKeep)
    return DownloadMedia(path, game, mediaSource.mUrl, pathSetter, md5Set, mediaType);

  if (pathSetter != nullptr)
    (game.Metadata().*pathSetter)(path);
  return ScrapeResult::Ok;
}

bool ScreenScraperSingleEngine::IsFatal(ScrapeResult result) const
{
  switch(result)
  {
    case ScrapeResult::QuotaReached:
    case ScrapeResult::DiskFull:
    case ScrapeResult::FatalError: return true;
    case ScrapeResult::Ok:
    case ScrapeResult::NotScraped:
    case ScrapeResult::NotFound:
    default: break;
  }
  return mAbortRequest;
}

ScrapeResult
ScreenScraperSingleEngine::DownloadAndStoreMedia(ScrapingMethod method, const ScreenScraperApis::Game& sourceData,
                                                 FileData& game, ProtectedSet& md5Set)
{
  bool ok = false;
  const Path rootFolder(game.TopAncestor().FilePath());
  const Path relativePath = game.FilePath().MakeRelative(rootFolder, ok);
  const std::string gameName = ok ? (relativePath.Directory() / game.FilePath().FilenameWithoutExtension()).ToString()
                                  : game.FilePath().FilenameWithoutExtension();
  const Path mediaFolder = rootFolder / "media";
  bool noKeep = method != ScrapingMethod::CompleteAndKeepExisting;

  // Main image
  ScrapeResult result = DownloadAndStoreMedia(game, noKeep, mediaFolder, sImageSubFolder, sourceData.mScreenScraperName, MediaType::Image, &MetadataDescriptor::SetImagePath, sourceData.MediaSources.mImage, md5Set);
  if (IsFatal(result)) return result;

  // Thumbnail image
  result = DownloadAndStoreMedia(game, noKeep, mediaFolder, sThumbnailSubFolder, sourceData.mScreenScraperName, MediaType::Image, &MetadataDescriptor::SetThumbnailPath, sourceData.MediaSources.mThumbnail, md5Set);
  if (IsFatal(result)) return result;

  // Notify main images
  if (mStageInterface != nullptr) mStageInterface->StageCompleted(&game, IScraperEngineStage::Stage::Images);

  // Video
  result = DownloadAndStoreMedia(game, noKeep, mediaFolder, sVideoSubFolder, sourceData.mScreenScraperName, MediaType::Video, &MetadataDescriptor::SetVideoPath, sourceData.MediaSources.mVideo, md5Set);
  if (IsFatal(result)) return result;

  // Notify video
  if (mStageInterface != nullptr) mStageInterface->StageCompleted(&game, IScraperEngineStage::Stage::Video);

  // Marquee
  result = DownloadAndStoreMedia(game, noKeep, mediaFolder, sMarqueeSubFolder, sourceData.mScreenScraperName, MediaType::Image, nullptr, sourceData.MediaSources.mMarquee, md5Set);
  if (IsFatal(result)) return result;

  // Wheels
  result = DownloadAndStoreMedia(game, noKeep, mediaFolder, sWheelSubFolder, sourceData.mScreenScraperName, MediaType::Image, nullptr, sourceData.MediaSources.mWheel, md5Set);
  if (IsFatal(result)) return result;

  // Manual
  result = DownloadAndStoreMedia(game, noKeep, mediaFolder, sManualSubFolder, sourceData.mScreenScraperName, MediaType::Image, nullptr, sourceData.MediaSources.mManual, md5Set);
  if (IsFatal(result)) return result;

  // Maps
  result = DownloadAndStoreMedia(game, noKeep, mediaFolder, sMapSubFolder, sourceData.mScreenScraperName, MediaType::Image, nullptr, sourceData.MediaSources.mMaps, md5Set);
  if (IsFatal(result)) return result;

  // TODO: Add more image download & save here
  return ScrapeResult::Ok;
}
