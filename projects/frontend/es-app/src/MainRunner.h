//
// Created by bkg2k on 13/11/2019.
//
#pragma once

#include <utils/sdl2/ISynchronousEvent.h>
#include <ApplicationWindow.h>
#include <utils/cplusplus/INoCopy.h>
#include <utils/os/fs/watching/IFileSystemWatcherNotification.h>
#include <utils/os/fs/watching/FileNotifier.h>

class AudioManager;
class SystemManager;

class MainRunner: private INoCopy, private ISynchronousEvent, private IFileSystemWatcherNotification
{
  public:
    //! Pending Exit
    enum class PendingExit
    {
       None,            //!< No Pending exit
       GamelistChanged, //!< At least one gamelist has changed. ES must reload
       ThemeChanged,    //!< Current theme has been modified. ES must reload
       MustExit,        //!< External quit required
       WaitingExit,     //!< Special wait state after a pending exit state has been processed
    };
    
    //! Runner exit state
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

    //! Pending exit
    PendingExit mPendingExit;
    //! Quit request state
    static ExitState sRequestedExitState;
    //! Quit request
    static bool sQuitRequested;

    //! Force reload list requested
    static bool sForceReloadFromDisk;

    /*!
     * @brief Reset last exit state
     */
    static void ResetExitState() { sQuitRequested = false; }

    /*!
     * @brief Reset last exit state
     */
    static void ResetForceReloadState() { sForceReloadFromDisk = false; }

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
     * @param gamelistWatcher FileNotifier to fill in with gamelist path
     * @param forceReloadFromDisk force reloading game list from disk
     * @return True if there is at least one system loaded
     */
    static bool TryToLoadConfiguredSystems(SystemManager& systemManager, FileNotifier& gamelistWatcher, bool forceReloadFromDisk);

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
     * @param systemManager System Manager
     * @param fileNotifier gamelist file notifier
     * @return Exit state
     */
    ExitState MainLoop(ApplicationWindow& window, SystemManager& systemManager, FileNotifier& fileNotifier);

    /*!
     * @brief Create ready flag file to notify all external software that
     * Emulationstation is ready
     */
    static void CreateReadyFlagFile();
    /*!
     * @brief Cleanup ready flag file
     */
    static void DeleteReadyFlagFile();

    /*!
     * @brief Tell if we have to save the gamelist, regarding the exitstate
     * @param state exit state
     * @return True if we have to update the gamelists before exiting
     */
    static bool DoWeHaveToUpdateGamelist(ExitState state);

    /*
     * ISynchronousEvent implementation
     */

    /*!
     * @brief Receive requested exit state event
     * @param event SDL event
     */
    void ReceiveSyncCallback(const SDL_Event& event) override;

    /*
     * IFileSystemWatcherNotification implementation
     */

    void FileSystemWatcherNotification(EventType event, const Path& path, const DateTime& time) override;

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
     * @param forceReloadFromDisk Force reload gamelist from disk
     */
    static void RequestQuit(ExitState requestedState, bool forceReloadFromDisk = false);
};
