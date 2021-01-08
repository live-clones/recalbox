//
// Created by bkg2k on 28/10/2019.
//
#pragma once

#include <string>

class InputEvent
{
  public:
    //! Event type
    enum class EventType: int
    {
      Unknown, //!< Unknown/Not initialized event
      Axis,    //!< Analog axis with values from -32768 to +32767
      Button,  //!< Button with binary state
      Hat,     //!< DPAD bitflag
      Key,     //!< Keyboard key
    };

  private:
    int       mDeviceIdentifier; //!< SDL Device Identifier or -1 for keyboard
    EventType mType;             //!< Event type
    int       mId;               //!< Event identifier
    int       mValue;            //!< Event value - type dependent
    int       mCode;             //!< SDL Raw code

  public:
    //! Define virtual default mousse device
    static constexpr int sMousseDevice = 0;
    //! Define virtual default keyboard device
    static constexpr int sKeyboardDevice = -1;
    //! Define empty device device
    static constexpr int sEmptyDevice = -2;

    /*!
     * @brief Default constructor
     */
    InputEvent()
      : mDeviceIdentifier(sKeyboardDevice),
        mType(EventType::Unknown),
        mId(-1),
        mValue(-999),
        mCode(-1)
    {
    }

    /*!
     * @brief Full constructor. Build an immutable Inpout event object
     * @param dev Device index
     * @param t Type
     * @param i Identifier
     * @param val Value
     * @param conf True if the input event is configured
     */
    InputEvent(int dev, EventType type, int id, int val)
      : mDeviceIdentifier(dev),
        mType(type),
        mId(id),
        mValue(val),
        mCode(-1)
    {
    }

    /*!
     * @brief Full constructor. Build an immutable Inpout event object
     * @param dev Device index
     * @param t Type
     * @param i Identifier
     * @param val Value
     * @param conf True if the input event is configured
     */
    InputEvent(int dev, EventType type, int id, int val, int code)
      : mDeviceIdentifier(dev),
        mType(type),
        mId(id),
        mValue(val),
        mCode(code)
    {
    }

    /*
     * Accessors
     */

    int Device()     const { return mDeviceIdentifier; }
    EventType Type() const { return mType; }
    int Id()         const { return mId; }
    int Value()      const { return mValue; }
    int Code()       const { return mCode; }

    /*
     * Special accessor for configuration convenience
     */

    bool AnyButtonPressed() const { return (mType == EventType::Button) && (mValue != 0); }
    bool AnyButtonReleased() const { return (mType == EventType::Button) && (mValue == 0); }
    bool AnythingPressed() const { return (mValue != 0); }
    bool AnythingReleased() const { return (mValue == 0); }

    /*!
     * @brief Convert current event to a string representation
     * @return String representation of the c urrent event
     */
    std::string ToString() const;

    /*!
     * @brief Get the raw SDL2 code for the current event.
     * Some emulators require raw code in their configurations
     */
    void StoreSDLCode(int deviceIndex);

    /*
     * Helpers
     */

    static std::string TypeToString(InputEvent::EventType type);
    static EventType StringToType(const std::string& type);
    static const char* HatDir(int val);
};
