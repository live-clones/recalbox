#pragma once

#include <systems/SystemData.h>
#include <systems/SystemDescriptor.h>
#include <systems/EmulatorManager.h>
#include <systems/MountMonitor.h>
#include <utils/os/system/IThreadPoolWorkerInterface.h>
#include <RootFolders.h>
#include <utils/cplusplus/INoCopy.h>
#include <views/IProgressInterface.h>
#include <utils/os/fs/watching/FileNotifier.h>
#include <utils/os/system/Mutex.h>
#include "IRomFolderChangeNotification.h"

class SystemManager :
  private INoCopy, // No copy allowed
  public IThreadPoolWorkerInterface<SystemDescriptor, SystemData*>, // Multi-threaded system loading
  public IThreadPoolWorkerInterface<SystemData*, bool>, // Multi-threaded system unloading
  public IMountMonitorNotifications
{
  public:
    //! Convenient alias for System list
    typedef std::vector<SystemData*> SystemList;

    //! Favorite system internal name
    static constexpr const char* sFavoriteSystemShortName = "favorites";
    //! Multiplayer system internal name
    static constexpr const char* sMultiplayerSystemShortName = "multiplayer";
    //! Last Played system internal name
    static constexpr const char* sLastPlayedSystemShortName = "lastplayed";
    //! All games system internal name
    static constexpr const char* sAllGamesSystemShortName = "allgames";
    //! Multiplayer system internal name
    static constexpr const char* sMultiplayerSystemFullName = "Multi Players";
    //! Last Played system internal name
    static constexpr const char* sLastPlayedSystemFullName = "Last Played";
    //! All games system internal name
    static constexpr const char* sAllGamesSystemFullName = "All Games";

  private:
    //! Rom source folder to read/write (false) / read-only (true) state
    typedef HashMap<std::string, bool> RomSources;

    //! File path to system weight file for fast loading/saving
    static constexpr const char* sWeightFilePath = "/recalbox/share/system/.emulationstation/.weights";

    //! Root tag
    static constexpr const char* sRootTag = "%ROOT%";

    //! Writable share roms
    static constexpr const char* sShareRomRoot = "/recalbox/share/roms";
    //! Read-only share roms
    static constexpr const char* sShareInitRomRoot = "/recalbox/share_init/roms";
    //! Update share roms
    static constexpr const char* sShareUpgradeRomRoot = "/recalbox/share_upgrade/roms";

    //! Special process of ports
    enum class PortTypes
    {
        None,          //!< Not a port / Do nothing
        ShareInitOnly, //!< From share_init only
        ShareOnly,     //!< From share only
    };

    enum class RomStructure
    {
      None,   //!< No rom structure at all
      Empty,  //!< Rom structure exists but empty
      Filled, //!< Rom structure with roms inside
    };

    //! Mount points
    Path::PathList mMountPoints;
    //! Mount point monitoring
    MountMonitor mMountPointMonitoring;
    //! Emulator manager
    EmulatorManager mEmulatorManager;
    //! Emulator manager guard
    Mutex mEmulatorGuard;

    //! Visible system, including virtual system (Arcade)
    std::vector<SystemData*> mVisibleSystemVector;
    //! Original of Visible system (copy)
    std::vector<SystemData*> mOriginalVisibleSystemVector;
    //! Hidden system, just here to hold their own children
    std::vector<SystemData*> mHiddenSystemVector;
    //! ALL systems, visible and hidden
    std::vector<SystemData*> mAllSystemVector;

    //! All declared system names
    std::vector<std::string> mAllDeclaredSystemShortNames;
    //! All declared system rom path
    Path::PathList mAllDeclaredSystemRomPathes;
    //! All declared system rom path
    HashSet<std::string> mAllDeclaredSystemExtensionSet;

    //! Progress interface called when loading/unloading
    IProgressInterface* mProgressInterface;
    //! Rom path change notifications interface
    IRomFolderChangeNotification& mRomFolderChangeNotificationInterface;

    //! The system manager is instructed to reload game list from disk, not only from gamelist.xml
    bool mForceReload;

    bool mPauseGamelistWatcher = false;

    /*!
     * @brief Check if there are at least one file from the given path whose extension is in the given set
     * @param path Path (folder) to check files in
     * @param extensionSet Extension list to check files against
     * @return True if a file with a matchng extension has been found, false otherwise
     */
    static bool HasFileWithExt(const Path& path, HashSet<std::string>& extensionSet);

    /*!
     * @brief Check if the root folder contains at least a valid rom path and return it
     * @param root Root mount point
     * @param romPath Output rom path found
     * @return True if a rom path has been found and stored in romPath. False otherwise
     */
    RomStructure CheckMountPoint(const DeviceMount& root, Path& romPath);

    /*!
     * @brief Initialize initial mount points
     */
    void InitializeMountPoints();

    /*!
     * @brief Create and add the favorite meta-system
     * @param systemList All system from which to fetch favorite games
     * @return True if the favorite system has been added
     */
    bool AddFavoriteSystem();

    /*!
     * @brief Add Arcade meta-system
     * @return True if the arcade system has been added
     */
    bool AddArcadeMetaSystem();

    /*!
     * @brief Add ports meta-system
     * @return True if the ports system has been added
     */
    bool AddPorts();

    /*!
     * @brief Add manually filtered meta system. Get item from Visible system vector and filter
     * games using the provided filter
     * @param filter Filter to apply
     * @param comparer Comparer to sort items. If null, no sorting is applied
     * @param identifier System identifier (short name)
     * @param fullname System full name
     * @return True if the system has been added
     */
    bool AddManuallyFilteredMetasystem(IFilter* filter, FileData::Comparer comparer, const std::string& identifier,
                                       const std::string& fullname, SystemData::Properties properties,
                                       FileSorts::Sorts fixedSort = FileSorts::Sorts::FileNameAscending);

    /*!
     * @brief Add All-games meta system (all games from all visible systems)
     * @return Alays true
     */
    bool AddAllGamesMetaSystem();

    /*!
     * @brief Add all games with min or max players > 1 (from all visible systems)
     * @return Always true
     */
    bool AddMultiplayerMetaSystems();

    /*!
     * @brief Add last-played games from all systems (from all visible systems)
     * @return Always true
     */
    bool AddLastPlayedMetaSystem();

    /*!
     * @brief Try to add any déclared genre virtual system, including all game of the required genre
     * @return Always true
     */
    bool AddGenresMetaSystem();

    /*!
     * @brief All all special collections
     * @param True if the current board is a portable system
     * @return Always true
     */
    bool AddSpecialCollectionsMetaSystems(bool portable);

    /*!
     * @brief All lightgun system
     * @return Always true
     */
    bool AddLightGunMetaSystem();

    /*!
     * @brief Get valid rom source for the given system descriptor
     * @param systemDescriptor System descriptor
     * @return Rom source folders and associated RW/RO states
     */
    RomSources GetRomSource(const SystemDescriptor& systemDescriptor, PortTypes port);

    /*!
     * @brief Create regular system from a SystemDescriptor object
     * @param systemDescriptor SystemDescriptor object
     * @param forceLoad Force reloading list from disk and not only from gamelist.xml
     * @return New system
     */
    SystemData* CreateRegularSystem(const SystemDescriptor& systemDescriptor, bool forceLoad);

    /*!
     * @brief Create Favorite system using favorites available in systems from a list
     * @param name Target system short name
     * @param fullName Target system full name
     * @param themeFolder Theme folder name
     * @param systems System list from which to fetch favorite games
     * @return New favorite games
     */
    SystemData* CreateFavoriteSystem(const std::string& name, const std::string& fullName,
                                     const std::string& themeFolder, const std::vector<SystemData*>& systems);

    /*!
     * @brief Create meta-system aggregating games from multiple systems
     * @param name Target system short name
     * @param fullName Target system fullname
     * @param themeFolder Theme folder name
     * @param systems System to fetch games to aggregate into a single list
     * @param properties System properties
     * @param doppelganger Map to FileData
     * @return New meta-system
     */
    SystemData* CreateMetaSystem(const std::string& name, const std::string& fullName,
                                 const std::string& themeFolder, const std::vector<SystemData*>& systems,
                                 SystemData::Properties properties, FileData::StringMap& doppelganger,
                                 FileSorts::Sorts fixedSort = FileSorts::Sorts::FileNameAscending);

    /*!
     * @brief Create meta-system aggregating games from multiple systems
     * @param name Target system short name
     * @param fullName Target system fullname
     * @param themeFolder Theme folder name
     * @param games Games to add
     * @param properties System properties
     * @param doppelganger Map to FileData
     * @return New meta-system
     */
    SystemData* CreateMetaSystem(const std::string& name, const std::string& fullName,
                                 const std::string& themeFolder, const FileData::List& games,
                                 SystemData::Properties properties, FileData::StringMap& doppelganger,
                                 FileSorts::Sorts fixedSort = FileSorts::Sorts::FileNameAscending);

    /*!
     * @brief Write exemple configuration file when no configuration file are available
     * @param path Configuration file path
     */
    static void GenerateExampleConfigurationFile(const Path& path);

    /*
     * ThreadPoolWorkingInterface implementation
     */

    /*!
     * @brief Load and parse a single gamelist, then return a complete fulfilled system
     * @param systemDescriptor System object from es_systems.cfg
     * @return New SystemData object or nullptr
     */
    SystemData* ThreadPoolRunJob(SystemDescriptor& systemDescriptor) override;

    /*!
     * @brief Update a single gamelist from the metadata
     * @param feed System from which to update the gamelist
     * @return Always true
     */
    bool ThreadPoolRunJob(SystemData*& feed) override;

    void ThreadPoolTick(int completed, int total) override;

	/*!
	 * @brief Sort systems based on conf option
	 * @param systems Systemlist to be sorted
	 * @param originalSystems original Systemlist
	 */
    static void SystemSorting(std::vector<SystemData*>& systems, const std::vector<SystemData *>& originalSystems);

    /*
     * IMountMonitorNotifications implementation
     */

    /*!
     * @brief Notify a new device partition has been mounted
     * @param root Mount Point
     */
    void NotifyDeviceMount(const DeviceMount& root) override;

    /*!
     * @brief Notify a new device partition has been unmounted
     * @param root Mount Point
     */
    void NotifyDeviceUnmount(const DeviceMount& root) override;

  public:
    /*!
     * @brief constructor
     */
    explicit SystemManager(IRomFolderChangeNotification& interface)
      : mMountPointMonitoring(this)
      , mProgressInterface(nullptr)
      , mRomFolderChangeNotificationInterface(interface)
      , mForceReload(false)
    {
    }

    //! Destructor
    ~SystemManager() override = default;

    /*!
     * @brief Set the progress interface
     * @param interface
     */
    void SetProgressInterface(IProgressInterface* interface) { mProgressInterface = interface; }

    /*!
     * @brief Get favorite system
     * @return Favorite system of nullptr if there is no favorite system
     */
    SystemData* FavoriteSystem();

    /*!
     * @brief Get system by (short) name
     * @param name Short name
     * @return System instance of nullptr if not found
     */
    SystemData* SystemByName(const std::string& name);

    /*!
     * @brief Get the first non-empty system
     * @return First non empty system or null if all systems are empty
     */
    SystemData* FirstNonEmptySystem();

    /*!
     * @brief Get visible system index by name
     * @param name System name
     * @return System index or -1 if not found
     */
    int getVisibleSystemIndex(const std::string& name);

    /*!
     * @brief Update gamelist that contain modified game metadata
     */
    void UpdateAllSystems();

    /*!
     * @brief Delete all systems and all sub-objects
     * @param updateGamelists
     */
    void DeleteAllSystems(bool updateGamelists);

    /*!
     * @brief Load the system config file at getConfigPath(). Returns true if no errors were encountered. An example will be written if the file doesn't exist.
     * @param gamelistWatcher FileNotifier to fill in with gamelist path
     * @param ForeReload force reloading from disk
     * @param portableSystem true if the current board is a portable system and does not need lightgun
     */
    bool LoadSystemConfigurations(FileNotifier& gamelistWatcher, bool ForeReload, bool portableSystem);

    /*!
     * @brief Get All system list, visibles + hidden
     * @return System list
     */
    const SystemList& GetAllSystemList() { return mAllSystemVector; }
    /*!
     * @brief Get visible-only system list
     * @return System list
     */
    const SystemList& GetVisibleSystemList() { return mVisibleSystemVector; }
    /*!
     * @brief Get Hidden-only system list
     * @return System list
     */
    const SystemList& GetHiddenSystemList() { return mHiddenSystemVector; }

    /*!
     * @brief Get all system (from es_systems.cfg) short names
     * @return system name list
     */
    const Strings::Vector& GetDeclaredSystemShortNames() { return mAllDeclaredSystemShortNames; }

    /*!
     * @brief Update last-played system with the given game
     * @param game Last played game
     */
    void UpdateLastPlayedSystem(FileData& game);

    /*!
     * @brief Get Last Played system
     * @return Last played system or nullptr if it does not exist
     */
    SystemData* LastPlayedSystem()
    {
      int index = getVisibleSystemIndex(sLastPlayedSystemShortName);
      if (index < 0) return nullptr;
      return mVisibleSystemVector[index];
    }

    /*!
     * @brief Get next system to the given system
     * @param to Reference system
     * @return Next system
     */
    SystemData* NextVisible(SystemData* to) const
    {
      int size = (int)mVisibleSystemVector.size();
      for(int i = size; --i>=0; )
        if (mVisibleSystemVector[i] == to)
          return mVisibleSystemVector[(++i) % size];
      return nullptr;
    }

    /*!
     * @brief Get previous system to the given system
     * @param to Reference system
     * @return Previous system
     */
    SystemData* PreviousVisible(SystemData* to) const
    {
      int size = (int)mVisibleSystemVector.size();
      for(int i = size; --i>=0; )
        if (mVisibleSystemVector[i] == to)
          return mVisibleSystemVector[(--i + size) % size];
      return nullptr;
    }

    //! Get emulator manager
    const EmulatorManager& Emulators() const { return mEmulatorManager; }

    /*!
     * @brief Fasqt Search result Quick sort
     * @param items Item to sort
     * @param low Range low index
     * @param high Range high index
     */
    static void SearchResultQuickSortAscending(FolderData::ResultList& items, int low, int high);

    /*!
     * @brief Search games from text
     * @param text Text to search for
     * @param maxpersystem Maximum results per system
     * @param maxglobal Maximum results
     * @return Sorted game found list
     */
    FileData::List searchTextInGames(FolderData::FastSearchContext context, const std::string& text, int maxpersystem, int maxglobal, SystemData* systemData);

    //! Auto-scrape game image
    static void AutoScrape(SystemData* pData);

    //! Sort or resort system list
    void SystemSorting();

    /*!
     * @brief Create an empty rom structure in
     * @param device Source device
     * @return True if th  path have been created successfully, false otherwise
     */
    static bool CreateRomFoldersIn(const DeviceMount& device);

    void PauseWatchingGamelistXml(bool pause);

    bool IsPauseGamelistWatcher() { return mPauseGamelistWatcher; }
};
