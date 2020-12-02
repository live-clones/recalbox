#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <input/InputDevice.h>
#include <input/OrderedDevices.h>
#include <utils/os/fs/Path.h>
#include <utils/storage/HashMap.h>
#include <utils/storage/Array.h>

class WindowManager;

class InputManager
{
  private:
    //! Device list
    typedef Array<InputDevice> InputDeviceList;

    //! Joystick deadzone, in the 0-32767 range
    static constexpr int sJoystickDeadZone = 23000;

    //! Index to SDL Identifiers
    SDL_JoystickID mIndexToId[Input::sMaxInputDevices];
    //! SDL Identifier to Joystick structures
    HashMap<SDL_JoystickID, SDL_Joystick*> mIdToSdlJoysticks;
    //! SDL Identifier to device configurations
    HashMap<SDL_JoystickID, InputDevice> mIdToDevices;
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
    bool IsInitialized() const { return !mIdToDevices.empty(); }

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

    std::vector<InputDevice> BuildCurrentDeviceList();

    /*!
     * @brief Load all joystick and load configurations
     */
    void LoadAllJoysticksConfiguration(std::vector<InputDevice> previous, WindowManager* window, bool padplugged);

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
    static bool LookupDevice(InputDeviceList& list, const std::string& guid, const std::string& name, InputDevice& output);

    /*!
     * @brief Lookup a device matching name in the given list
     * @param list List to look in
     * @param name Name to look for
     * @return Device instance if found, nullptr otherwise
     */
    static bool LookupDevice(InputDeviceList& list, const std::string& name, InputDevice& output);

    /*!
     * @brief Lookup a device in the given list
     * @param list List to look in
     * @param name Name to look for
     * @return Device instance if found, nullptr otherwise
     */
    static bool LookupDevice(InputDeviceList& list, InputDevice& output);

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
     * @param window Main window
     */
    void Initialize(WindowManager* window, bool padplugged = false);

    /*!
     * Finalize the input manager and free all resources
     */
    void Finalize();

    static void IntitializeSDL2JoystickSystem();

    static void FinalizeSDL2JoystickSystem();

    /*!
     * Get number of initialized devices
     */
    int DeviceCount() const { return mIdToDevices.size(); }

    /*!
     * @brief Parse an SDL event and generate an InputCompactEvent accordingly
     * @param ev SDL event
     * @param resultEvent InputCompactEvent to fill with event information
     * @return True if the resultEvent is valid, false otherwise
     */
    InputCompactEvent ManageSDLEvent(WindowManager* window, const SDL_Event& ev);

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

    /*!
     * @brief Get device by SDL Identifier
     * @param deviceId Device identifier
     * @return Device configuration
     */
    InputDevice& GetDeviceConfigurationFromId(SDL_JoystickID deviceId);

    /*!
     * @brief Get device by SDL Identifier
     * @param deviceId Device identifier
     * @return Device configuration
     */
    InputDevice& GetDeviceConfigurationFromIndex(int index) { return GetDeviceConfigurationFromId(mIndexToId[index]); }

    /*!
     * @brief Generate an ordered device list in function of player devices configuratons
     * @return OrderedDevice object
     */
    OrderedDevices GenerateConfiguration();

    /*!
     * @brief Generate all player configurations into a single string
     * ready to be used in the configgen
     * @return Configuration string
     */
    static std::string GenerateConfiggenConfiguration(const OrderedDevices& devices);

    /*!
     * @brief Lookup Xml configuration for a particular device, lookinf for matching
     * guid and/or name
     * @param device Device to look for configuration
     * @return
     */
    static bool LookupDeviceXmlConfiguration(InputDevice& device);

    static void LogRawEvent(const InputEvent& event);

    static void LogCompactEvent(const InputCompactEvent& event);
};
