//
// Created by bkg2k on 07/12/2019.
//
#pragma once

#include <string>
#include <utils/Http.h>
#include <utils/datetime/DateTime.h>
#include <rapidjson/document.h>
#include <scraping/new/ScrapeResult.h>


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
        virtual std::string GetFavoriteLanguage() const = 0;
        //! Get favorite region
        virtual std::string GetFavoriteRegion() const = 0;
        //! Get main image type
        virtual Image GetImageType() const = 0;
        //! Get thumbnail image type
        virtual Image GetThumbnailType() const = 0;
        //! GetVideo type
        virtual Video GetVideo() const = 0;
        //! Check if marquee are required
        virtual bool GetWantMarquee() const = 0;
        //! Check if wheel are required
        virtual bool GetWantWheel() const = 0;
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
      //! Editor
      DateTime mReleaseDate;
      //! Rating
      float mRating;

      //! Media Urls
      struct MediaUrl
      {
        //! Main image
        std::string mImage;
        //! Main image file format
        std::string mImageFormat;
        //! Main image size
        long long mImageSize;

        //! Thumbnail image
        std::string mThumbnail;
        //! Thumbnail image file format
        std::string mThumbnailFormat;
        //! Thumbnail image
        long long mThumbnailSize;

        //! Video
        std::string mVideo;
        //! Video file format
        std::string mVideoFormat;
        //! Video
        long long mVideoSize;
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
    void DeserializeGameInformation(const std::string& json, Game& game);

    /*!
     * @brief Extract the best-matching text from an array of regionalized texts, regarding the given preferred region
     * @param array Json array of regionalized texts
     * @param preferedregion Preferred region
     * @return Best-matching text
     */
    static std::string ExtractRegionalizedText(const rapidjson::Value& array, const std::string& preferedregion);

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
    static std::string ExtractMedia(const rapidjson::Value& medias, const char* type, const std::string& region, std::string& format, long long& size);

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

