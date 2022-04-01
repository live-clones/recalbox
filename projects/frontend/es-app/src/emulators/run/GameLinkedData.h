//
// Created by bkg2k on 26/12/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <emulators/run/NetPlayData.h>
#include <emulators/run/CrtData.h>
#include <emulators/run/PatchData.h>

class GameLinkedData
{
  public:
    /*!
     * @brief Configure Netplay data as client
     * @param coreName Libretro core
     * @param ip Target ip
     * @param port Target port
     * @param playerPassword Password for players
     * @param viewerPassword Password for viewers
     * @param asViewer True to oin as viewer, false to join as player
     */
    GameLinkedData(const std::string& coreName, const std::string& ip, int port, const std::string& playerPassword, const std::string& viewerPassword, bool asViewer)
      : mNetPlayData(coreName, ip, port, playerPassword, viewerPassword, asViewer)
    {
    }

    /*!
     * @brief Configure netplay data as host
     * @param port Listening port
     * @param playerPassword Password for players
     * @param viewerPassword Password for viewers
     */
    GameLinkedData(int port, const std::string& playerPassword, const std::string& viewerPassword)
      : mNetPlayData(port, playerPassword, viewerPassword)
    {
    }

    //! Default constructor
    GameLinkedData() = default;

    /*
     * Accessors
     */

    //! Get Netplay object
    const NetPlayData& NetPlay()const { return mNetPlayData; }

    //! Get writable Crt object
    CrtData& ConfigurableCrt() { return mCrtData; }

    //! Get read only Crt object
    const CrtData& Crt() const { return mCrtData; }

    //! Get writable Patch data
    PatchData& ConfigurablePath() { return mPatchData; }

    //! Get read only Patch data
    const PatchData& Patch() const { return mPatchData; }

  private:
    //! Netplay data
    NetPlayData mNetPlayData;

    //! CRT data
    CrtData mCrtData;

    PatchData mPatchData;
};