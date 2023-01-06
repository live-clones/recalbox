//
// Created by bkg2k on 05/01/23.
//
#pragma once

#include <utils/String.h>
#include <vector>

class DeviceStatus
{
  public:
    //! Signal strength
    enum class SignalStrength
    {
      Unknown, //!< Unknown RSSI ( >= 10000 )
      Best,    //!< Best ( >= 0 )
      Good,    //!< Good ( >= -30 )
      Average, //!< Average ( >= 60 )
      Bad,     //!< Bad ( >= 90 )
      Awfull,  //!< Very bad ( < 90 )
    };

    //! Device structure
    struct Device
    {
      String UUID;    //!< Device identifier
      String Name;    //!< Device name
      String Alias;   //!< Device alias
      String Address; //!< Device Address
      SignalStrength RSSI; //!< Signal strength
      bool Paired;    //!< Paired status
      bool Trusted;   //!< Trusted status
      bool Connected; //!< Connected status
      bool Blocked;   //!< Blocked status
    };

    //! Get remaining time
    [[nodiscard]] int RemainingTime() const { return mRemainingTime;}
    //! Get total time
    [[nodiscard]] int TotalTime() const { return mTotalTime; }

    //! Get device statuses
    [[nodiscard]] const std::vector<Device>& Devices() const { return mDevices; }

    //! Valid?
    [[nodiscard]] bool IsValid() const { return mRemainingTime > 0; }

    /*!
     * @brief Create new DeviceStatus class instance, deserializing the given message
     * @param message Input message
     * @return New device statuses
     */
    static DeviceStatus Deserialize(const String& message);

    /*!
     * @brief Return en empty DeviceStatus instance
     * @return Empty DeviceStatus instance
     */
    static DeviceStatus CreateEmpty() { return DeviceStatus(); }

  private:
    //! Device list
    std::vector<Device> mDevices;
    //! Pairing remaining time
    int mRemainingTime;
    //! Pairing total time
    int mTotalTime;

    //! Private constructor
    DeviceStatus()
      : mRemainingTime(-1)
      , mTotalTime(-1)
    {}
};
