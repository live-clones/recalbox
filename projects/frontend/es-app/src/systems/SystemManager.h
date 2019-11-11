#pragma once

#include "SystemData.h"
#include "SystemDescriptor.h"
#include <utils/os/system/ThreadPoolWorkerInterface.h>
#include <RootFolders.h>
#include <utils/Xml.h>

class SystemManager :
  public ThreadPoolWorkerInterface<SystemDescriptor, SystemData*>, // Multi-threaded system loading
  public ThreadPoolWorkerInterface<SystemData*, bool> // Multi-threaded system unloading
{
  public:
    //! Convenient alias for system collision map
    typedef std::map<std::string, int> XmlNodeCollisionMap;
    //! Convenient alias for XML node list
    typedef std::vector<XmlNode> XmlNodeList;
    //! Convenient alias for System list
    typedef std::vector<SystemData*> SystemList;

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
    static bool loadSystemNodes(XmlNodeCollisionMap& collisionMap, XmlNodeList& nodeStore, const XmlDocument& document);

    /*!
     * Load systemList file into the given XML Document, then parse and store systems into the given node store
     * Perform all file/xml checks.
     * @param document Xml Document to store content to
     * @param collisionMap Collision map to keep track of nodes in the store
     * @param nodeStore System node store
     * @return true if the operation is successful and at least one system has been processed. False otherwise
     */
    static bool loadSystemList(XmlDocument& document, XmlNodeCollisionMap& collisionMap, XmlNodeList& nodeStore, const Path& filepath);

    /*!
     * Load and parse the given file to populate a property tree
     * @param document Document to populate
     * @param filepath Filepath to load & parse
     * @return false if the file does not exist or if the file is not parsable.
     */
    static bool loadXmlFile(XmlDocument& document, const Path& filepath);

    bool AddFavoriteSystem(const XmlNodeList& systemList);

    bool AddArcadeMetaSystem();

    static SystemData* CreateRegularSystem(const SystemDescriptor& systemDescriptor);

    static SystemData* CreateFavoriteSystem(const std::string& name, const std::string& fullName,
                                            const std::string& themeFolder, const std::vector<SystemData*>& systems);

    static SystemData* CreateMetaSystem(const std::string& name, const std::string& fullName,
                                        const std::string& themeFolder, const std::vector<SystemData*>& systems);

    static void writeExampleConfig(const Path& path);

    static Path getUserConfigurationAbsolutePath()     { return RootFolders::DataRootFolder     / "system/.emulationstation/es_systems.cfg"; }
    static Path getTemplateConfigurationAbsolutePath() { return RootFolders::TemplateRootFolder / "system/.emulationstation/es_systems.cfg"; }

    static void DeserializeEmulatorTree(XmlNode treeNode, EmulatorList& emulatorList);
    static bool DeserializeSystemDescriptor(XmlNode treeNode, SystemDescriptor& systemDescriptor);

    /*
     * ThreadPoolWorkingInterface implementation
     */

    /*!
     * @brief Load and parse a single gamelist, then return a complete fulfilled system
     * @param feed System object from es_systems.cfg
     * @return New SystemData object or nullptr
     */
    SystemData* ThreadPoolRunJob(SystemDescriptor& feed) override;

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

    /*!
     * @brief Get All system list, visibles + hidden
     * @return System list
     */
    const SystemList& GetAllSystemList() { return sAllSystemVector; }
    /*!
     * @brief Get visible-only system list
     * @return System list
     */
    const SystemList& GetVisibleSystemList() { return sVisibleSystemVector; }
    /*!
     * @brief Get Hidden-only system list
     * @return System list
     */
    const SystemList& GetHiddenSystemList() { return sHiddenSystemVector; }

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

