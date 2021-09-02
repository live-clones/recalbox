#pragma once

#include <SDL2/SDL.h>
#include <string>
#include <input/InputDevice.h>
#include <input/OrderedDevices.h>
#include <utils/os/fs/Path.h>
#include <utils/storage/HashMap.h>
#include <utils/storage/Array.h>
#include "IInputChange.h"

class WindowManager;
class InputMapper;

class InputManager
{
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
     * @brief Refresh configurations. Reload all joysticks
     * @param window Main window
     */
    void Refresh(WindowManager* window, bool padplugged);

    /*!
     * Get number of initialized devices
     */
    int DeviceCount() const { return (int)mIdToDevices.size(); }

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
    InputDevice& GetDeviceConfigurationFromIndex(int index) { return GetDeviceConfigurationFromId(mIndexToId[index]); }

    /*!
     * @brief Generate an ordered device list in function of player devices configuratons
     * @return OrderedDevice object
     */
    OrderedDevices GetMappedDeviceList(const InputMapper& mapper);

    /*!
     * @brief Generate all player configurations into a single string
     * ready to be used in the configgen
     * @return Configuration string
     */
    std::string GetMappedDeviceListConfiguration(const InputMapper& mapper);

    /*!
     * @brief Lookup Xml configuration for a particular device, lookinf for matching
     * guid and/or name
     * @param device Device to look for configuration
     * @return
     */
    static bool LookupDeviceXmlConfiguration(InputDevice& device);

    /*!
     * @brief Log a detailled report of the raw input event
     * @param event Input event
     */
    static void LogRawEvent(const InputEvent& event);

    /*!
     * @brief Log a detailled report of the input compact event
     * @param event compact event
     */
    static void LogCompactEvent(const InputCompactEvent& event);

    /*!
     * @brief Add a new interface to call when a pad is added or removed
     * @param interface New interface
     */
    void AddNotificationInterface(IInputChange* interface);

    /*!
     * @brief Remove a notofocation interface
     * Does nothing if the given interface has not been previously added
     * @param interface Interface to remove
     */
    void RemoveNotificationInterface(IInputChange* interface);

    /*!
     * @brief Initialize SDL's joysticks
     */
    static void IntitializeSDL2JoystickSystem();

    /*!
     * @brief Finalize SDL's joysticks
     */
    static void FinalizeSDL2JoystickSystem();

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
    //! Default Mousse
    InputDevice mMousse;

    //! Notification interfaces
    Array<IInputChange*> mNotificationInterfaces;

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
     * @param keyboard SDL event
     * @return InputCompactEvent filled with event information
     */
    InputCompactEvent ManageKeyEvent(const SDL_KeyboardEvent& key, bool down);

    /*!
     * @brief Process a mousse SDL event and generate an InputCompactEvent accordingly
     * @param mousse SDL event
     * @return InputCompactEvent filled with event information
     */
    InputCompactEvent ManageMousseButtonEvent(const SDL_MouseButtonEvent& button, bool down);

    /*!
     * @brief Get device by SDL Identifier
     * @param deviceId Device identifier
     * @return Device configuration
     */
    InputDevice& GetDeviceConfigurationFromId(SDL_JoystickID deviceId);
};
