//
// Created by bkg2k on 24/02/2020.
//
#pragma once

#include <input/Input.h>
#include <input/InputDevice.h>

//! Ordered devices regarding user configuration
class OrderedDevices
{
  private:
    /*!
     * @brief Ordered devices
     * Index: Index of device as seen in te user configuration
     * Value: Target input device
     */
    InputDevice mDevicesIndexes[Input::sMaxInputDevices];
    //! Configured device count
    unsigned int mConfiguredBitFlags;
    //! Configured device count
    int mCount;

  public:
    /*!
     * @brief Default constructor
     */
    OrderedDevices()
      : mDevicesIndexes {},
        mConfiguredBitFlags(0),
        mCount(0)
    {
    }

    //! Device count
    int Count() const { return mCount; }

    /*!
     * @brief Check if the device at posiion i has been configured
     * @param i Device index
     * @return True if the device has been configured
     */
    bool IsConfigured(int i) const { return (mConfiguredBitFlags & (1 << i)) != 0; }

    /*!
     * @brief Get device at given index
     * @param i Device Index
     * @return InputDevice reference
     */
    const InputDevice& Device(int i) const { return mDevicesIndexes[i]; }

    /*!
     * @brief Set device at given indec
     * @param i Inde
     * @param device InputDevice object
     */
    void SetDevice(int i, const InputDevice& device)
    {
      if ((unsigned int)i < (unsigned int)Input::sMaxInputDevices)
      {
        mDevicesIndexes[i] = device;
        mConfiguredBitFlags |= 1 << i;
      }
      mCount = 0;
      for(int b = sizeof(mConfiguredBitFlags) * 8; --b >= 0; )
        if (((mConfiguredBitFlags >> b) & 1) != 0)
          mCount++;
    }
};

