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
    bool IsCrtAdapterAttached() const override { return true; }

    //! This adapter is an RGB Dual
    CrtAdapterType GetCrtAdapter() const override { return CrtAdapterType::RGBDual; }

    //! RGB Dual has support for 31khz
    bool Has31KhzSupport() const override { return true; }

    //! Return select output frequency
    HorizontalFrequency GetHorizontalFrequency() const override { return GetRGBDual31khzSwitchState() ? HorizontalFrequency::KHz31 : HorizontalFrequency::KHz15; }

    //! This adapter has no support of forced 50hz
    bool HasForced50hzSupport() const override { return true; }

    //! Get 50hz switch state
    bool MustForce50Hz() const override { return GetRGBDual50hzSwitchState(); }

  private:
    static constexpr const char* sRGBDual31khzSwitch = "/sys/devices/platform/recalboxrgbdual/dipswitch-31khz/value";
    static constexpr const char* sRGBDual50hzSwitch = "/sys/devices/platform/recalboxrgbdual/dipswitch-50hz/value";

    //! Get 31khz switch state on RGB dual board
    static bool GetRGBDual31khzSwitchState() { return Files::LoadFile(Path(sRGBDual31khzSwitch)) == "0\n"; }
    //! Get 50hz switch state on RGB dual board
    static bool GetRGBDual50hzSwitchState() { return Files::LoadFile(Path(sRGBDual50hzSwitch)) == "0\n"; }
};



