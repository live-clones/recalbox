//
// Created by bkg2k on 17/03/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <scraping/scrapers/screenscraper/ScreenScraperEnums.h>
#include <scraping/scrapers/screenscraper/Languages.h>
#include <games/classifications/Regions.h>

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

    //! Reinitialize configuration
    virtual void ResetConfiguration() = 0;

    //! Get Token
    virtual std::string GetBearer() const = 0;
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
