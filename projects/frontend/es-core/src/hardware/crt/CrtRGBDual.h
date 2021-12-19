//
// Created by bkg2k on 19/12/2021.
//
// As part of the RECALBOX Project
// http://www.recalbox.com
//
#pragma once

#include <hardware/crt/ICrtInterface.h>
#include <utils/Files.h>

class CrtRGBDual : public ICrtInterface
{
  public:
    //! Constructor
    explicit CrtRGBDual(bool automaticallyDetected) : ICrtInterface(automaticallyDetected) {}

    //! An RGB Dual is attached
    bool IsCrtAdapterAttached() override { return true; }

    //! This adapter is an RGB Dual
    CrtAdapterType GetCrtAdapter() override { return CrtAdapterType::RGBDual; }

    //! RGB Dual has support for 31khz
    bool Has31KhzSupport() override { return true; }

    //! Return select output frequency
    HorizontalFrequency GetHorizontalFrequency() override { return GetRGBDual31khzSwitchState() ? HorizontalFrequency::KHz31 : HorizontalFrequency::KHz15; }

    //! This adapter has no support of forced 50hz
    bool HasForced50hzSupport() override { return true; }

    //! Get 50hz switch state
    bool MustForce50Hz() override { return GetRGBDual50hzSwitchState(); }

  private:
    static constexpr const char* sRGBDual31khzSwitch = "/sys/nowhere/or/elsewhere";
    static constexpr const char* sRGBDual50hzSwitch = "/sys/nowhere/or/elsewhere";

    //! Get 31khz switch state on RGB dual board
    static bool GetRGBDual31khzSwitchState() { return Files::LoadFile(Path(sRGBDual31khzSwitch)) == "1"; }
    //! Get 50hz switch state on RGB dual board
    static bool GetRGBDual50hzSwitchState() { return Files::LoadFile(Path(sRGBDual50hzSwitch)) == "1"; }
};



