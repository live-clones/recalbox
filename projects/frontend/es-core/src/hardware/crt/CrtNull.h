//
// Created by bkg2k on 19/12/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <hardware/crt/ICrtInterface.h>

class CrtNull : public ICrtInterface
{
  public:
    //! Constructor
    explicit CrtNull(bool automaticallyDetected) : ICrtInterface(automaticallyDetected) {}

    bool IsCrtAdapterAttached() const override { return false; }

    CrtAdapterType GetCrtAdapter() const override { return CrtAdapterType::None; }

    bool Has31KhzSupport() const override { return false; }

    HorizontalFrequency GetHorizontalFrequency() const override { return ICrtInterface::HorizontalFrequency::KHz15; }

    bool HasForced50hzSupport() const override { return false; }

    bool MustForce50Hz() const override { return false; }
};



