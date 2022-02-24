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
#include "ScreenScraperEnums.h"
#include "Languages.h"


class ScreenScraperApis
{
  public:
    /*!
     * @brief Interface to retrieve credentials
     */
    class IConfiguration
    {
      public:
        //!Image type
        enum class Image
        {
          None,             //!< No image
          ScreenshotIngame, //!< In-game screenshot
          ScreenshotTitle,  //!< Title screenshot
          Wheel,            //!< Wheel
          Marquee,          //!< Marquee
          Box2d,            //!< Box 2d (front)
          Box3d,            //!< Box 3d (front)
          MixV1,            //!< Server-side mix v1
          MixV2,            //!< Server-side mix v2
        };
        //! Video type
        enum class Video
        {
          None,      //!< No video
          Raw,       //!< Unprocessed video
          Optimized, //!< Optimized video
        };

        //! Get Login
        virtual std::string GetLogin() const = 0;
        //! Get password
        virtual std::string GetPassword() const = 0;
        //! Get favorite language
        virtual Languages GetFavoriteLanguage() const = 0;
        //! Get favorite region
        virtual Regions::GameRegions GetFavoriteRegion() const = 0;
        //! Get main image type
        virtual ScreenScraperEnums::ScreenScraperImageType GetImageType() const = 0;
        //! Get thumbnail image type
        virtual ScreenScraperEnums::ScreenScraperImageType GetThumbnailType() const = 0;
        //! GetVideo type
        virtual ScreenScraperEnums::ScreenScraperVideoType GetVideo() const = 0;
        //! Check if marquee are required
        virtual bool GetWantMarquee() const = 0;
        //! Check if wheel are required
        virtual bool GetWantWheel() const = 0;
        //! Check if marquee are required
        virtual bool GetWantManual() const = 0;
        //! Check if wheel are required
        virtual bool GetWantMaps() const = 0;
        //! Check if p2k are required
        virtual bool GetWantP2K() const = 0;
    };

    //! User object (reduced to Recalbox needs)
    struct User
    {
      int mThreads = 1;        //! maxthreads
      int mRequestDone = 0;    //! requeststoday
      int mMaxRatePerMin = 0;  //! maxrequestspermin
      int mMaxRatePerDay = 0;  //! maxrequestsperday
      std::string mRegion;     //! favregion
    };

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
      //! Rom regions
      unsigned int mRomRegions;
      //! Rom crc
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
        //! Main image
        std::string mImage;
        //! Main image file format
        std::string mImageFormat;
        //! main image md5
        std::string mImageMd5;
        //! Main image size
        long long mImageSize;

        //! Thumbnail image
        std::string mThumbnail;
        //! Thumbnail image file format
        std::string mThumbnailFormat;
        //! Thumbnail image file md5
        std::string mThumbnailMd5;
        //! Thumbnail image
        long long mThumbnailSize;

        //! Video
        std::string mVideo;
        //! Video file format
        std::string mVideoFormat;
        //! Video file md5
        std::string mVideoMd5;
        //! Video
        long long mVideoSize;

        //! Marquee
        std::string mMarquee;
        //! Marquee file format
        std::string mMarqueeFormat;
        //! Marquee file md5
        std::string mMarqueeMd5;
        //! Marquee
        long long mMarqueeSize;

        //! Wheel
        std::string mWheel;
        //! Wheel file format
        std::string mWheelFormat;
        //! Wheel file md5
        std::string mWheelMd5;
        //! Wheel
        long long mWheelSize;

        //! Manual
        std::string mManual;
        //! Manual file format
        std::string mManualFormat;
        //! Manual file md5
        std::string mManualMd5;
        //! Manual
        long long mManualSize;

        //! Maps
        std::string mMaps;
        //! Maps file format
        std::string mMapsFormat;
        //! Maps file md5
        std::string mMapsMd5;
        //! Maps
        long long mMapsSize;
      }
      MediaSources;
    };

  private:
    static constexpr const char* API_DEV_U = "\xF1\x5A\xA8\x46\x25\xDE\x48\x2A";
    static constexpr const char* API_DEV_P = "\xC0\x0C\x80\x45\x30\xD6\x7F\x6A\x69\xB5\x02\x9D\xAD\x6B\xA3\x33\xE6\x7A\xE8\x4E";
    static constexpr const char* API_DEV_K = "\x83\x2E\xA9\xF4\x05\x67\xC1\xDB\xB1\x65\xC7\x0D\xFE\x29\xA3\x48";

    //! Credential interface
    IConfiguration& mConfiguration;

    //! Htpp client
    Http mClient;

    //! Api type
    enum class Api
    {
      UserInfo, //!< Get user information
      GameInfo, //!< Get game information
    };

    //! X-Or the Space Sheriff
    static std::string XOrTheSpaceSheriff(const std::string& _input, const std::string& key)
    {
      std::string buffer = _input;
      for (int i = (int) _input.size(); --i >= 0;)
        buffer[i] = (char) ((unsigned char)_input[i] ^ (unsigned char)(key[i % key.size()] + (i * 17)));
      return buffer;
    }

    /*!
     * @brief Build the common URL part of all apis
     * @param api Api type
     * @return Url
     */
    std::string BuildUrlCommon(Api api);

    /*!
     * @brief Build a jeuInfo url
     * @param system ScreenScraper system id
     * @param path File path
     * @param crc32 optionnal CRC32
     * @param md5 optionnal MD5
     * @return Url
     */
    std::string BuildUrlGameInfo(int system, const Path& path, const std::string& crc32, const std::string& md5, long long size);

    /*!
     * @brief Deserialize a gameinfo request JSON result into a Game object
     * @param json Input json
     * @param game Output Game object
     */
    void DeserializeGameInformation(const std::string& json, Game& game, Path path);

    static std::string GetRequiredRegion(unsigned intromRegion, const Path& path, Regions::GameRegions favoriteRegion);

    /*!
     * @brief Extract the best-matching text from an array of regionalized texts, regarding the given preferred region
     * @param array Json array of regionalized texts
     * @param requiredRegion Required region
     * @return Best-matching text
     */
    static std::string ExtractRegionalizedText(const rapidjson::Value& array, const std::string& requiredRegion, unsigned int romRegion);

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

    static std::string CleanGameName(const std::string& source);

  public:
    /*!
     * @brief Constructor
     * @param credentials
     */
    explicit ScreenScraperApis(IConfiguration* credentials)
      : mConfiguration(*credentials)
    {
    }

    //! Get user
    User GetUserInformation();

    /*!
     * @brief Get Game informations
     * @param system ScreenScraper system id
     * @param path File path
     * @param crc32 optionnal CRC32
     * @param md5 optionnal MD5
     * @return Game structure with the result code in the mResult field
     */
    Game GetGameInformation(int system, const Path& path, const std::string& crc32, const std::string& md5, long long size);

    /*!
     * @brief Download a media into a target file
     * @param mediaurl Media url
     * @param to Filepath
     * @return ScrapeResult Request status
     */
    ScrapeResult GetMedia(const std::string& mediaurl, const Path& to, long long& size);
};

