//
// Created by bkg2k on 17/03/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <games/FileData.h>
#include <utils/os/system/IThreadPoolWorkerInterface.h>
#include <utils/os/system/ThreadPool.h>
#include <utils/cplusplus/StaticLifeCycleControler.h>
#include <scraping/scrapers/screenscraper/ScreenScraperSingleEngine.h>
#include "scraping/scrapers/recalbox/RecalboxEndPoints.h"
#include <utils/sdl2/SyncronousEvent.h>
#include <RecalboxConf.h>
#include <SDL_timer.h>

// Forward declaration
class SystemManager;

class ScraperSeamless : public IThreadPoolWorkerInterface<FileData*, FileData*>
                      , public StaticLifeCycleControler<ScraperSeamless>
                      , public IConfiguration
                      , public ISynchronousEvent
                      , public IScraperEngineStage
{
  public:
    //! Scraping "engines" count
    static constexpr int sScrapingEngineCount = 8;

    /*!
     * @brief Constructor
     */
    ScraperSeamless();

    //! Destructor
    virtual ~ScraperSeamless() = default;

    /*!
     * @brief Push a new candidate to scraping
     * @param game Game to check/scrape
     */
    void Push(FileData* game, IScraperEngineStage* interface);

    //! Check if the given game is currently scraping
    bool IsScraping(FileData* game) const { return mRunningScrapes.contains(game); }

    /*!
     * @brief Get how long the given game is scraping
     * @param game Game to check
     * @return Elapsed time in ms, or -1 if the given game is not currently scraping
     */
    int HowLong(FileData* game) const
    {
      Information* result = mRunningScrapes.try_get(game);
      return result != nullptr ? (int)(SDL_GetTicks() - (*result).StartTick) : -1;
    }

    //! Get authentication status
    bool IsAuthenticated() const { return mAuthenticated; }

  private:
    //! Storage class
    struct Information
    {
      unsigned long long StartTick;  //!< Timestamp of scrape start
      IScraperEngineStage* Interface; //!< Callback interface
    };

    //! Recalbox configuration
    RecalboxConf& mConfiguration;

    //! Endpoint provider
    RecalboxEndPoints mEndPoints;
    //! Protected media set
    ProtectedSet mProtectedSet;

    //! Scraping "engines"
    ScreenScraperSingleEngine mEngines[sScrapingEngineCount];
    //! Busy Engines bitflag
    int mBusyEngines;
    //! Busy engines protection
    Mutex mBusyEngineLocker;

    //! Main thread synchronizer
    SyncronousEvent mSender;

    //! Current scraping set
    HashMap<FileData*, Information> mRunningScrapes;
    //! Set protector
    Mutex mRunningScrapesLoker;
    //! Thread pool
    ThreadPool<FileData*, FileData*> mPool;
    //! Authenticated?
    bool mAuthenticated;

    //! Basic authentication
    bool Authenticate();

    /*!
     * @brief Pop game from the scrape set
     * @param game Game to pop
     * @result Callback interface for the given game
     */
    IScraperEngineStage* Pop(FileData* game, bool remove);

    /*!
     * @brief Get a free engine index
     * @return Engine index
     */
    int AllocateEngine()
    {
      Mutex::AutoLock locker(mBusyEngineLocker);
      for(int i = sScrapingEngineCount; --i >= 0;)
        if ((mBusyEngines & (1 << i)) == 0)
        {
          mBusyEngines |= 1 << i;
          return i;
        }
      return -1;
    }

    /*!
     * @brief Free engine by index
     * @param index Indix of engine to free
     */
    void FreeEngine(int index)
    {
      Mutex::AutoLock locker(mBusyEngineLocker);
      mBusyEngines &= ~(1 << index);
    }

    /*
     * IThreadPoolWorkerInterface implementation
     */

    /*!
     * @brief The main runner. Implement here the task to process a feed object
     * @param feed Feed object to process
     * @return Result Object
     */
    FileData* ThreadPoolRunJob(FileData*& feed) override;

    /*
     * ScreenScraperApis::IConfiguration implementation
     */

    //! Empty reset
    void ResetConfiguration() override {}

    //! Get screenscraper login
    std::string GetLogin() const override { return Strings::Empty; }

    //! Get screenscraper password
    std::string GetPassword() const override { return Strings::Empty; }

    //! Get recalbox token
    std::string GetBearer() const override { return Strings::Trim(mConfiguration.GetRecalboxPrivateKey()); }

    //! Get favorite language
    Languages GetFavoriteLanguage() const override { return LanguagesTools::GetScrapingLanguage(); };

    //! Get favorite region
    Regions::GameRegions GetFavoriteRegion() const override { return mConfiguration.GetScreenScraperRegion(); }

    //! Get main image type
    ScreenScraperEnums::ScreenScraperImageType GetImageType() const override { return mConfiguration.GetScreenScraperMainMedia(); }

    //! Get thumbnail image typ
    ScreenScraperEnums::ScreenScraperImageType GetThumbnailType() const override { return mConfiguration.GetScreenScraperThumbnail(); }

    //! Check if video are required
    ScreenScraperEnums::ScreenScraperVideoType GetVideo() const override
    {
      switch(mConfiguration.GetScreenScraperVideo())
      {
        case ScreenScraperEnums::ScreenScraperVideoType::Raw:
        case ScreenScraperEnums::ScreenScraperVideoType::Optimized: return ScreenScraperEnums::ScreenScraperVideoType::Optimized;
        case ScreenScraperEnums::ScreenScraperVideoType::None:
        default: break;
      }
      return ScreenScraperEnums::ScreenScraperVideoType::None;
    }

    //! Check if marquee are required
    bool GetWantMarquee() const override { return mConfiguration.GetScreenScraperWantMarquee(); }

    //! Check if wheel are required
    bool GetWantWheel() const override { return mConfiguration.GetScreenScraperWantWheel(); }

    //! Check if manual are required
    bool GetWantManual() const override { return mConfiguration.GetScreenScraperWantManual(); }

    //! Check if maps are required
    bool GetWantMaps() const override { return mConfiguration.GetScreenScraperWantMaps(); }

    //! Check if p2k are required
    bool GetWantP2K() const override { return mConfiguration.GetScreenScraperWantP2K(); }

    /*
     * ISynchronousEvent implementation
     */

    /*!
     * @brief Receive synchronous SDL2 event
     * @param event SDL event with .user populated by the sender
     */
    void ReceiveSyncCallback(const SDL_Event& event) override;

    /*
     * IScrapeEngineStage implementation
     */

    /*!
     * @brief Rezport scraping stage completion
     * @param game Target game
     * @param stage Last stage completed
     */
    void StageCompleted(FileData* game, Stage stage) override;
};



