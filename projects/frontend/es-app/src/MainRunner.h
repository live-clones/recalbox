//
// Created by bkg2k on 13/11/2019.
//
#pragma once

#include <string>

class MainRunner
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
    static constexpr const char* sReadyFile = "/tmp/emulationstation.ready";
    //! Requested width
    unsigned int mRequestedWidth;
    //! Requested height
    unsigned int mRequestedHeight;

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
     * @brief Initialize all audio
     */
    static void InitializeAudio();

    /*!
     * @brief Play loading jingle (loading.ogg) from theme if available
     */
    static void PlayLoadingSound();

    /*!
     * @brief Try loading system configuration.
     * @return True if there is at least one system loaded
     */
    static bool TryToLoadConfiguredSystems();

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
    static ExitState MainLoop(Window& window);

    /*!
     * @brief Create ready flag file to notify all external software that
     * Emulationstation is ready
     */
    static void CreateReadyFlagFile();
    /*!
     * @brief Cleanup ready flag file
     */
    static void DeleteReadyFlagFile();

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
};
