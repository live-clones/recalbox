#pragma once

#include "SystemData.h"
#include <utils/os/system/ThreadPool.h>
#include <utils/os/system/ThreadPoolWorkerInterface.h>
#include <RootFolders.h>

//! convenient ptree type access
typedef boost::property_tree::ptree Tree;

class SystemManager :
  public ThreadPoolWorkerInterface<Tree, SystemData*>, // Multi-threaded system loading
  public ThreadPoolWorkerInterface<SystemData*, bool> // Multi-threaded system unloading
{
  public:
    //! Convenient alias for system collision map
    typedef std::map<std::string, int> XmlNodeCollisionMap;
    //! Convenient alias for XML node list
    typedef std::vector<Tree> XmlNodeList;

  private:
    static constexpr const char* sWeightFilePath = "/recalbox/share/system/.emulationstation/.weights";

    //! Visible system, including virtual system (Arcade)
    std::vector<SystemData*> sVisibleSystemVector;
    //! Hidden system, just here to hold their own children
    std::vector<SystemData*> sHiddenSystemVector;
    //! ALL systems, visible and hidden
    std::vector<SystemData*> sAllSystemVector;

    /*!
     * @brief Private constructor
     */
    SystemManager() = default;

    /*!
     * Run though the system list and store system nodes into the given store.
     * If a system already exists in the store, the new system node is ignored
     * @param collisionMap Collision map to keep track of nodes in the store
     * @param nodeStore Node store to fill with unique system nodes
     * @param document Source document
     * @return true if the system list contains one or more system. False if the systemList node does not exist or is empty.
     */
    bool loadSystemNodes(XmlNodeCollisionMap& collisionMap, XmlNodeList& nodeStore, const Tree& document);

    /*!
     * Load systemList file into the given XML Document, then parse and store systems into the given node store
     * Perform all file/xml checks.
     * @param document Xml Document to store content to
     * @param collisionMap Collision map to keep track of nodes in the store
     * @param nodeStore System node store
     * @return true if the operation is successful and at least one system has been processed. False otherwise
     */
    bool loadSystemList(Tree& document, XmlNodeCollisionMap& collisionMap, XmlNodeList& nodeStore, const std::string& filepath);

    /*!
     * Load and parse the given file to populate a property tree
     * @param document Document to populate
     * @param filepath Filepath to load & parse
     * @return false if the file does not exist or if the file is not parsable.
     */
    static bool loadXmlFile(Tree& document, const std::string& filepath);

    bool AddFavoriteSystem(const XmlNodeList& systemList);

    bool AddArcadeMetaSystem();

    static SystemData* CreateRegularSystem(const std::string& name, const std::string& fullName, const std::string& startPath,
                                    const std::string& filteredExtensions, const std::string& command,
                                    const std::vector<PlatformIds::PlatformId>& platformIds, const std::string& themeFolder,
                                    const SystemData::EmulatorList& emuNodes);

    static SystemData* CreateFavoriteSystem(const std::string& name, const std::string& fullName,
                                      const std::string& themeFolder, const std::vector<SystemData*>& systems);

    static SystemData* CreateMetaSystem(const std::string& name, const std::string& fullName,
                                 const std::string& themeFolder, const std::vector<SystemData*>& systems);

    static void writeExampleConfig(const std::string& path);

    static std::string getUserConfigurationAbsolutePath()     { return RootFolders::DataRootFolder     + "/system/.emulationstation/es_systems.cfg"; }
    static std::string getTemplateConfigurationAbsolutePath() { return RootFolders::TemplateRootFolder + "/system/.emulationstation/es_systems.cfg"; }

    static SystemData::EmulatorList DeserializeEmulatorTree(const Tree& treeNode);

    /*
     * ThreadPoolWorkingInterface implementation
     */

    /*!
     * @brief Load and parse a single gamelist, then return a complete fulfilled system
     * @param feed System object from es_systems.cfg
     * @return New SystemData object or nullptr
     */
    SystemData* ThreadPoolRunJob(Tree& feed) override;

    /*!
     * @brief Update a single gamelist from the metadata
     * @param feed System from which to update the gamelist
     * @return Always true
     */
    bool ThreadPoolRunJob(SystemData*& feed) override;

  public:
    static SystemManager& Instance()
    {
      static SystemManager instance;
      return instance;
    }

    SystemData* getFavoriteSystem();
    SystemData* getSystem(std::string& name);
    SystemData* getFirstSystemWithGame();
    int getVisibleSystemIndex(const std::string& name);

    void deleteSystems();
    /*!
     * @brief Load the system config file at getConfigPath(). Returns true if no errors were encountered. An example will be written if the file doesn't exist.
     */
    bool loadConfig();

    const std::vector<SystemData*>& getAllSystems() { return sAllSystemVector; }
    const std::vector<SystemData*>& getVisibleSystems() { return sVisibleSystemVector; }
    const std::vector<SystemData*>& getHiddenSystems() { return sHiddenSystemVector; }

    inline SystemData* getNextVisible(SystemData* to) const
    {
      int size = (int)sVisibleSystemVector.size();
      for(int i = size; --i>=0; )
        if (sVisibleSystemVector[i] == to)
          return sVisibleSystemVector[(++i) % size];
      return nullptr;
    }

    inline SystemData* getPreviousVisible(SystemData* to) const
    {
      int size = (int)sVisibleSystemVector.size();
      for(int i = size; --i>=0; )
        if (sVisibleSystemVector[i] == to)
          return sVisibleSystemVector[(--i + size) % size];
      return nullptr;
    }
};

