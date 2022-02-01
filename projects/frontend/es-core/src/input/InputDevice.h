#pragma once

#include <string>
#include <input/InputEvent.h>
#include <input/InputCompactEvent.h>
#include <SDL2/SDL_joystick.h>
#include <pugixml/pugixml.hpp>

/*!
 * @brief Hold input configurations for a given device
 */
class InputDevice
{
  public:
    enum class Entry
    {
      None,
      Start,
      Select,
      Hotkey,
      A,
      B,
      X,
      Y,
      L1,
      R1,
      L2,
      R2,
      L3,
      R3,
      Up,
      Right,
      Down,
      Left,
      Joy1AxisH,
      Joy1AxisV,
      Joy2AxisH,
      Joy2AxisV,
      VolumeUp,
      VolumeDown,
      BrightnessUp,
      BrightnessDown,
      __Count,
    };

    //! Maximum Axis
    static constexpr int sMaxAxis = 8;

  private:
    InputEvent mInputEvents[(int)Entry::__Count]; //!< Entry configurations
    char mDeviceName[128];                        //!< Device name: Keyboard or Pad/joystick name
    int mDeviceNameLength;                        //!< Device name length in char
    SDL_JoystickGUID mDeviceGUID;                 //!< GUID
    SDL_Joystick* mDeviceSDL;                     //!< SDL2 structure
    int mDeviceId;                                //!< SDL2 Joystick Identifier
    int mDeviceIndex;                             //!< SDL2 Joystick Index
    int mDeviceNbAxes;                            //!< Axis count
    int mDeviceNbHats;                            //!< Hat count
    int mDeviceNbButtons;                         //!< Button count
    int mConfigurationBits;                       //!< Configured entries bitflag
    int mPreviousAxisValues[sMaxAxis];            //!< Recorded axis value for move detection
    //! This special flags is managed by configuration UI to keep the
    //! "configure" flag in compact event while entries are configured
    bool mConfiguring;

    /*!
     * @brief Convert Entry to Compact Entry.
     * Joysticks are converted to their negative position (Up, Left)
     * @param source source entry
     * @return compact entry
     */
    static InputCompactEvent::Entry ConvertEntry(Entry source);

  public:
    /*!
     * @brief Default constructor
     * @return Empty InputDevice
     */
    InputDevice()
      : mDeviceName {},
        mDeviceNameLength(0),
        mDeviceGUID {},
        mDeviceSDL(nullptr),
        mDeviceId(0),
        mDeviceIndex(0),
        mDeviceNbAxes(0),
        mDeviceNbHats(0),
        mDeviceNbButtons(0),
        mConfigurationBits(0),
        mPreviousAxisValues{},
        mConfiguring(false)
    {
    }

    /*!
     * @brief Constructor
     * @param deviceId Device identifier
     * @param deviceIndex Device index
     * @param deviceName Device name
     * @param deviceGUID Device GUID
     * @param deviceNbAxes Number of axises
     * @param deviceNbHats Number of hats
     * @param deviceNbButtons Numper of buttons
     */
    InputDevice(SDL_Joystick* device, SDL_JoystickID deviceId, int deviceIndex, const std::string& deviceName,
                const SDL_JoystickGUID& deviceGUID, int deviceNbAxes, int deviceNbHats, int deviceNbButtons);

    /*!
     * @brief Copy constructor
     * @param source source object to copy
     */
    InputDevice(const InputDevice& source) = default;

    /*!
     * @brief Default assignment operator
     * @param source source object to copy
     * @return Self
     */
    InputDevice& operator =(const InputDevice& source) = default;

    /*
     * Accessors
     */

    std::string Name() const { return std::string(mDeviceName, mDeviceNameLength); }
    std::string NameExtented() const;
    std::string GUID() const { char sguid[64]; SDL_JoystickGetGUIDString(mDeviceGUID, sguid, sizeof(sguid)); return sguid; }
    const char* RawName() const { return mDeviceName; }
    const SDL_JoystickGUID& RawGUID() const { return mDeviceGUID; }
    int Identifier()   const { return mDeviceId; };
    int Index()        const { return mDeviceIndex; };
    int AxeCount()     const { return mDeviceNbAxes; };
    int HatCount()     const { return mDeviceNbHats; };
    int ButtonCount()  const { return mDeviceNbButtons; };
    std::string PowerLevel() const;
    //std::string getSysPowerLevel();

