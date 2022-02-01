//
// Created by bkg2k on 21/12/2019.
//
#pragma once

#include <utils/storage/Queue.h>
#include <utils/storage/HashMap.h>
#include <SDL.h>
#include "PadConfiguration.h"
#include "PadItems.h"

class Pad
{
  public:
    //! Pad event
    struct Event
    {
      PadItems      Item;   //!< Target item
      int           Value;  //!< Analog value (for axis)
      bool          On;     //!< Numeric value on/off (true/false)
      bool          Analog; //!< Use Analog value, not Numeric value
      unsigned char Pad;    //!< Pad number
    } __attribute__((packed));

  private:
    //! Joystick deadzone, in the 0-32767 range
    static constexpr int sJoystickDeadZone = 12000;
    //! Axis subrnage
    static constexpr int sAxisSubRange = 16;

    //! Event queue
    Queue<Event> mEventQueue;

    //! Global configuration reference
    const Configuration* mConfiguration;
    //! Pad configurations references
    const PadConfiguration& mPadConfiguration;
    //! SDL Index to Recalbox Index
    HashMap<SDL_JoystickID, int> mSdlToRecalboxIndex;
    //! Hotkey state per device
    bool mHotkeyOnOff[Input::sMaxInputDevices];
    //! Numerical value previous state
    int mItemOnOff[Input::sMaxInputDevices];
    //! 32-ranged value previous state
    unsigned char mItemValues[Input::sMaxInputDevices][16];
    //! Devices readiness
    bool mReady;

  public:
    /*!
     * @brief Constructor
     * @param padConfiguration Pad Configuration
     * @param orderedDevices ordered devices
     */
    explicit Pad(const PadConfiguration& padConfiguration);

    /*!
     * @brief Push a raw SDL2 event to be translated in 0, 1 or more pad events
     * @param event SDL event
     */
    void PushSDL2Event(const SDL_Event& event);

    /*!
     * @brief Get next pad event
     * @param event Output event
     * @return True if an event have been read. False if the devices have been released
     */
    bool PopPadEvent(Pad::Event& event);

    /*!
     * @brief Open all configured devices from the given ordered device list
     */
    void Open(const OrderedDevices& orderedDevices);

    /*!
     * @brief Check device readiness
     * @return True if configured devices have been opened successfully
     */
    bool Ready() const { return mReady; }

    /*!
     * @brief Release all devices
     */
    void Release();
};
