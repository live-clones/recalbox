//
// Created by bkg2k on 23/03/2022.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include "RecalboxEndPoints.h"
#include <scraping/scrapers/screenscraper/ScreenScraperEngineBase.h>

class RecalboxEngineImplementation : public ScreenScraperEngineBase
{
  public:
    RecalboxEngineImplementation(IScraperEngineFreezer* freezer)
      : ScreenScraperEngineBase(Endpoint(), freezer)
      , mLanguage(Languages::Unknown)
      , mRegion(Regions::GameRegions::Unknown)
      , mMainImage(ScreenScraperEnums::ScreenScraperImageType::MixV1)
      , mThumbnailImage(ScreenScraperEnums::ScreenScraperImageType::None)
      , mVideo(ScreenScraperEnums::ScreenScraperVideoType::None)
      , mWantMarquee(false)
      , mWantWheel(false)
      , mWantManual(false)
      , mWantMaps(false)
      , mWantP2K(false)
    {
    }

  private:
    //! Recalbox credentials: Login
    std::string mLogin;
    //! Recalbox credentials: Password
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

    //! EndPoint providers
    static RecalboxEndPoints& Endpoint()
    {
      static RecalboxEndPoints sEndPoints;
      return sEndPoints;
    }

    /*
     * ScreenScraperApis::IConfiguration implementation
     */

    //! Reinitialize configuration
    void ResetConfiguration() override
    {
      RecalboxConf& conf = RecalboxConf::Instance();
      // Credentials
      mLogin          = Strings::Trim(conf.GetRecalboxScraperLogin());
      mPassword       = Strings::Trim(conf.GetRecalboxScraperPassword());

      // Language & region
      mRegion         = conf.GetScreenScraperRegion();
      mLanguage       = LanguagesTools::GetScrapingLanguage();

      // Medias
      mMainImage      = conf.GetScreenScraperMainMedia();
      mThumbnailImage = conf.GetScreenScraperThumbnail();
      mVideo          = conf.GetScreenScraperVideo();
      mWantMarquee    = conf.GetScreenScraperWantMarquee();
      mWantWheel      = conf.GetScreenScraperWantWheel();
      mWantManual     = conf.GetScreenScraperWantManual();
      mWantMaps       = conf.GetScreenScraperWantMaps();
      mWantP2K        = conf.GetScreenScraperWantP2K();
    }

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
};



