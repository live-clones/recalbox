//
// Created by bkg2k on 28/10/2019.
//
#pragma once

#include <string>
#include <SDL.h>
#include <utils/Strings.h>

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
    int       mDevice; //!< SDL Device index or -1 for keyboard
    EventType mType;   //!< Event type
    int       mId;     //!< Event identifier
    int       mValue;  //!< Event value - type dependent
    int       mCode;   //!< SDL Raw code

  public:
    //! Define virtual default keyboard device
    static constexpr int sKeyboardDevice = -1;

    /*!
     * @brief Default constructor
     */
    InputEvent()
      : mDevice(sKeyboardDevice),
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
      : mDevice(dev),
        mType(type),
        mId(id),
        mValue(val),
        mCode(-1)
    {
    }

    /*
     * Accessors
     */

    int Device()     const { return mDevice; }
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
    std::string ToString() const
    {
      switch (mType)
      {
        case EventType::Button:  return std::string("Button ").append(Strings::ToString(mId));
        case EventType::Axis:    return std::string("Axis ").append(Strings::ToString(mId)).append(1, (mValue > 0 ? '+' : '-'));
        case EventType::Hat:     return std::string("Hat ").append(Strings::ToString(mId)).append(1, ' ').append(HatDir(mValue));
        case EventType::Key:     return std::string("Key ").append(SDL_GetKeyName((SDL_Keycode) mId));
        case EventType::Unknown:
        default: break;
      }
      return std::string("Input to string error");
    }

    #ifndef SDL_JOYSTICK_IS_OVERRIDEN_BY_RECALBOX
      #define SDL_JoystickAxisEventCodeById(mDevice, mId) (-1)
      #define SDL_JoystickButtonEventCodeById(mDevice, mId) (-1)
      #define SDL_JoystickHatEventCodeById(mDevice, mId) (-1)
    #endif


    /*!
     * @brief Get the raw SDL2 code for the current event.
     * Some emulators require raw code in their configurations
     */
    void StoreSDLCode()
    {
      switch (mType)
      {
        case EventType::Axis:
          mCode = SDL_JoystickAxisEventCodeById(mDevice, mId);
          break;
        case EventType::Button:
          mCode = SDL_JoystickButtonEventCodeById(mDevice, mId);
          break;
        case EventType::Hat:
          mCode = SDL_JoystickHatEventCodeById(mDevice, mId);
          break;
        case EventType::Key:
        case EventType::Unknown:
        default: break;
      }
    }

    /*
     * Helpers
     */

    static std::string TypeToString(InputEvent::EventType type);
    static EventType StringToType(const std::string& type);
    static const char* HatDir(int val)
    {
      if ((val & SDL_HAT_UP) != 0)    return "up";
      if ((val & SDL_HAT_DOWN) != 0)  return "down";
      if ((val & SDL_HAT_LEFT) != 0)  return "left";
      if ((val & SDL_HAT_RIGHT) != 0) return "right";
      return "neutral?";
    }
};
