//
// Created by bkg2k on 26/12/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <hardware/Board.h>

class CrtData
{
  public:
    //! Video system
    enum class VideoRegion
    {
      Auto, //!< Automatic selection
      Pal,  //!< Forced Pal
      Ntsc, //!< Forced Ntsc
    };

    //! Default constructor
    CrtData()
      : mRegionConfigured(false)
      , mHighResolutionConfigured(false)
      , mRegion(VideoRegion::Auto)
      , mHighResoution(false)
    {
    }

    /*!
     * @brief Check if there is a CRT board and the user requested to choose individual 480 or 240 options
     * @return True if the class needs to be configured, false otherwise
     */
    bool IsHighResolutionConfigured() const
    {
      if (!mHighResolutionConfigured)
        if (Board::Instance().CrtBoard().GetCrtAdapter() == CrtAdapterType::RGBDual)
          if (RecalboxConf::Instance().GetSystemCRTGameOptions())
            return true;
      return false;
    }

    /*!
     * @brief Check if there is a CRT board and the user requested to choose individual NTSC options
     * @return True if the class needs to be configured, false otherwise
     */
    bool IsRegionConfigured() const
    {
      if (!mRegionConfigured)
        if (Board::Instance().CrtBoard().GetCrtAdapter() == CrtAdapterType::RGBDual)
          if (RecalboxConf::Instance().GetSystemCRTGameOptions())
            return true;
      return false;
    }

    /*!
     * @brief Configure crt data
     * @param ntsc True for NTSC, false for PAL
     */
    void ConfigureNTSC(VideoRegion video)
    {
      mRegion = video;
      mRegionConfigured = true;
    }

    /*!
     * @brief Configure crt data
     * @param highRez True for 480, false for 240
     */
    void ConfigureHighResolution(bool highRez)
    {
      mHighResoution = highRez;
      mHighResolutionConfigured = true;
    }

    /*!
     * @brief Check if the target system requires choosing between PAL or NTSC
     * @param system target system
     * @return True if the choice is required, false otherwise
     */
    bool MustChoosePALorNTSC(const SystemData& system) const
    {
      return system.Descriptor().CrtMultiRegion() &&        // System must support multi-region
             !Board::Instance().CrtBoard().MustForce50Hz(); // & hardware must not force 50hz
    }

    /*!
     * @brief Check if the target system requires choosing between 240 or 480
     * @param system target system
     * @return True if the choice is required, false otherwise
     */
    bool MustChooseHighResolution(const SystemData& system) const
    {
      return system.Descriptor().CrtHighResolution();
    }
/*
  *//*!
  * @brief Check if high resolution is a progressive one
  * @return True if high resolution is a progressive one, false otherwise
  *//*
  bool HighResolutionIsProgressive() const
  {

    if (Board::Instance().CrtBoard().GetHorizontalFrequency() == ICrtInterface::HorizontalFrequency::KHz31)

  }*/

    /*
     * Accesors
     */

    bool HighResolution() const { return mHighResoution; }

    VideoRegion Region() const { return mRegion; }

  private:
    //! NTSC configured
    bool mRegionConfigured;
    //! 480i configured
    bool mHighResolutionConfigured;
    //! Video system (default: auto
    VideoRegion mRegion;
    //! 480? (default: 240p)
    bool mHighResoution;
};