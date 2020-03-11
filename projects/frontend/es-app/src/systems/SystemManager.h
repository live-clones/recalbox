#pragma once

#include "SystemData.h"
#include "SystemDescriptor.h"
#include "EmulatorManager.h"
#include <utils/os/system/IThreadPoolWorkerInterface.h>
#include <RootFolders.h>
#include <utils/Xml.h>
#include <utils/cplusplus/INoCopy.h>
#include <views/IProgressInterface.h>
#include <utils/os/fs/watching/FileNotifier.h>

class SystemManager :
  private INoCopy, // No copy allowed
  public IThreadPoolWorkerInterface<SystemDescriptor, SystemData*>, // Multi-threaded system loading
  public IThreadPoolWorkerInterface<SystemData*, bool> // Multi-threaded system unloading
{
  public:
    //! Convenient alias for system collision map
    typedef std::map<std::string, int> XmlNodeCollisionMap;
    //! Convenient alias for XML node list
    typedef std::vector<XmlNode> XmlNodeList;
    //! Convenient alias for System list
    typedef std::vector<SystemData*> SystemList;

    //! Multiplayer system internal name
    static constexpr const char* sFavoriteSystemShortName = "favorites";
    //! Multiplayer system internal name
    static constexpr const char* sMultiplayerSystemShortName = "multiplayers";
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
    //! File path to system weight file for fast loading/saving
    static constexpr const char* sWeightFilePath = "/recalbox/share/system/.emulationstation/.weights";

    // Emulator manager
    EmulatorManager mEmulatorManager;

    //! Visible system, including virtual system (Arcade)
    std::vector<SystemData*> mVisibleSystemVector;
    //! Hidden system, just here to hold their own children
    std::vector<SystemData*> mHiddenSystemVector;
    //! ALL systems, visible and hidden
    std::vector<SystemData*> mAllSystemVector;

    //! All declared system names
    std::vector<std::string> mAllDeclaredSystemShortNames;

    //! Progress interface called when loading/unloading
    IProgressInterface* mProgressInterface;

    //! The system manager is instructed to reload game list from disk, not only from gamelist.xml
    bool mForceReload;

    /*!
     * Run though the system list and store system nodes into the given store.
     * If a system already exists in the store, the new system node is ignored
     * @param collisionMap Collision map to keep track of nodes in the store
     * @param nodeStore Node store to fill with unique system nodes
     * @param document Source document
     * @return true if the system list contains one or more system. False if the systemList node does not exist or is empty.
     */
    static bool LoadSystemXMLNodes(XmlNodeCollisionMap& collisionMap, XmlNodeList& nodeStore, const XmlDocument& document);

    /*!
     * Load systemList file into the given XML Document, then parse and store systems into the given node store
     * Perform all file/xml checks.
     * @param document Xml Document to store content to
     * @param collisionMap Collision map to keep track of nodes in the store
     * @param nodeStore System node store
     * @return true if the operation is successful and at least one system has been processed. False otherwise
     */
    static bool LoadSystemList(XmlDocument& document, XmlNodeCollisionMap& collisionMap, XmlNodeList& nodeStore, const Path& filepath);

    /*!
     * Load and parse the given file to populate a property tree
     * @param document Document to populate
     * @param filepath Filepath to load & parse
     * @return false if the file does not exist or if the file is not parsable.
     */
    static bool LoadXMLFile(XmlDocument& document, const Path& filepath);

    /*!
     * @brief Create and add the favorite meta-system
     * @param systemList All system from which to fetch favorite games
     * @return True if the favorite system has been added
     */
    bool AddFavoriteSystem(const XmlNodeList& systemList);

    /*!
     * @brief Add Arcade meta-system
     * @return True if the arcade system has been added
     */
    bool AddArcadeMetaSystem();

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
     * @return
     */
    bool AddSpecialCollectionsMetaSystems();

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

    /*!
     * @brief Get User Configuration filepath
     * @return User Configuration filepath
     */
    static Path UserConfigurationPath()     { return RootFolders::DataRootFolder / "system/.emulationstation/es_systems.cfg"; }

    /*!
     * @brief Get Template Configuration filepath
     * @return Template Configuration filepath
     */
    static Path TemplateConfigurationPath() { return RootFolders::TemplateRootFolder / "system/.emulationstation/es_systems.cfg"; }

    /*!
     * @brief Deserialize an emulator node and all its tree into an EmulatorList object
     * @param treeNode XML node to deserialize
     * @param emulatorList target EmulatorList object
     */
    static void DeserializeEmulatorTree(XmlNode treeNode, EmulatorList& emulatorList);

    /*!
     * @brief Deserialize XML system node into a SystemDescriptor oject
     * @param treeNode Node to deserialize
     * @param systemDescriptor Target SystemDescriptor object
     * @return True if the XML node has been successfully deserialized
     */
    static bool DeserializeSystemDescriptor(XmlNode treeNode, SystemDescriptor& systemDescriptor);

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

  public:
    /*!
     * @brief constructor
     */
    SystemManager()
     : mProgressInterface(nullptr),
       mForceReload(false)
    {
    }

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
    SystemData* SystemByName(std::string& name);

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
     */
    bool LoadSystemConfigurations(FileNotifier& gamelistWatcher, bool ForeReload);

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
    FileData::List searchTextInGames(FolderData::FastSearchContext context, const std::string& text, int maxpersystem, int maxglobal);
};

