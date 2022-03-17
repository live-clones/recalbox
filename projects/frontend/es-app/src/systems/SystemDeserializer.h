//
// Created by bkg2k on 03/04/2020.
//
#pragma once

#include <utils/Xml.h>
#include <vector>
#include <map>
#include <RootFolders.h>
#include <emulators/EmulatorList.h>
#include "SystemDescriptor.h"

class SystemDeserializer
{
  public:
    //! Convenient alias for system collision map
    typedef std::map<std::string, int> XmlNodeCollisionMap;
    //! Convenient alias for XML node list
    typedef std::vector<XmlNode> XmlNodeList;

  private:
    //! Favorite system internal name
    static constexpr const char* sFavoriteSystemShortName = "favorites";

    //! Sorted storage, keeping original system node ordering
    XmlNodeList mSystemList;
    //! System key to vector index
    XmlNodeCollisionMap mSystemMap;

    //! Template document
    XmlDocument mTemplateDocument;
    //! User systems
    XmlDocument mUserDocument;

    /*!
     * @brief Get Template Configuration filepath
     * @return Template Configuration filepath
     */
    static Path TemplateConfigurationPath() { return RootFolders::TemplateRootFolder / "system/.emulationstation/systemlist.xml"; }

    /*!
     * @brief Deserialize an emulator node and all its tree into an EmulatorList object
     * @param treeNode XML node to deserialize
     * @param emulatorList target EmulatorList object
     */
    static void DeserializeEmulatorTree(XmlNode treeNode, EmulatorList& emulatorList);

    /*!
     * Run though the system list and store system nodes into the given store.
     * If a system already exists in the store, the new system node is ignored
     * @param collisionMap Collision map to keep track of nodes in the store
     * @param nodeStore Node store to fill with unique system nodes
     * @param document Source document
     * @return true if the system list contains one or more system. False if the systemList node does not exist or is empty.
     */
    bool LoadSystemXMLNodes(const XmlDocument& document);

    /*!
     * Load and parse the given file to populate a property tree
     * @param document Document to populate
     * @param filepath Filepath to load & parse
     * @return false if the file does not exist or if the file is not parsable.
     */
    static bool LoadXMLFile(XmlDocument& document, const Path& filepath);

    /*!
     * Load systemList file into the given XML Document, then parse and store systems into the given node store
     * Perform all file/xml checks.
     * @param document Xml Document to store content to
     * @param collisionMap Collision map to keep track of nodes in the store
     * @param nodeStore System node store
     * @return true if the operation is successful and at least one system has been processed. False otherwise
     */
    bool LoadSystemList(XmlDocument& document, const Path& filepath);

  public:

    /*!
     * @brief Get User Configuration filepath
     * @return User Configuration filepath
     */
    static Path UserConfigurationPath()     { return RootFolders::DataRootFolder / "system/.emulationstation/systemlist.xml"; }

    /*!
     * @brief Deserialize XML system node into a SystemDescriptor object
     * @param index System index from 0..Count()-1
     * @param systemDescriptor Target SystemDescriptor object
     * @return True if the XML node has been successfully deserialized
     */
    bool Deserialize(int index, SystemDescriptor& systemDescriptor);

    /*!
     * @brief Load all systems
     */
    bool LoadSystems();

    /*!
     * @brief Get all system nodes
     * @return System node list
     */
    int Count() { return mSystemList.size(); }
};
