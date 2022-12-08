//
// Created by bkg2k on 05/01/23.
//
#pragma once

#include "guis/Gui.h"
#include "components/ImageComponent.h"
#include "utils/gl/Rectangle.h"

class BluetoothOverlayGUI : public Gui
{
  public:
    /*!
     * @brief Constructor
     * @param windowManager Window manager reference
     */
    BluetoothOverlayGUI(WindowManager& windowManager);

    /*!
     * @brief Update notification information
     * @param remainingMs Remaining time in second
     * @param totalMs Total time in second
     */
    void UpdatePairingTime(int remaining, int total)
    {
      mRemaining = remaining * 1000;
      mTotal = total * 1000;
    }

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
};
