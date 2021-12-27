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
      , mInterlacedConfigured(false)
      , mRegion(VideoRegion::Auto)
      , mInterlaced(false)
    {
    }

    /*!
     * @brief Check if there is a CRT board and the user requested to choose individual 480i options
     * @return True if the class needs to be configured, false otherwise
     */
    bool IsInterlacedConfigured() const
    {
      if (!mInterlacedConfigured)
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
     * @param i480 True for 480i, false for 240p
     */
    void Configure480i(bool i480)
    {
      mInterlaced = i480;
      mInterlacedConfigured = true;
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
     * @brief Check if the target system requires choosing between 240p or 480i
     * @param system target system
     * @return True if the choice is required, false otherwise
     */
    bool MustChoose240or480i(const SystemData& system) const
    {
      return system.Descriptor().CrtInterlaced();
    }

    /*
     * Accesors
     */

    bool Interlaced() const { return mInterlaced; }

    VideoRegion Region() const { return mRegion; }

  private:
    //! NTSC configured
    bool mRegionConfigured;
    //! 480i configured
    bool mInterlacedConfigured;
    //! Video system (default: auto
    VideoRegion mRegion;
    //! 480e? (default: 240p)
    bool mInterlaced;
};