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

/*!
 * List of systems and their IDs from
 * https://www.screenscraper.fr/api/systemesListe.php?devid=xxx&devpassword=yyy&softname=zzz&output=XML
 */
static const HashMap<PlatformIds::PlatformId, int>& GetPlatformIDs()
{
  static const HashMap<PlatformIds::PlatformId, int> screenscraperPlatformidMap
    {
      { PlatformIds::PlatformId::AMSTRAD_CPC,               65},
      { PlatformIds::PlatformId::AMSTRAD_GX4000,            87},
      { PlatformIds::PlatformId::APPLE_II,                  86},
      { PlatformIds::PlatformId::APPLE_IIGS,                217},
      { PlatformIds::PlatformId::APPLE_MACOS,               146},
      { PlatformIds::PlatformId::ARCADE,                    75},
      { PlatformIds::PlatformId::ATARI_8BITS,               43},
      { PlatformIds::PlatformId::ATARI_2600,                26},
      { PlatformIds::PlatformId::ATARI_5200,                40},
      { PlatformIds::PlatformId::ATARI_7800,                41},
      { PlatformIds::PlatformId::ATARI_JAGUAR,              27},
      { PlatformIds::PlatformId::ATARI_JAGUAR_CD,           171},
      { PlatformIds::PlatformId::ATARI_LYNX,                28},
      { PlatformIds::PlatformId::ATARI_ST,                  42},
      { PlatformIds::PlatformId::BANDAI_WONDERSWAN,         45},
      { PlatformIds::PlatformId::BANDAI_WONDERSWAN_COLOR,   46},
      { PlatformIds::PlatformId::COLECOVISION,              48},
      { PlatformIds::PlatformId::COMMODORE_64,              66},
      { PlatformIds::PlatformId::COMMODORE_AMIGA,           64},
      { PlatformIds::PlatformId::COMMODORE_CD32,            130},
      { PlatformIds::PlatformId::COMMODORE_CDTV,            129},
      { PlatformIds::PlatformId::FAIRCHILD_CHANNELF,        80},
      { PlatformIds::PlatformId::GAMEENGINE_DAPHNE,         49},
      { PlatformIds::PlatformId::GAMEENGINE_LUTRO,          206},
      { PlatformIds::PlatformId::GAMEENGINE_OPENBOR,        214},
      { PlatformIds::PlatformId::GAMEENGINE_SCUMMVM,        123},
      { PlatformIds::PlatformId::GAMEENGINE_SOLARUS,        223},
      { PlatformIds::PlatformId::GAMEENGINE_TIC80,          222},
      { PlatformIds::PlatformId::GCE_VECTREX,               102},
      { PlatformIds::PlatformId::IBM_PC,                    135},
      { PlatformIds::PlatformId::MAGNAVOX_ODYSSEY2,         104},
      { PlatformIds::PlatformId::MATTEL_INTELLIVISION,      115},
      { PlatformIds::PlatformId::MGT_SAMCOUPE,              213},
      { PlatformIds::PlatformId::MICROSOFT_MSX,             113},
      { PlatformIds::PlatformId::MICROSOFT_MSX1,            113},
      { PlatformIds::PlatformId::MICROSOFT_MSX2,            116},
      { PlatformIds::PlatformId::MICROSOFT_MSXTURBOR,       118},
      { PlatformIds::PlatformId::MICROSOFT_XBOX,            32},
      { PlatformIds::PlatformId::MICROSOFT_XBOX_360,        33},
      { PlatformIds::PlatformId::NEC_PC88,                  221},
      { PlatformIds::PlatformId::NEC_PC98,                  208},
      { PlatformIds::PlatformId::NEC_PCENGINE,              31},
      { PlatformIds::PlatformId::NEC_PCENGINE_CD,           114},
      { PlatformIds::PlatformId::NEC_PCFX,                  72},
      { PlatformIds::PlatformId::NEC_SUPERGRAFX,            31},
      { PlatformIds::PlatformId::NEOGEO,                    142},
      { PlatformIds::PlatformId::NEOGEO_CD,                 142},
      { PlatformIds::PlatformId::NEOGEO_POCKET,             25},
      { PlatformIds::PlatformId::NEOGEO_POCKET_COLOR,       82},
      { PlatformIds::PlatformId::NINTENDO_3DS,              17},
      { PlatformIds::PlatformId::NINTENDO_64,               14},
      { PlatformIds::PlatformId::NINTENDO_DS,               15},
      { PlatformIds::PlatformId::NINTENDO_FDS,              106},
      { PlatformIds::PlatformId::NINTENDO_GAME_AND_WATCH,   52},
      { PlatformIds::PlatformId::NINTENDO_GAMEBOY,          9},
      { PlatformIds::PlatformId::NINTENDO_GAMEBOY_ADVANCE,  12},
      { PlatformIds::PlatformId::NINTENDO_GAMEBOY_COLOR,    10},
      { PlatformIds::PlatformId::NINTENDO_GAMECUBE,         13},
      { PlatformIds::PlatformId::NINTENDO_NES,              3},
      { PlatformIds::PlatformId::NINTENDO_POKEMINI,         211},
      { PlatformIds::PlatformId::NINTENDO_SATELLAVIEW,      107},
      { PlatformIds::PlatformId::NINTENDO_SNES,             4},
      { PlatformIds::PlatformId::NINTENDO_SUFAMITURBO,      108},
      { PlatformIds::PlatformId::NINTENDO_VIRTUAL_BOY,      11},
      { PlatformIds::PlatformId::NINTENDO_WII,              16},
      { PlatformIds::PlatformId::NINTENDO_WII_U,            18},
      { PlatformIds::PlatformId::OSH_UZEBOX,                216},
      { PlatformIds::PlatformId::PALM_PDA,                  219},
      { PlatformIds::PlatformId::PANASONIC_3DO,             29},
      { PlatformIds::PlatformId::PORT_2048,                 135},
      { PlatformIds::PlatformId::PORT_BOOM3,                135},
      { PlatformIds::PlatformId::PORT_CANNONBALL,           135},
      { PlatformIds::PlatformId::PORT_CAVE_STORY,           135},
      { PlatformIds::PlatformId::PORT_CRAFT,                135},
      { PlatformIds::PlatformId::PORT_DINOTHAWR,            135},
      { PlatformIds::PlatformId::PORT_ECWOLF,               135},
      { PlatformIds::PlatformId::PORT_MRBOOM,               135},
      { PlatformIds::PlatformId::PORT_OPENLARA,             135},
      { PlatformIds::PlatformId::PORT_PRBOOM,               135},
      { PlatformIds::PlatformId::PORT_REMINISCENCE,         135},
      { PlatformIds::PlatformId::PORT_SMW,                  135},
      { PlatformIds::PlatformId::PORT_THEPOWDERTOY,         135},
      { PlatformIds::PlatformId::PORT_TYRQUAKE,             135},
      { PlatformIds::PlatformId::PORT_VITAQUAKE2,           135},
      { PlatformIds::PlatformId::PORT_VITAQUAKE3,           135},
      { PlatformIds::PlatformId::PORT_VITAVOYAGER,          135},
      { PlatformIds::PlatformId::PORT_XRICK,                135},
      { PlatformIds::PlatformId::SAMMY_ATOMISWAVE,          53},
      { PlatformIds::PlatformId::SEGA_32X,                  19},
      { PlatformIds::PlatformId::SEGA_CD,                   20},
      { PlatformIds::PlatformId::SEGA_DREAMCAST,            23},
      { PlatformIds::PlatformId::SEGA_GAME_GEAR,            21},
      { PlatformIds::PlatformId::SEGA_GENESIS,              1},
      { PlatformIds::PlatformId::SEGA_MASTER_SYSTEM,        2},
      { PlatformIds::PlatformId::SEGA_MEGA_DRIVE,           1},
      { PlatformIds::PlatformId::SEGA_NAOMI,                56},
      { PlatformIds::PlatformId::SEGA_SATURN,               22},
      { PlatformIds::PlatformId::SEGA_SG1000,               109},
      { PlatformIds::PlatformId::SHARP_X1,                  220},
      { PlatformIds::PlatformId::SHARP_X68000,              79},
      { PlatformIds::PlatformId::SINCLAIR_ZX_SPECTRUM,      76},
      { PlatformIds::PlatformId::SINCLAIR_ZX_81,            77},
      { PlatformIds::PlatformId::SONY_PLAYSTATION,          57},
      { PlatformIds::PlatformId::SONY_PLAYSTATION_2,        58},
      { PlatformIds::PlatformId::SONY_PLAYSTATION_3,        59},
      { PlatformIds::PlatformId::SONY_PLAYSTATION_VITA,     62},
      { PlatformIds::PlatformId::SONY_PLAYSTATION_PORTABLE, 61},
      { PlatformIds::PlatformId::SPECTRAVISION_SPECTRAVIDEO,218},
      { PlatformIds::PlatformId::TANDERINE_ORICATMOS,       131},
      { PlatformIds::PlatformId::THOMSON_MOTO,              141},
    };

  return screenscraperPlatformidMap;
}

