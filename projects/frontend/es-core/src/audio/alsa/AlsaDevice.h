//
// Created by bkg2k on 13/08/2020.
//
#pragma once

#include <string>
#include "audio/NameFiltering.h"

class AlsaDevice
{
  private:
    // Internal references
    std::string mDeviceName; //!< Device name
    int mDeviceId;           //!< Device ID
    int mSubDeviceCount;     //!< Sub device copunt

  public:
    //! Default constructor
    AlsaDevice(int id, const std::string& name, int subdevices)
    {
      mDeviceId = id;
      mDeviceName = NameFiltering::Filter(name, NameFiltering::Source::Device);
      mSubDeviceCount = subdevices;
    }

    //! Get mixer identifier
    int Identifier() const { return mDeviceId; }
    //! Get mixer name
    const std::string& Name() const { return mDeviceName; }
};
