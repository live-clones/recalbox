//
// Created by bkg2k on 06/01/23.
//
#pragma once

#include "DeviceStatus.h"

class IBluetoothDeviceStatusListener
{
  public:
    //! Destructor
    virtual ~IBluetoothDeviceStatusListener() = default;

    /*!
     * @brief Receive latest device status
     * @param status DeviceStatus instance
     */
    virtual void ReceiveBluetoothDeviceStatus(DeviceStatus& status) = 0;
};
