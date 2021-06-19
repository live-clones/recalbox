//
// Created by bkg2k on 04/12/2019.
//
#pragma once

#include <scraping/scrapers/IScraperEngine.h>
#include <utils/os/system/Mutex.h>
#include <utils/os/system/Signal.h>
#include <utils/sdl2/SyncronousEvent.h>
#include <utils/os/system/ThreadPool.h>
#include <games/MetadataFieldDescriptor.h>
#include "ScreenScraperApis.h"
#include <utils/storage/Set.h>

class ScreenScraperEngine
  : public IScraperEngine,
    public IThreadPoolWorkerInterface<FileData*, bool>,
    public ScreenScraperApis::IConfiguration,
    public ISynchronousEvent
{
  private:

    class MutexSet
    {
      public:
        HashSet<std::string> mSet{};
        Mutex mMutex;
    };

    //! Persistant engine class accross requests
    class Engine
    {
      private:
        //! Tell if the engine is running
        volatile bool mRunning;
        //! True if the current scrape is requested to abort
        volatile bool mAbortRequest;
        //! Quota reached
        volatile bool mQuotaReached;
        //! Statistics: Text infos
        int mTextInfo;
        //! Statistics: Images
        int mImages;
        //! Statistics: Video
        int mVideos;
        //! Statistics: Media size
        long long mMediaSize;
        //! ScreenScraper WebApi
        ScreenScraperApis mCaller;
        //! Configuration
        ScreenScraperApis::IConfiguration& mConfiguration;

        /*!
         * @brief Compute MD5 hash from a file
         * @param path Filepath
         * @return Hash hexa string
         */
        static std::string ComputeMD5(const Path& path);

        /*!
         * @brief Send a game info request
         * @param game FileData game object
         * @param md5 Optionnal md5
         * @param size Rom size
         * @return Result
         */
        ScrapeResult RequestGameInfo(ScreenScraperApis::Game& result, const FileData& game, long long size);

        /*!
         * @brief Send a game info request
         * @param game FileData game object
         * @param md5 Optionnal md5
         * @param size Rom size
         * @return Result
         */
        ScrapeResult RequestZipGameInfo(ScreenScraperApis::Game& result, const FileData& game, long long size);

        /*!
         * @brief Check if the current game needs to be scraped regarding the given method
         * @param method Scraping method
         * @param game Game to scrape
         * @return True of the game need to be scraped
         */
        bool NeedScrapping(ScrappingMethod method, FileData& game);

        /*!
         * @brief Store scraped data into destination game's metadata, regarding the scraping method
         * @param method Scraping method
         * @param sourceData Source data
         * @param game DFestination game
         */
        void StoreTextData(ScrappingMethod method, const ScreenScraperApis::Game& sourceData, FileData& game);

        /*!
         * @brief Download an store media one after once
         * @param method Scraping method
         * @param sourceData Source data
         * @param game DFestination game
         * @return True if the quota is reached and the scraping must stop ASAP. False in any other case
         */
        ScrapeResult DownloadAndStoreMedia(ScrappingMethod method, const ScreenScraperApis::Game& sourceData, FileData& game, MutexSet& md5Set);

        /*!
         * @brief Download and store one media
         * @param gameName Relatiove game path to the system rom folder
         * @param method Scraping method
         * @param game Game being scraped
         * @param mediaFolder Base media folder (roms/<system>/media/<mediatype>)
         * @param media Media being downloaded
         * @param format MEdia format (file extension)
         * @return Scrape result
         */
        ScrapeResult DownloadMedia(const Path& AbsoluteImagePath, FileData& game,
                                   const std::string& media, SetPathMethodType pathSetter, MutexSet& md5Set, std::string mediaType);

      public:
        explicit Engine(ScreenScraperApis::IConfiguration* configuration)
          : mRunning(false),
            mAbortRequest(false),
            mQuotaReached(false),
            mTextInfo(0),
            mImages(0),
            mVideos(0),
            mMediaSize(0),
            mCaller(configuration),
            mConfiguration(*configuration)
        {
          Initialize(false);
        }

        /*!
         * @brief Reset the engine
         * @param noabort True to not reinitialize the abort flag
         */
        void Initialize(bool noabort);

        /*!
         * @brief Scrape a single game
         * @param game game to scrape
         * @return True if the whole process must stop for whatever reason
         */
        ScrapeResult Scrape(ScrappingMethod method, FileData& game, MutexSet& md5Set);

        /*!
         * @brief Abort the current engine. The engine is required to quit its current scraping ASAP
         */
        void Abort() { mAbortRequest = true; }

        //! Check if the engine has been aborted
        bool IsAborted() const { return mAbortRequest; }

        //! Check if the engine is running
        bool IsRunning() const { return mRunning; }

        //! Stats Text infos
        int StatsTextInfo() const { return mTextInfo; };

        //! Stats images downloaded
        int StatsImages() const { return mImages; };

        //! Stats videos downloaded
        int StatsVideos() const { return mVideos; };

        //! Stats videos downloaded
        long long StatsMediaSize() const { return mMediaSize; };
    };

    //! Maximum simultaneous engines
    static constexpr int sMaxEngines = 15;
    //! Maximum file size for md5 calculation
    static constexpr int sMaxMd5Calculation = (20 << 20); // 20 Mb

    //! Thread pool
    ThreadPool<FileData*, bool> mRunner;

    //! Engines
    Engine mEngines[sMaxEngines];
    //! Bitflag of allocated engine. If the bit at index X is set, the engine is allocated to a thread
    int mAllocatedEngines;

    //! Scraping method
    ScrappingMethod mMethod;

    //! Notification interface
    INotifyScrapeResult* mNotifier;

    //! Minimum free disk
    long long mDiskMinimumFree;

    //! Screenscraper credentials: Login
    std::string mLogin;
    //! Screenscraper credentials: Password
    std::string mPassword;
    //! Favorite language
    Languages mLanguage;
    //! Favorite region
    Regions::GameRegions mRegion;
    //! Main image
    ScreenScraperEnums::ScreenScraperImageType mMainImage;
    //! Thumbnail image
    ScreenScraperEnums::ScreenScraperImageType mThumbnailImage;
    //! Video
    ScreenScraperEnums::ScreenScraperVideoType mVideo;
    //! Marquee
    bool mWantMarquee;
    //! Wheel
    bool mWantWheel;
    //! Marquee
    bool mWantManual;
    //! Wheel
    bool mWantMaps;
    //! Pad 2 keyboard
    bool mWantP2K;

    //! Live stats: Total
    int mTotal;
    //! Live stats: Processed
    int mCount;

    //! Statistics: Scraped games
    int mStatScraped;
    //! Statistics: Gamesnot found
    int mStatNotFound;
    //! Statistics: Scrape errors
    int mStatErrors;
    //! Statistics: Text infos
    int mTextInfo;
    //! Statistics: Images
    int mImages;
    //! Statistics: Video
    int mVideos;
    //! Statistics: Media size
    long long mMediaSize;

    //! Engine allocator protection
    Mutex mEngineMutex;
    //! Free engine signal
    Signal mEngineSignal;

    MutexSet mMd5Set;

    //! Main thread synchronizer
    SyncronousEvent mSender;

    //! Database message
    std::string mDatabaseMessage;

    /*
     * Getters
     */

    //! Get current scraping session completion
    int Completed() const { return mCount; }

    //! Get current scraping session's total item to process
    int Total() const { return mTotal; }

    /*
     * Engine providers/recyclers
     */

    /*!
     * @brief Obtain a free engine index or -1 if no engine is available
     * @return Engine index from 0 to sMaxEngines-1, or -1
     */
    int ObtainEngine();

    /*!
     * @brief Recycle and free the engine at the given index
     * @param index Engine index to free and recycle
     */
    void RecycleEngine(int index);

    /*
     * IScraperEngine implementation
     */

    /*!
     * @brief Called from the factory to reset the current instance to its initial state
     * Implementation is responsible for reseting all its internal structures as if it were just created.
     */
    void Initialize() override;

    /*!
     * @brief Run the scraper using the given methods, on the given system list and report progress using notifyTarget
     * @param method Scraping method
     * @param systemList System list
     * @param notifyTarget Interface for reporting scraping progression
     * @return True if everything has been successful. False if cancelled, quota reached or fatal error occurred
     */
    bool RunOn(ScrappingMethod method, const SystemManager::SystemList& systemList, INotifyScrapeResult* notifyTarget,
               long long diskMinimumFree) override;

    /*!
     * @brief Run the scraper using the given methods, on the given single game and report progress using notifyTarget
     * @param method Scraping method
     * @param singleGame Single game to scrape
     * @param notifyTarget Interface for reporting scraping progression
     * @return True if everything has been successful. False if cancelled, quota reached or fatal error occurred
     */
    bool RunOn(ScrappingMethod method, FileData& singleGame,
               INotifyScrapeResult* notifyTarget, long long diskMinimumFree) override;

    //! Get total to scrape
    int ScrapesTotal() const override { return mTotal; }

    //! Get processed items
    int ScrapesProcessed() const override { return mCount; }

    //! Get pending items (still not scraped)
    int ScrapesStillPending() const override { return mTotal - mCount; }

    //! Get successfully scraped games
    int ScrapesSuccessful() const override { return mStatScraped; }

    //! Get unsuccessfully scraped games
    int ScrapesNotFound() const override { return mStatNotFound; }

    //! Get failes scrapes
    int ScrapesErrors() const override { return mStatErrors; }

    //! Stats Text infos
    int StatsTextInfo() const override { return mTextInfo; };

    //! Stats images downloaded
    int StatsImages() const override { return mImages; };

    //! Stats videos downloaded
    int StatsVideos() const override { return mVideos; };

    //! Stats videos downloaded
    long long StatsMediaSize() const override { return mMediaSize; };


    //! Get Scraper message
    std::string ScraperDatabaseMessage() override { return mDatabaseMessage; };

    /*!
     * @brief Abort the current engine
     * @param waitforcompletion If true, wait for completion before exit
     * @return True
     */
    bool Abort(bool waitforcompletion) override
    {
      for(int i = sMaxEngines; --i >= 0; )
      {
        mEngines[i].Abort();
        mEngineSignal.Fire();
      }
      if (waitforcompletion)
        mRunner.WaitForCompletion();
      return true;
    }

    /*!
     * @brief Check if the engine is running, allowing UI to know when the engine actually stops after an abort request
     * @return True if the engine is running
     */
    bool IsRunning() const override
    {
      for(int i = sMaxEngines; --i >= 0; )
        if (mEngines[i].IsRunning())
          return true;
      return false;
    }

    /*
     * IThreadPoolWorkerInterface
     */

    /*!
     * @brief The main runner. Implement here the task to process a feed object
     * @param feed Feed object to process
     * @return Result Object
     */
    bool ThreadPoolRunJob(FileData*& feed) override;

    /*
     * ScreenScraperApis::IConfiguration implementation
     */

    //! Get screenscraper login
    std::string GetLogin() const override { return mLogin; }

    //! Get screenscraper password
    std::string GetPassword() const override { return mPassword; }

    //! Get favorite language
    Languages GetFavoriteLanguage() const override { return mLanguage; };

    //! Get favorite region
    Regions::GameRegions GetFavoriteRegion() const override { return mRegion; }

    //! Get main image type
    ScreenScraperEnums::ScreenScraperImageType GetImageType() const override { return mMainImage; }

    //! Get thumbnail image typ
    ScreenScraperEnums::ScreenScraperImageType GetThumbnailType() const override { return mThumbnailImage; }

    //! Check if video are required
    ScreenScraperEnums::ScreenScraperVideoType GetVideo() const override { return mVideo; }

    //! Check if marquee are required
    bool GetWantMarquee() const override { return mWantMarquee; }

    //! Check if wheel are required
    bool GetWantWheel() const override { return mWantWheel; }

    //! Check if manual are required
    bool GetWantManual() const override { return mWantManual; }

    //! Check if maps are required
    bool GetWantMaps() const override { return mWantMaps; }

    //! Check if p2k are required
    bool GetWantP2K() const override { return mWantP2K; }

    /*
     * ISynchronousEvent implementation
     */

    /*!
     * @brief Receive synchronous SDL2 event
     * @param event SDL event with .user populated by the sender
     */
    void ReceiveSyncCallback(const SDL_Event& event) override ;

  public:
    //! Constructor
    ScreenScraperEngine();
};
