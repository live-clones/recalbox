//
// Created by bkg2k on 17/03/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <scraping/scrapers/screenscraper/ScreenScraperApis.h>
#include <scraping/scrapers/screenscraper/ProtectedSet.h>
#include <scraping/scrapers/ScrapingMethod.h>
#include <games/FileData.h>
#include <games/MetadataFieldDescriptor.h>
#include <scraping/scrapers/IScraperEngineStage.h>
#include <scraping/scrapers/IScraperEngineFreezer.h>

//! Persistant engine class accross requests
class ScreenScraperSingleEngine
{
  private:
    enum class MediaType
    {
      Image, //!< Images or pdf
      Video, //!< Video
    };

    //! Maximum file size for md5 calculation
    static constexpr int sMaxMd5Calculation = (20 << 20); // 20 Mb

    //! Sub folder image
    static std::string sImageSubFolder;
    //! Sub folder thumbnail
    static std::string sThumbnailSubFolder;
    //! Sub folder video
    static std::string sVideoSubFolder;
    //! Sub folder wheel
    static std::string sWheelSubFolder;
    //! Sub folder marquee
    static std::string sMarqueeSubFolder;
    //! Sub folder manual
    static std::string sManualSubFolder;
    //! Sub folder map
    static std::string sMapSubFolder;

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
    IConfiguration& mConfiguration;
    //! Stage interface
    IScraperEngineStage* mStageInterface;

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
    bool NeedScraping(ScrapingMethod method, FileData& game);

    /*!
     * @brief Store scraped data into destination game's metadata, regarding the scraping method
     * @param method Scraping method
     * @param sourceData Source data
     * @param game DFestination game
     */
    void StoreTextData(ScrapingMethod method, const ScreenScraperApis::Game& sourceData, FileData& game);

    /*!
     * @brief Download an store media one after one
     * @param method Scraping method
     * @param sourceData Source data
     * @param game DFestination game
     * @return True if the quota is reached and the scraping must stop ASAP. False in any other case
     */
    ScrapeResult DownloadAndStoreMedia(ScrapingMethod method, const ScreenScraperApis::Game& sourceData, FileData& game, ProtectedSet& md5Set);

    /*!
     * @brief Download a single media
     * @param game Game to download med
     * @param noKeep Don't keep the file even if it exists
     * @param target Target Path
     * @param subPath Mdia sub folder
     * @param name Game name
     * @param mediaType Media type
     * @param pathSetter Method to set media path in game object
     * @param mediaSource Media source
     * @param md5Set MD5 protected set
     * @return ScrapeResult
     */
    ScrapeResult DownloadAndStoreMedia(FileData& game, bool noKeep, const Path& target, const std::string& subPath, const std::string& name, MediaType mediaType, SetPathMethodType pathSetter, const ScreenScraperApis::Game::MediaUrl::Media& mediaSource, ProtectedSet& md5Set);

    /*!
     * @brief Check if the result code is fatal or not
     * @param result Result code
     * @return True if the code is fatal
     */
    bool IsFatal(ScrapeResult result) const;

    /*!
     * @brief Download and store one media
     * @param gameName Relatiove game path to the system rom folder
     * @param method Scraping method
     * @param game Game being scraped
     * @param mediaFolder Base media folder (roms/<system>/media/<mediatype>)
     * @param media Media being downloaded
     * @param format Media format (file extension)
     * @return Scrape result
     */
    ScrapeResult DownloadMedia(const Path& AbsoluteImagePath, FileData& game, const std::string& media, SetPathMethodType pathSetter, ProtectedSet& md5Set, MediaType mediaType);

  public:
    explicit ScreenScraperSingleEngine(IConfiguration* configuration, IEndPointProvider* endPointProvider, IScraperEngineStage* stageInterface)
      : mRunning(false)
      , mAbortRequest(false)
      , mQuotaReached(false)
      , mTextInfo(0)
      , mImages(0)
      , mVideos(0)
      , mMediaSize(0)
      , mCaller(configuration, endPointProvider)
      , mConfiguration(*configuration)
      , mStageInterface(stageInterface)
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
    ScrapeResult Scrape(ScrapingMethod method, FileData& game, ProtectedSet& md5Set);

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



