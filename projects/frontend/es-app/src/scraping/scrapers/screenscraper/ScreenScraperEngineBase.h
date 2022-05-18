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
#include <scraping/scrapers/screenscraper/ScreenScraperApis.h>
#include <scraping/scrapers/screenscraper/ScreenScraperSingleEngine.h>
#include <scraping/scrapers/screenscraper/ProtectedSet.h>

class ScreenScraperEngineBase
  : public IScraperEngine,
    public IThreadPoolWorkerInterface<FileData*, bool>,
    public IConfiguration,
    public ISynchronousEvent
{
  private:

    //! Maximum simultaneous engines
    static constexpr int sMaxEngines = 15;

    //! Thread pool
    ThreadPool<FileData*, bool> mRunner;
    //! Engines
    ScreenScraperSingleEngine mEngines[sMaxEngines];
    //! Bitflag of allocated engine. If the bit at index X is set, the engine is allocated to a thread
    int mAllocatedEngines;

    //! Scraping method
    ScrapingMethod mMethod;
    //! Scraping endpoint reference
    IEndPointProvider& mEndPoint;

    //! Notification interface
    INotifyScrapeResult* mNotifier;

    //! Minimum free disk
    long long mDiskMinimumFree;

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

    ProtectedSet mMd5Set;

    //! Main thread synchronizer
    SyncronousEvent mSender;

    //! Database message
    std::string mDatabaseMessage;

    //! Freeze interface
    IScraperEngineFreezer* mFreezeInterface;

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
    bool RunOn(ScrapingMethod method, const SystemManager::SystemList& systemList, INotifyScrapeResult* notifyTarget,
               long long diskMinimumFree) override;

    /*!
     * @brief Run the scraper using the given methods, on the given single game and report progress using notifyTarget
     * @param method Scraping method
     * @param singleGame Single game to scrape
     * @param notifyTarget Interface for reporting scraping progression
     * @return True if everything has been successful. False if cancelled, quota reached or fatal error occurred
     */
    bool RunOn(ScrapingMethod method, FileData& singleGame,
               INotifyScrapeResult* notifyTarget, long long diskMinimumFree) override;

    /*!
     * @brief Clear result interface
     */
    void ClearResultInterface() { mNotifier = nullptr; }

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
      // Clear callback interface
      ClearResultInterface();

      // Cancel pending jobs
      mRunner.CancelPendingJobs();
      // Send abort signal to all jobs
      for(int i = sMaxEngines; --i >= 0; )
      {
        mEngines[i].Abort();
        mEngineSignal.Fire();
      }
      // Wait for running jobs to complete
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
     * ISynchronousEvent implementation
     */

    /*!
     * @brief Receive synchronous SDL2 event
     * @param event SDL event with .user populated by the sender
     */
    void ReceiveSyncCallback(const SDL_Event& event) override ;

  public:
    /*!
     * @brief Constructor
     * @param endpoint End point provider
     */
    ScreenScraperEngineBase(IEndPointProvider& endpoint, IScraperEngineFreezer* freezer);
};
