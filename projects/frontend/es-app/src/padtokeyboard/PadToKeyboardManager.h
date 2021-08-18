//
// Created by bkg2k on 22/02/2020.
//
#pragma once

#include <input/InputManager.h>
#include <utils/os/system/Thread.h>
#include <padtokeyboard/pad/MappingConfiguration.h>
#include <padtokeyboard/mouse/VirtualMouse.h>

class PadToKeyboardManager : private Thread
{
  private:
    //! Ordered device configuration
    OrderedDevices mConfiguration;

    //! Mapping from pad items to keys
    MappingConfiguration mMappingConfiguration;
    //! Full pad configuration (translated from InputDevice)
    PadConfiguration mPadConfiguration;
    //! Pad event reader
    Pad mPadReader;
    //! Keyboard event writer
    VirtualKeyboard mKeyboardWriter;
    //! Mouse event writer
    VirtualMouse mMouseWriter;

    //! This instance has been fully configured and has been started
    bool mValid;

    /*
     * Thread implementation
     */

    /*!
     * @brief Main runner
     */
    void Run() override;

  public:
    /*!
     * @brief Buidl a manager using ther given ordered pad configuration
     * @param orderedDevices Ordered devices
     */
    explicit PadToKeyboardManager(const OrderedDevices& orderedDevices, const Path& rompath);

    /*!
     * @brief Start the Pad 2 keyboard thread
     */
    void StartMapping();

    /*!
     * @brief Stop the thread, and wait for its completion
     */
    void StopMapping();

    /*!
     * @brief Check if the Pad2Key is properly configured and running
     * @return True if everything is running fine, false otherwise
     */
    bool IsValid() const { return mValid; }
};
