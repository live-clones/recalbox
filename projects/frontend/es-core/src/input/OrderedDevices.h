//
// Created by thierry.imbert on 24/02/2020.
//
#pragma once

#include <input/Input.h>

class InputDevice;

//! Ordered devices regarding user configuration
class OrderedDevices
{
  private:
    /*!
     * @brief Ordered devices
     * Index: Index of device as seen in te user configuration
     * Value: Target input device
     */
    InputDevice* mDevicesIndexes[Input::sMaxInputDevices];
    //! Configured device count
    int mCount;

  public:
    /*!
     * @brief Default constructor
     */
    OrderedDevices()
      : mDevicesIndexes {},
        mCount(0)
    {
      for(int i = Input::sMaxInputDevices; --i >= 0; )
        mDevicesIndexes[i] = nullptr;
    }

    //! Device count
    int Count() const { return mCount; }

    /*!
     * @brief Get device at given index
     * @param i Device Index
     * @return InputDevice reference
     */
    const InputDevice* Device(int i) const { return mDevicesIndexes[i]; }

    /*!
     * @brief Set device at given indec
     * @param i Inde
     * @param device InputDevice object
     */
    void SetDevice(int i, InputDevice* device) { if ((unsigned int)i < (unsigned int)Input::sMaxInputDevices) mDevicesIndexes[i] = device; }

    /*!
     * @brief Shrink deices so there is no hole between
     */
    void Shrink()
    {
      int count = 0;
      for (auto Device : mDevicesIndexes)
      {
        mDevicesIndexes[count] = Device;
        if (mDevicesIndexes[count] != nullptr) ++count;
      }
      mCount = count;
    }
};

