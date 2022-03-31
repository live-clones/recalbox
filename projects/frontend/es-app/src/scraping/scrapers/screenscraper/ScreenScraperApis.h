//
// Created by bkg2k on 07/12/2019.
//
#pragma once

#include <string>
#include <utils/Http.h>
#include <utils/datetime/DateTime.h>
#include <rapidjson/document.h>
#include <scraping/ScrapeResult.h>
#include <games/classifications/Genres.h>
#include <games/classifications/Regions.h>
#include <scraping/scrapers/screenscraper/IConfiguration.h>
#include <scraping/scrapers/screenscraper/IEndPointProvider.h>

class ScreenScraperApis
{
  public:
    //! Game object
    struct Game
    {
      //! Result
      ScrapeResult mResult = ScrapeResult::NotFound;
      //! ScreenScraper Game Name
      std::string mScreenScraperName;
      //! Game Name
      std::string mName;
      //! Developer
      std::string mDeveloper;
      //! Publisher
      std::string mPublisher;
      //! Synopsis
      std::string mSynopsis;
      //! Players
      std::string mPlayers;
      //! Genre
      std::string mGenre;
      //! Rom region
      std::string mRegion;
      //! Rom region
      std::string mCrc;
      //! Pad2Keyboard
      std::string mP2k;
      //! Editor
      DateTime mReleaseDate;
      //! Rating
      float mRating;
      //! Normalized genre
      GameGenres mGenreId;
      //! Adult state
      bool mAdult;

      //! Media Urls
      struct MediaUrl
      {
        struct Media
        {
          //! source Url
          std::string mUrl;
          //! file format
          std::string mFormat;
          //! md5
          std::string mMd5;
          //! size
          long long mSize;
        };

        //! Main image
        Media mImage;
        //! Thumbnail image
        Media mThumbnail;
        //! Video
        Media mVideo;
        //! Marquee
        Media mMarquee;
        //! Wheel (clear logo)
        Media mWheel;
        //! Game Manual
        Media mManual;
        //! Game Maps
        Media mMaps;
      }
      MediaSources;
    };

  private:
    //! Credential interface
    IConfiguration& mConfiguration;
    //! Endpoint provider
    IEndPointProvider& mEndPointProvider;

    // Client initialized status
    bool mClientInitialized;

    //! Htpp client
    Http mClient;

    /*!
     * @brief Deserialize a gameinfo request JSON result into a Game object
     * @param json Input json
     * @param game Output Game object
     */
    void DeserializeGameInformationInner(const rapidjson::Value& json, Game& game, const Path& path);

    /*!
     * @brief Deserialize a gameinfo request JSON result into a Game object
     * @param json Input json
     * @param game Output Game object
     */
    void DeserializeGameInformationOuter(const std::string& json, Game& game, const Path& path);

    static std::string GetRequiredRegion(Regions::GameRegions romRegion, const Path& path, Regions::GameRegions favoriteRegion);

    /*!
     * @brief Extract the best-matching text from an array of regionalized texts, regarding the given preferred region
     * @param array Json array of regionalized texts
     * @param requiredRegion Required region
     * @return Best-matching text
     */
    static std::string ExtractRegionalizedText(const rapidjson::Value& array, const std::string& requiredRegion);

    /*!
     * @brief Extract the best-matching text from an array of localized texts, regarding the given preferred language
     * @param array Json array of localized texts
     * @param preferedlanguage Preferred language
     * @return Best-matching text
     */
    static std::string ExtractLocalizedText(const rapidjson::Value& array, const std::string& preferedlanguage);

    /*!
     * @brief Extract the best-matching media URL, given the type, parent type and prefered region.
     * @param medias Media name
     * @param type Type
     * @param region Preferred region
     * @param format If an url is found, this string contains the media format (jpg, png, mp4, ...)
     * @return Best-matching url
     */
    static std::string ExtractMedia(const rapidjson::Value& medias, const char* type, const std::string& region, std::string& format, long long& size, std::string&  md5);

    /*!
     * @brief Extract best matching genre texts, comma-separated.
     * @param array Json array of genres
     * @param preferedlanguage Prefered language
     * @return Genre strings, comma-separated
     */
    static std::string ExtractLocalizedGenre(const rapidjson::Value& array, const std::string& preferedlanguage);

    /*!
     * @brief Extract a normalized game genre
     * @param array Json array of genres
     * @return GameGenres value
     */
    static GameGenres ExtractNormalizedGenre(const rapidjson::Value& array);

    /*!
     * @brief Extract the adult state
     * @param array Json array of genres
     * @return True if the game is flagged Adult, false otherwise
     */
    static bool ExtractAdultState(const rapidjson::Value& array);

    /*!
     * @brief Decode inner string encoding, including XML and X encoding like
     * &quot; \u0260 and \r\n
     * @param raw Raw string, decoded in place
     */
    static void DecodeString(std::string& raw);

    /*!
     * @brief Return a clean game name, removing all non file authorized chars
     * @param source Original game
     * @return Clean name
     */
    static std::string CleanGameName(const std::string& source);

    /*!
     * @brief Check if the http client is initialized & initialize if required
     */
    void InitializeClient()
    {
      if (!mClientInitialized)
      {
        if (mEndPointProvider.RequireBasicAuth())
          mClient.SetBasicAuth(mConfiguration.GetLogin(), mConfiguration.GetPassword());
        mClientInitialized = true;
      }
    }

  public:
    /*!
     * @brief Constructor
     * @param credentials
     */
    explicit ScreenScraperApis(IConfiguration* credentials, IEndPointProvider* endPointProvider)
      : mConfiguration(*credentials)
      , mEndPointProvider(*endPointProvider)
      , mClientInitialized(false)
    {
    }

    //! Get user
    ScreenScraperUser GetUserInformation();

    /*!
     * @brief Get Game informations
     * @param file Game to work on
     * @param crc32 optionnal CRC32
     * @param md5 optionnal MD5
     * @return Game structure with the result code in the mResult field
     */
    Game GetGameInformation(const FileData& file, const std::string& crc32, const std::string& md5, long long size);

    /*!
     * @brief Download a media into a target file
     * @param mediaurl Media url
     * @param to Filepath
     * @return ScrapeResult Request status
     */
    ScrapeResult GetMedia(const FileData& game, const std::string& mediaurl, const Path& to, long long& size);
};

