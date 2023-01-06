//
// Created by bkg2k on 05/01/23.
//
#pragma once

#include "guis/Gui.h"
#include "components/ImageComponent.h"
#include "utils/gl/Rectangle.h"
#include "DeviceStatus.h"
#include "IBluetoothDeviceStatusListener.h"

class BluetoothOverlayGUI
  : public Gui
  , public IBluetoothDeviceStatusListener
{
  public:
    /*!
     * @brief Constructor
     * @param windowManager Window manager reference
     */
    explicit BluetoothOverlayGUI(WindowManager& windowManager);

    /*
     * Component/Gui overrides
     */

    /*!
     * @brief Update bluetooth timer
     * @param deltaTime Elapsed time from the previous frame, in millisecond
     */
    void Update(int deltaTime) override;

    /*!
     * @brief Draw bluetooth notification
     * @param parentTrans Transformation
     */
    void Render(const Transform4x4f& parentTrans) override;

  private:
    //! Remaining time in ms
    int mRemaining;
    //! Total time in ms
    int mTotal;


    //! BT icon
    ImageComponent mBluetoothIcon;
    //! Progress bar position
    Rectangle mProgressBar;

    /*
     * IBluetoothDeviceStatusListener implementation
     */

    /*!
     * @brief Receive latest device status
     * @param status DeviceStatus instance
     */
    void ReceiveBluetoothDeviceStatus(DeviceStatus& status) final
    {
      mRemaining = status.RemainingTime() * 1000;
      mTotal = status.TotalTime() * 1000;
    }
};
