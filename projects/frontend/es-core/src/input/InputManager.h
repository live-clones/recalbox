#pragma once

#include <SDL.h>
#include <vector>
#include <map>
#include <string>
#include <input/InputDevice.h>
#include <utils/os/fs/Path.h>

class Window;

class InputManager
{
  private:
    //! Device list
    typedef std::vector<InputDevice*> InputDeviceList;

    //! Joystick deadzone, in the 0-32767 range
    static constexpr int sJoystickDeadZone = 23000;

    //! SDL Identifier to Joystick structures
    std::map<SDL_JoystickID, SDL_Joystick*> mJoysticks;
    //! SDL Identifier to device configurations
    std::map<SDL_JoystickID, InputDevice*> mDevices;
    //! Default Keyboard
    InputDevice mKeyboard;

    /*!
     * @brief Default constructor
     */
    InputManager();

    /*!
     * @brief Load default keyboard configuration
     * Event if the user deletes es_input.cfg, he's still able to reconfigure using a keyboard
     */
    void LoadDefaultKeyboardConfiguration();

    /*!
     * @brief Get the initialization state
     * @return True if the manager is initialized
     */
    bool IsInitialized() const { return !mDevices.empty(); }

    /*!
     * @brief Get the GUID string of an SDL joystik
     * @param joyystick SDL Joystick handle
     * @return GUID string
     */
    static std::string DeviceGUIDString(SDL_Joystick* joystick);

    /*!
     * @brief Clear all configurations
     */
    void ClearAllConfigurations();

    /*!
     * @brief Load all joystick and load configurations
     */
    void LoadAllJoysticksConfiguration();

    /*!
     * @brief Load joystick configuration (by index)
     * @param index Joystick index from to 0 to available joysticks-1
     */
    void LoadJoystickConfiguration(int index);

    /*!
     * @brief Process an Axis SDL event and generate an InputCompactEvent accordingly
     * @param axis SDL event
     * @return InputCompactEvent filled with event information
     */
    InputCompactEvent ManageAxisEvent(const SDL_JoyAxisEvent& axis);

    /*!
     * @brief Process a button SDL event and generate an InputCompactEvent accordingly
     * @param button SDL event
     * @return InputCompactEvent filled with event information
     */
    InputCompactEvent ManageButtonEvent(const SDL_JoyButtonEvent& button);

    /*!
     * @brief Process a hat SDL event and generate an InputCompactEvent accordingly
     * @param hat SDL event
     * @return InputCompactEvent filled with event information
     */
    InputCompactEvent ManageHatEvent(const SDL_JoyHatEvent& hat);

    /*!
     * @brief Process a keyboard SDL event and generate an InputCompactEvent accordingly
     * @param hat SDL event
     * @return InputCompactEvent filled with event information
     */
    InputCompactEvent ManageKeyEvent(const SDL_KeyboardEvent& key, bool down);

    /*!
     * @brief Lookup Xml configuration for a particular device, lookinf for matching
     * guid and/or name
     * @param device Device to look for configuration
     * @return
     */
    static bool LookupDeviceXmlConfiguration(InputDevice& device);

    /*!
     * @brief Fill the given list with configured devices
     * @param list
     */
    void FillConfiguredDevicelist(InputDeviceList& list);

    /*!
     * @brief Lookup a device matching both guid and name in the given list
     * @param list List to look in
     * @param guid Guid to look for
     * @param name Name to look for
     * @return Device instance if found, nullptr otherwise
     */
    static InputDevice* LookupDevice(InputDeviceList& list, const std::string& guid, const std::string& name);

    /*!
     * @brief Lookup a device matching name in the given list
     * @param list List to look in
     * @param name Name to look for
     * @return Device instance if found, nullptr otherwise
     */
    static InputDevice* LookupDevice(InputDeviceList& list, const std::string& name);

    /*!
     * @brief Lookup a device in the given list
     * @param list List to look in
     * @param name Name to look for
     * @return Device instance if found, nullptr otherwise
     */
    static InputDevice* LookupDevice(InputDeviceList& list);

  public:
    /*!
     * @brief Default destructor
     */
    virtual ~InputManager();

    /*!
     * @brief Instance
     * @return Singleton instance
     */
    static InputManager& Instance();

    /*!
     * @brief Initialize the InputManager
     */
    void Initialize();

    /*!
     * Finalize the input manager and free all resources
     */
    void Finalize();

    /*!
     * Get number of initialized devices
     */
    int DeviceCount() const { return mJoysticks.size(); }

    /*!
     * @brief Parse an SDL event and generate an InputCompactEvent accordingly
     * @param ev SDL event
     * @param resultEvent InputCompactEvent to fill with event information
     * @return True if the resultEvent is valid, false otherwise
     */
    InputCompactEvent ManageSDLEvent(const SDL_Event& ev);

    /*!
     * @brief Get number of configured devices, either manually or from Xml configuration file
     * @return Configured device count
     */
    int ConfiguredDeviceCount();

    /*!
     * @brief Get configuration path
     * @return Configuration path
     */
    static Path ConfigurationPath();

    /*!
     * @brief Write device configuration to Xml configuration file
     * @param device
     */
    static void WriteDeviceXmlConfiguration(InputDevice& device);

    //int getButtonCountByDevice(SDL_JoystickID identifier) const { return identifier == InputEvent::sKeyboardDevice ? mKeyboard.ButtonCount() : SDL_JoystickNumButtons(mJoysticks.at(identifier)); }
    //int getAxisCountByDevice(SDL_JoystickID identifier) const { return identifier == InputEvent::sKeyboardDevice ? 0 : SDL_JoystickNumButtons(mJoysticks.at(identifier)); }

    /*!
     * @brief Get device by SDL Indetifier
     * @param deviceId Device identifier
     * @return Device configuration
     */
    InputDevice* GetDeviceConfiguration(int deviceId);

    /*!
     * @brief Generate all player configurations into a single string
     * ready to be used in the configgen
     * @return Configuration string
     */
    std::string GenerateConfiggenConfiguration();

    static void LogRawEvent(const InputEvent& event);

    static void LogCompactEvent(const InputCompactEvent& event);
};