    bool IsKeyboard() const { return mDeviceId == InputEvent::sKeyboardDevice; }
    bool IsPad()      const { return mDeviceId != InputEvent::sKeyboardDevice; }

    /*!
     * @brief Get recorded axis value for a particular axis
     * @param axis Axis index
     * @return Recorded value
     */
    int PreviousAxisValues(int axis) const { return axis < sMaxAxis ? mPreviousAxisValues[axis] : 0; };

    /*!
     * @brief Record Axis value
     * @param axis Axis index
     * @param value Value to record
     */
    void SetPreviousAxisValues(int axis, int value) { if (axis < sMaxAxis) mPreviousAxisValues[axis] = value; };

    /*!
     * @brief Reset all InputEvent
     */
    void ClearAll();

    /*!
     * @brief Load configuration from another config objecvt
     * @param source source object to copy configuration
     */
    void LoadFrom(const InputDevice& source);

    /*
     * Manage entries
     */

    /*!
     * @brief Check is a particular entry is set
     * @param input
     * @return
     */
    bool IsSet(Entry input) const { return ((mConfigurationBits & (1 << (int)input)) != 0); }

    /*!
     * @brief Configure a new entry
     * @param input input entry
     * @param event event to map to input entry
     */
    void Set(Entry input, InputEvent event);

    /*!
     * @brief Remove an entry configuration
     * @param input input entry to unconfigure
     */
    void Unset(Entry input);

    /*
     * Manage the whole configuration
     */

    /*!
     * @brief Load from SDL configuration string
     * @param configuration Configuration string
     */
    bool LoadAutoConfiguration(const std::string& configuration);

    /*!
     * @brief Load the configuration from the given XML
     * @param root Root node to read configuration from
     * @return Number of loaded configuration entry
     */
    int LoadFromXml(pugi::xml_node root);

    /*!
     * @brief Save the configuration to the given XML
     * @param parent Root node to write the configuration to
     */
    void SaveToXml(pugi::xml_node parent) const;

    /*!
     * @brief Set the "in configuration" state on/off
     * @param state true if the Device is in configuration process
     */
    void SetConfiguringState(bool state) { mConfiguring = state; }

    /*!
     * @brief Check if at least one entry has been configured
     * @return True if at least one entry has been configured
     */
    bool IsConfigured() const { return mConfigurationBits != 0 && !mConfiguring; }

    /*!
     * @brief Check if the given event match the entry.
     * This is the most generic matching method
     * @param entry Entry me check for matching
     * @param event Event to check for matching
     * @return True if the event match the given entry
     */
    bool IsMatching(Entry entry, InputEvent event) const;

    /*!
     * @brief Check if the given event match at least a configured entry
     * @param event Event to check
     * @return The first matching entry
     */
    InputDevice::Entry GetMatchedEntry(InputEvent event) const;

    /*!
     * @brief Get the configuration of a particular entry
     * @param entry Entry to get configuration
     * @param result Resulting input configuration
     * @return True if the entry was found and result contains valid data
     */
    bool GetEntryConfiguration(Entry entry, InputEvent& result) const;

    /*
     * Converters
     */

    /*!
     * @brief Convert this input event to its compact form
     * @param event source event
     * @return Compact event
     */
    InputCompactEvent ConvertToCompact(const InputEvent& event);

    /*
     * Helpers
     */

    /*!
     * @brief Convert entry to its string representation
     * @param entry entry to convert
     * @return converted string
     */
    static std::string EntryToString(Entry entry);

    /*!
     * @brief Convert a string entry representation to the matching entry
     * Any unknown string is converted to Entry::None
     * @param entry String entry
     * @return converted entry
     */
    static Entry StringToEntry(const std::string& entry);

    /*!
     * @brief Compare the current device to the given one
     * @param to compare to this device
     * @return True if name, uid, buttons, hat and axis are matching. False otherwise
     */
    bool EqualsTo(const InputDevice& to) const;
};
