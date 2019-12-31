//
// Created by bkg2k on 31/12/2019.
//
#pragma once

class Bios;

class IBiosScanReporting
{
  public:
    /*!
     * @brief Report a new Bios has been scanned
     * @param bios Newly scanned bios
     */
    virtual void ScanProgress(const Bios& bios) = 0;

    /*!
     * @brief Report the bios scan is complete
     */
    virtual void ScanComplete() = 0;
};
