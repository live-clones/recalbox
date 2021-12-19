//
// Created by bkg2k on 20/12/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <hardware/crt/ICrtInterface.h>

class CrtRGBPi : public ICrtInterface
{
  public:
    //! Constructor
    explicit CrtRGBPi(bool automaticallyDetected) : ICrtInterface(automaticallyDetected) {}

    //! Yes a CRT adapter is attached!
    bool IsCrtAdapterAttached() override { return true; }

    //! This adapter is an RGBPi
    CrtAdapterType GetCrtAdapter() override { return CrtAdapterType::RGBPi; }

    //! No 31khz support
    bool Has31KhzSupport() override { return false; }

    //! This adapter is stuck on 15khz
    HorizontalFrequency GetHorizontalFrequency() override { return ICrtInterface::HorizontalFrequency::KHz15; }

    //! This adapter has no support of forced 50hz
    bool HasForced50hzSupport() override { return false; }

    //! This adapter cannot force 50hz
    bool MustForce50Hz() override { return false; }
};
