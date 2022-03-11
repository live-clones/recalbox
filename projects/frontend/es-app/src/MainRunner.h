//
// Created by bkg2k on 13/11/2019.
//
#pragma once

#include <utils/sdl2/ISynchronousEvent.h>
#include <ApplicationWindow.h>
#include <utils/cplusplus/INoCopy.h>
#include <utils/os/fs/watching/IFileSystemWatcherNotification.h>
#include <utils/os/fs/watching/FileNotifier.h>
#include <usernotifications/NotificationManager.h>
#include <guis/GuiWaitLongExecution.h>
#include <utils/storage/Queue.h>
#include <systems/GameRunner.h>

class AudioManager;
class SystemManager;

//! Special operations
enum class HardwareTriggeredSpecialOperations
{
  Suspend,  //!< The hardware require a Suspend operation
  Resume,   //!< The hardware just woken up
  PowerOff, //!< The hardware require a Power-off
};

class MainRunner
  : private INoCopy
  , private ISynchronousEvent
  , private IFileSystemWatcherNotification
  , public IHardwareNotifications
  , private ILongExecution<HardwareTriggeredSpecialOperations, bool>
  , private IRomFolderChangeNotification
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
      Quit,             //!< Normal quit (usually requested by external software)
      FatalError,       //!< Initialization error or runtime fatal error
      Relaunch,         //!< Relaunch requested!
      RelaunchNoUpdate, //!< Relaunch requested! No gamelist update
      NormalReboot,     //!< Normal reboot machine requested, save everything
      FastReboot,       //!< Fast reboot machine, save nothing
      Shutdown,         //!< Relaunch machine
      FastShutdown,     //!< Relaunch machine, save nothing
    };

    //! Temporary file used as flag of readyness
    static constexpr const char* sReadyFile = "/tmp/externalnotifications/emulationstation.ready";
    //! Temporary file used as quit request
    static constexpr const char* sQuitNow = "/tmp/externalnotifications/emulationstation.quitnow";
    //! Upgrade file flag. Only available once in /tmp after a successful update
    static constexpr const char* sUpgradeFileFlag = "/tmp/upgraded";
    static constexpr const char* sUpgradeFailedFlag = "/tmp/upgradefailed";

  private:
    //! Requested width
    unsigned int mRequestedWidth;
    //! Requested height
    unsigned int mRequestedHeight;
    //! Requested window mode
    bool mRequestWindowed;

    //! Pending exit
    PendingExit mPendingExit;

    //! Run count
    int mRunCount;

    //! Quit request state
    static ExitState sRequestedExitState;
    //! Quit request
    static bool sQuitRequested;
    //! Force reload list requested
    static bool sForceReloadFromDisk;

    //! Recalbox configuration
    RecalboxConf mConfiguration;
    //! Crt configuration
    CrtConf mCrtConfiguration;

    //! Nofitication manager
    NotificationManager mNotificationManager;

    //! Window reference
    ApplicationWindow* mApplicationWindow;

    //! Known added devices
    HashSet<std::string> mAddedDevices;
    //! Known empty added devices
    HashSet<std::string> mAddedEmptyDevices;
    //! Known removed devices
    HashSet<std::string> mRemovedDevices;

    /*!
     * @brief Reset last exit state
     */
    static void ResetExitState() { sQuitRequested = false; }

    /*!
     * @brief Reset last exit state
     */
    static void ResetForceReloadState() { sForceReloadFromDisk = false; }

    /*!
     * @brief Display Intro
     */
    void Intro(bool debug);

    /*!
     * @brief Check home folder existence
     */
    static void CheckHomeFolder();

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
     * @param window Main window
     */
    static void CheckUpdateMessage(WindowManager& window);

    /*!
     * @brief Check if Recalbox upgrade has failed push an error popup
     * @param window Main window
    */
    static void CheckUpdateFailed(WindowManager& window);

    /*!
     * @brief Check if this is the first launch and run a wizard if required
     * @param window Main window
     */
    static void CheckFirstTimeWizard(WindowManager& window);

    /*!
     * @brief Initialize input configurations
     * @param window Main window
     */
    static void CheckAndInitializeInput(WindowManager& window);

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
    void ReceiveSyncCallback(const SDL_Event& event) final;

    /*
     * IFileSystemWatcherNotification implementation
     */

    void FileSystemWatcherNotification(EventType event, const Path& path, const DateTime& time) final;

    /*
     * IHardwareEvents implementation
     */

    bool IsApplicationRunning() final { return !GameRunner::IsGameRunning(); }

    /*!
     * @brief Headphone has been pluggen in
     * @param board current board
     */
    void HeadphonePluggedIn(BoardType board) final;

    /*!
     * @brief Headphone has been unplugged
     * @param board current board
     */
    void HeadphoneUnplugged(BoardType board) final;

    /*!
     * @brief Decrease volume
     * @param board current board
     */
    void VolumeDecrease(BoardType board, float percent) final;

    /*!
     * @brief Increase volume
     * @param board current board
     */
    void VolumeIncrease(BoardType board, float percent) final;

    /*!
     * @brief Suspend will occur in a short delay
     * @param board current board
     * @param delayInMs delay in ms before suspend will occurs
     */
    void PowerButtonPressed(BoardType board, int delayInMs) final;

    /*!
     * @brief We have been resumed from suspend mode
     * @param board current board
     */
    void Resume(BoardType board) final;

    /*
     * ILongExecution implementation
     */

    /*!
     * @brief Dummy execution of sleeps, allowing UI to draw special operation wait-windows
     * @param from Source window
     * @param parameter Operation
     * @return Not used
     */
    bool Execute(GuiWaitLongExecution<HardwareTriggeredSpecialOperations, bool>& from,
                 const HardwareTriggeredSpecialOperations& parameter) override;

    /*!
     * @brief Called when special wait-window close so that we can execute the required operation
     * @param parameter Operation required
     * @param result Not used
     */
    void Completed(const HardwareTriggeredSpecialOperations& parameter, const bool& result) override;

  public:
    /*!
     * @brief Constructor
     * @param executablePath current executable path
     * @param width Requested width
     * @param height Requested height
     * @param windowed No fullscreen
     * @param runCount Number of time the MainRunner has been run
     */
    MainRunner(const std::string& executablePath, unsigned int width, unsigned int height, bool windowed, int runCount, char** environment, bool debug);

    //! Destructor
    virtual ~MainRunner();

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

    /*!
     * @brief Set the system locale
     * @param executablePath Path to current executable
     */
    static void SetLocale(const std::string& executablePath);

    /*
     * RomFolderChangeNotification implementaton
     */

    /*!
     * @brief Notify a new rompath has been added
     * @param root rompath added
     */
    void RomPathAdded(const DeviceMount& root) override;

    /*!
     * @brief Notify an existing reompath has been removed
     * @param root rompath removed
     */
    void RomPathRemoved(const DeviceMount& root) override;

    /*!
     * @brief Notify a device has been added with no valid rom path found
     * @param deviceRoot Device mount point
     */
    void NoRomPathFound(const DeviceMount& deviceRoot) override;

    /*!
     * @brief Install CRT features
     */
    void InstallCRTFeatures();
};