ScreenScraperEngine::ScreenScraperEngine()
  : mRunner(this, "Scraper-ssfr", false),
    mEngines
    {
      Engine(this), Engine(this), Engine(this), Engine(this), Engine(this),
      Engine(this), Engine(this), Engine(this), Engine(this), Engine(this),
      Engine(this), Engine(this), Engine(this), Engine(this), Engine(this)
    },
    mAllocatedEngines(0),
    mMethod(ScrappingMethod::All),
    mNotifier(nullptr),
    mDiskMinimumFree(0),
    mMainImage(ScreenScraperApis::IConfiguration::Image::MixV1),
    mThumbnailImage(ScreenScraperApis::IConfiguration::Image::None),
    mVideo(ScreenScraperApis::IConfiguration::Video::None),
    mWantMarquee(false),
    mWantWheel(false),
    mWantManual(false),
    mWantMaps(false),
    mWantP2K(false),
    mTotal(0),
    mCount(0),
    mStatScraped(0),
    mStatNotFound(0),
    mStatErrors(0),
    mTextInfo(0),
    mImages(0),
    mVideos(0),
    mMediaSize(0),
    mSender(this)
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
  mLanguage = RecalboxConf::Instance().GetScreenScraperLanguage();

  // Images
  mMainImage = ScreenScraperApis::IConfiguration::Image::MixV1; // Default to mix v1
  std::string value = RecalboxConf::Instance().GetScreenScraperMainMedia();
  if (value == "screenshot"  ) mMainImage = ScreenScraperApis::IConfiguration::Image::ScreenshotIngame;
  else if (value == "title"  ) mMainImage = ScreenScraperApis::IConfiguration::Image::ScreenshotTitle;
  else if (value == "logo"   ) mMainImage = ScreenScraperApis::IConfiguration::Image::Wheel;
  else if (value == "marquee") mMainImage = ScreenScraperApis::IConfiguration::Image::Marquee;
  else if (value == "box2d"  ) mMainImage = ScreenScraperApis::IConfiguration::Image::Box2d;
  else if (value == "box3d"  ) mMainImage = ScreenScraperApis::IConfiguration::Image::Box3d;
  else if (value == "mixv1"  ) mMainImage = ScreenScraperApis::IConfiguration::Image::MixV1;
  else if (value == "mixv2"  ) mMainImage = ScreenScraperApis::IConfiguration::Image::MixV2;
  mThumbnailImage = ScreenScraperApis::IConfiguration::Image::Box3d; // Default to box-3d
  value = RecalboxConf::Instance().GetScreenScraperThumbnail();
  if (value == "screenshot"  ) mThumbnailImage = ScreenScraperApis::IConfiguration::Image::ScreenshotIngame;
  else if (value == "title"  ) mThumbnailImage = ScreenScraperApis::IConfiguration::Image::ScreenshotTitle;
  else if (value == "logo"   ) mThumbnailImage = ScreenScraperApis::IConfiguration::Image::Wheel;
  else if (value == "marquee") mThumbnailImage = ScreenScraperApis::IConfiguration::Image::Marquee;
  else if (value == "box2d"  ) mThumbnailImage = ScreenScraperApis::IConfiguration::Image::Box2d;
  else if (value == "box3d"  ) mThumbnailImage = ScreenScraperApis::IConfiguration::Image::Box3d;
  else if (value == "mixv1"  ) mThumbnailImage = ScreenScraperApis::IConfiguration::Image::MixV1;
  else if (value == "mixv2"  ) mThumbnailImage = ScreenScraperApis::IConfiguration::Image::MixV2;
  mVideo = ScreenScraperApis::IConfiguration::Video::None; // Default to no video
  value = RecalboxConf::Instance().GetScreenScraperVideo();
  if (value == "normal") mVideo = ScreenScraperApis::IConfiguration::Video::Raw;
  else if (value == "optimized") mVideo = ScreenScraperApis::IConfiguration::Video::Optimized;
  mWantMarquee = RecalboxConf::Instance().GetScreenScraperWantMarquee();
  mWantWheel   = RecalboxConf::Instance().GetScreenScraperWantWheel();
  mWantManual  = RecalboxConf::Instance().GetScreenScraperWantManual();
  mWantMaps    = RecalboxConf::Instance().GetScreenScraperWantMaps();
  mWantP2K     = RecalboxConf::Instance().GetScreenScraperWantP2K();
}

