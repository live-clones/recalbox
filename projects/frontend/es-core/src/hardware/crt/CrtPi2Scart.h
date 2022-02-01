//
// Created by bkg2k on 20/12/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <hardware/crt/ICrtInterface.h>

class CrtPi2Scart : public ICrtInterface
{
  public:
    //! Constructor
    explicit CrtPi2Scart(bool automaticallyDetected) : ICrtInterface(automaticallyDetected) {}

    //! Yes a CRT adapter is attached!
    bool IsCrtAdapterAttached() const override { return true; }

    //! This adapter is a Pi2Scart
    CrtAdapterType GetCrtAdapter() const override { return CrtAdapterType::Pi2Scart; }

    //! No 31khz support
    bool Has31KhzSupport() const override { return false; }

    //! This adapter is stuck on 15khz
    HorizontalFrequency GetHorizontalFrequency() const override { return ICrtInterface::HorizontalFrequency::KHz15; }

    //! This adapter has no support of forced 50hz
    bool HasForced50hzSupport() const override { return false; }

    //! This adapter cannot force 50hz
    bool MustForce50Hz() const override { return false; }
};
