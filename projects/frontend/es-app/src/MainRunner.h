//
// Created by bkg2k on 13/11/2019.
//
#pragma once

#include <string>
#include <Window.h>
#include <utils/sdl2/ISyncronousEvent.h>
#include <utils/cplusplus/INoCopy.h>

class AudioManager;
class SystemManager;

class MainRunner: private INoCopy, private ISyncronousEvent
{
  public:
    // Runner exit state
    enum class ExitState
    {
      Quit,         //!< Normal quit (usually requested by external software)
      FatalError,   //!< Initialization error or runtime fatal error
      Relaunch,     //!< Relaunch requested!
      NormalReboot, //!< Normal reboot machine requested, save everything
      FastReboot,   //!< Fast reboot machine, save nothing
      Shutdown,     //!< Relaunch machine
      FastShutdown, //!< Relaunch machine, save nothing
    };

  private:
    //! Temporary file used as flag of readyness
    static constexpr const char* sReadyFile = "/tmp/emulationstation.ready";
    //! Temporary file used as quit request
    static constexpr const char* sQuitNow = "/tmp/emulationstation.quitnow";
    //! Requested width
    unsigned int mRequestedWidth;
    //! Requested height
    unsigned int mRequestedHeight;
    //! Quit request state
    static ExitState mRequestedExitState;
    //! Quit request
    static bool mQuitRequested;

    /*!
     * @brief Reset last exit state
     */
    static void ResetExitState() { mQuitRequested = false; }

    /*!
     * @brief Set the system locale
     * @param executablePath Path to current executable
     */
    static void SetLocale(const std::string& executablePath);

    /*!
     * @brief Open the main logger
     */
    static void OpenLogs();

    /*!
     * @brief Check home folder existence
     */
    static void CheckHomeFolder();

    /*!
     * @brief Set architecture in settings
     */
    static void SetArchitecture();

    /*!
     * @brief Play loading jingle (loading.ogg) from theme if available
     */
    static void PlayLoadingSound(AudioManager& audioManager);

    /*!
     * @brief Try loading system configuration.
     * @param systemManager System manager instance
     * @return True if there is at least one system loaded
     */
    static bool TryToLoadConfiguredSystems(SystemManager& systemManager);

    /*!
     * @brief Check if Recalbox has been updated and push a display changelog popup
     */
    static void CheckUpdateMessage(Window& window);

    /*!
     * @brief Initialize input configurations
     * @param window Main window
     */
    static void CheckAndInitializeInput(Window& window);

    /*!
     * @brief Main SDL event loop w/ UI update/refresh
     * @param window Main window
     * @return Exit state
     */
    static ExitState MainLoop(Window& window, SystemManager& systemManager);

    /*!
     * @brief Create ready flag file to notify all external software that
     * Emulationstation is ready
     */
    static void CreateReadyFlagFile();
    /*!
     * @brief Cleanup ready flag file
     */
    static void DeleteReadyFlagFile();

    /*
     * ISynchronousEvent implementation
     */

    /*!
     * @brief Receive requested exit state event
     * @param event SDL event
     */
    void ReceiveSyncCallback(const SDL_Event& event) override;

    /*!
     * @brief Tell if we have to save the gamelist, regarding the exitstate
     * @param state exit state
     * @return True if we have to update the gamelists before exiting
     */
    static bool DoWeHaveToUpdateGamelist(ExitState state);

  public:
    /*!
     * @brief Constructor
     * @param executablePath current executable path
     * @param width Requested width
     * @param height  Requested height
     */
    MainRunner(const std::string& executablePath, unsigned int width, unsigned int height);

    /*!
     * @brief Run the game!
     */
    ExitState Run();

    /*!
     * @brief Request the application to quit using a particular exitstate
     * @param requestedState Requested Exit State
     */
    static void RequestQuit(ExitState requestedState);
};