bool ScreenScraperEngine::RunOn(ScrappingMethod method, FileData& singleGame, INotifyScrapeResult* notifyTarget,
                                long long diskMinimumFree)
{
  LOG(LogInfo) << "Starting new single game scrapping session...";

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
  LOG(LogInfo) << "Starting new multi-system scrapping session...";

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
    LOG(LogDebug) << "Got engine #" << engineIndex << " for " << feed->getPath().ToString();
    Engine& engine = mEngines[engineIndex];
    if (!engine.IsAborted())
    {
      engine.Initialize(true);
      switch(engine.Scrape(mMethod, *feed))
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
  else LOG(LogError) << "No more engine available!";
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
    // End of scrapping?
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
    mEngineSignal.Signal();
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

ScrapeResult ScreenScraperEngine::Engine::Scrape(ScrappingMethod method, FileData& game)
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

      LOG(LogDebug) << "Start scrapping data for " << game.getPath().ToString();
      if (mAbortRequest) break;

      // Get file size
      long long size = game.getPath().Size();
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
        switch (DownloadAndStoreMedia(method, gameResult, game))
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
  std::string md5 = (size < sMaxMd5Calculation) ? ComputeMD5(game.getPath()) : std::string();
  LOG(LogDebug) << "MD5 of " << game.getPath().ToString() << " : " << md5;

  // Get crc32
  std::string crc32;
  if (game.Metadata().RomCrc32() != 0) game.Metadata().RomCrc32AsString();

  // Lookup system
  for(int i = game.getSystem()->PlatformCount(); --i >= 0; )
  {
    if (mAbortRequest) break;

    // Convert platform ID
    PlatformIds::PlatformId systemid = game.getSystem()->PlatformIds(i);
    const HashMap<PlatformIds::PlatformId, int>& Map = GetPlatformIDs();
    int* sssysid = Map.try_get(systemid);
    if (sssysid == nullptr) return result.mResult;

    // Call!
    result = mCaller.GetGameInformation(*sssysid, game.getPath(), crc32, md5, size);
    switch(result.mResult)
    {
      case ScrapeResult::NotFound: continue;
      case ScrapeResult::QuotaReached:
      case ScrapeResult::DiskFull:
      case ScrapeResult::FatalError:
      case ScrapeResult::NotScraped:
      case ScrapeResult::Ok: return result.mResult;
    }
  }

  return result.mResult;
}

ScrapeResult ScreenScraperEngine::Engine::RequestZipGameInfo(ScreenScraperApis::Game& result, const FileData& game, long long size)
{
  if (Strings::ToLowerASCII(game.getPath().Extension()) == ".zip")
  {
    Zip zip(game.getPath());
    if (zip.Count() == 1) // Ignore multi-file archives
    {
      // Get real name
      Path filePath = zip.FileName(0);

      // Get MD5
      std::string md5 = zip.Md5(0);
      LOG(LogDebug) << "MD5 of " << filePath.ToString() << " [" << game.getPath().ToString() << "] : " << md5;

      // Get crc32
      int crc32i = zip.Crc32(0);
      std::string crc32 = Strings::ToHexa(crc32i, 8);

      // Lookup system
      for (int i = game.getSystem()->PlatformCount(); --i >= 0;)
      {
        if (mAbortRequest) break;

        // Convert platform ID
        PlatformIds::PlatformId systemid = game.getSystem()->PlatformIds(i);
        const HashMap<PlatformIds::PlatformId, int>& Map = GetPlatformIDs();
        int* sssysid = Map.try_get(systemid);
        if (sssysid == nullptr) return result.mResult;
        if (*sssysid == 75) continue; // Arcade

        // Call!
        result = mCaller.GetGameInformation(*sssysid, filePath, crc32, md5, size);
        switch (result.mResult)
        {
          case ScrapeResult::NotFound: continue;
          case ScrapeResult::DiskFull:
          case ScrapeResult::QuotaReached:
          case ScrapeResult::FatalError:
          case ScrapeResult::NotScraped:
          case ScrapeResult::Ok: return result.mResult;
        }
      }
    }
  }

  return result.mResult;
}

bool ScreenScraperEngine::Engine::NeedScrapping(ScrappingMethod method, FileData& game)
{
  const Path rootMediaPath = game.getTopAncestor().getPath() / "media";
  const std::string gameFile = game.getPath().FilenameWithoutExtension();
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
      if (mConfiguration.GetImageType() != ScreenScraperApis::IConfiguration::Image::None)
        if (game.Metadata().Image().IsEmpty()) return true;
      if (mConfiguration.GetThumbnailType() != ScreenScraperApis::IConfiguration::Image::None)
        if (game.Metadata().Thumbnail().IsEmpty()) return true;
      if (mConfiguration.GetVideo() != ScreenScraperApis::IConfiguration::Video::None)
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
  LOG(LogError) << "Unknown scrapping method";
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
  // Store data only if they are not empty and not scrapped if method is IncompleteKeep
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

ScrapeResult ScreenScraperEngine::Engine::DownloadMedia(const std::string& gameName, FileData& game,
  const Path& mediaFolder, const std::string& media, const std::string& format, SetPathMethodType pathSetter)
{
  if (!media.empty())
  {
    Path AbsoluteImagePath = mediaFolder / (gameName + '.' + format);
    long long size = 0;

    switch(mCaller.GetMedia(media, AbsoluteImagePath, size))
    {
      case ScrapeResult::Ok:
      {
        if (pathSetter != nullptr)
          (game.Metadata().*pathSetter)(AbsoluteImagePath);
        mImages++;
        mMediaSize += size;
        break;
      }
      case ScrapeResult::NotScraped: break;
      case ScrapeResult::NotFound: LOG(LogError) << "Missing media!"; break;
      case ScrapeResult::DiskFull: return ScrapeResult::DiskFull;
      case ScrapeResult::QuotaReached: return ScrapeResult::QuotaReached;
      case ScrapeResult::FatalError: break;
    }
  }
  return ScrapeResult::Ok;
}

ScrapeResult
ScreenScraperEngine::Engine::DownloadAndStoreMedia(ScrappingMethod method, const ScreenScraperApis::Game& sourceData,
                                                   FileData& game)
{
  bool ok = false;
  const Path rootFolder(game.getTopAncestor().getPath());
  const Path relativePath = game.getPath().MakeRelative(rootFolder, ok);
  const std::string gameName = ok ? (relativePath.Directory() / game.getPath().FilenameWithoutExtension()).ToString()
                                  : game.getPath().FilenameWithoutExtension();
  const Path mediaFolder = rootFolder / "media";

  // Main image
  if (!game.Metadata().Image().Exists() || method != ScrappingMethod::IncompleteKeep)
  {
    ScrapeResult result = DownloadMedia(gameName, game, mediaFolder / "images", sourceData.MediaSources.mImage,
                                        sourceData.MediaSources.mImageFormat, &MetadataDescriptor::SetImagePath);
    if (result != ScrapeResult::Ok || mAbortRequest) return result;
  }

  // Thumbnail image
  if (!game.Metadata().Thumbnail().Exists() || method != ScrappingMethod::IncompleteKeep)
  {
    ScrapeResult result = DownloadMedia(gameName, game, mediaFolder / "thumbnails",
                                        sourceData.MediaSources.mThumbnail, sourceData.MediaSources.mThumbnailFormat,
                                        &MetadataDescriptor::SetThumbnailPath);
    if (result != ScrapeResult::Ok || mAbortRequest) return result;
  }

  // Video
  if (!game.Metadata().Video().Exists() || method != ScrappingMethod::IncompleteKeep)
  {
    ScrapeResult result = DownloadMedia(gameName, game, mediaFolder / "videos", sourceData.MediaSources.mVideo,
                                        sourceData.MediaSources.mVideoFormat, &MetadataDescriptor::SetVideoPath);
    if (result != ScrapeResult::Ok || mAbortRequest) return result;
  }

  // Marquee - TODO: Absolutepath is built twice
  Path MarqueeAbsoluteImagePath = mediaFolder / "marquees" / (gameName + '.' + sourceData.MediaSources.mMarqueeFormat);
  if (!MarqueeAbsoluteImagePath.Exists() || method != ScrappingMethod::IncompleteKeep)
  {
    ScrapeResult result = DownloadMedia(gameName, game, mediaFolder / "marquees",
                                        sourceData.MediaSources.mMarquee, sourceData.MediaSources.mMarqueeFormat,
                                        nullptr);
    if (result != ScrapeResult::Ok || mAbortRequest) return result;
  }

  // Wheel - TODO: Absolutepath is built twice
  Path WheelAbsoluteImagePath = mediaFolder / "wheels" / (gameName + '.' + sourceData.MediaSources.mWheelFormat);
  if (!WheelAbsoluteImagePath.Exists() || method != ScrappingMethod::IncompleteKeep)
  {
    ScrapeResult result = DownloadMedia(gameName, game, mediaFolder / "wheels", sourceData.MediaSources.mWheel,
                                        sourceData.MediaSources.mWheelFormat, nullptr);
    if (result != ScrapeResult::Ok || mAbortRequest) return result;
  }

  // Manual - TODO: Absolutepath is built twice
  Path ManualAbsoluteImagePath = mediaFolder / "manuals" / (gameName + '.' + sourceData.MediaSources.mManualFormat);
  if (!ManualAbsoluteImagePath.Exists() || method != ScrappingMethod::IncompleteKeep)
  {
    ScrapeResult result = DownloadMedia(gameName, game, mediaFolder / "manuals",
                                        sourceData.MediaSources.mManual, sourceData.MediaSources.mManualFormat,
                                        nullptr);
    if (result != ScrapeResult::Ok || mAbortRequest) return result;
  }

  // Maps - TODO: Absolutepath is built twice
  Path MapAbsoluteImagePath = mediaFolder / "maps" / (gameName + '.' + sourceData.MediaSources.mMapsFormat);
  if (!MapAbsoluteImagePath.Exists() || method != ScrappingMethod::IncompleteKeep)
  {
    ScrapeResult result = DownloadMedia(gameName, game, mediaFolder / "maps", sourceData.MediaSources.mMaps,
                                        sourceData.MediaSources.mMapsFormat, nullptr);
    if (result != ScrapeResult::Ok || mAbortRequest) return result;
  }

  // TODO: Add more image download & save here

  return ScrapeResult::Ok;
}
