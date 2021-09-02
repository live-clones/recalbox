//
// Created by bkg2k on 22/02/2020.
//
#pragma once

#include <padtokeyboard/pad/MappingConfiguration.h>
#include <sdl2/Sdl2Runner.h>
#include <padtokeyboard/mouse/VirtualMouse.h>

class PadToKeyboardManager : private ISdl2EventNotifier
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

    //! SDL Runner reverence
    Sdl2Runner& mSdlRunner;

    //! This instance has been fully configured and has been started
    bool mValid;

    /*
     * ISdl2EventNotifier implementation
     */

     /*!
      * @brief Receive SDL2 events. One call per event
      * @param event Received event
      */
    void Sdl2EventReceived(const SDL_Event& event) override;

  public:
    /*!
     * @brief Buidl a manager using ther given ordered pad configuration
     * @param orderedDevices Ordered devices
     */
    explicit PadToKeyboardManager(const OrderedDevices& orderedDevices, const Path& rompath, Sdl2Runner& sdl2Runner);

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
