#pragma once

#include <vector>
#include <string>
#include "FileData.h"
#include "Window.h"
#include "MetaData.h"
#include "PlatformId.h"
#include "ThemeData.h"
#include "FileSorts.h"

#include <boost/property_tree/ptree.hpp>

class SystemData
{
public:
	SystemData(const std::string& name, const std::string& fullName, const std::string& startPath,
               const std::vector<std::string>& extensions, const std::string& command,
               const std::vector<PlatformIds::PlatformId>& platformIds, const std::string& themeFolder,
               std::map<std::string, std::vector<std::string>*>* map);
	SystemData(const std::string& name, const std::string& fullName, const std::string& command,
			   const std::string& themeFolder, std::vector<SystemData*>* systems);
	~SystemData();

	inline FileData* getRootFolder() const { return mRootFolder; };
	inline const std::string& getName() const { return mName; }
	inline const std::string& getFullName() const { return mFullName; }
	inline const std::string& getStartPath() const { return mStartPath; }
	//inline const std::vector<std::string>& getExtensions() const { return mSearchExtensions; }
	inline const std::string& getThemeFolder() const { return mThemeFolder; }
	inline bool getHasFavorites() const { return mHasFavorites; }
	inline bool isFavorite() const { return mIsFavorite; }
	inline std::vector<FileData*> getFavorites() const { return mRootFolder->getFavoritesRecursive(GAME); }
	inline unsigned int getSortId() const { return mSortId; };
	inline FileData::SortType getSortType() const { return FileSorts::SortTypes.at(mSortId); };
	inline void setSortId(const unsigned int sortId = 0) { mSortId = sortId; };

	inline const std::vector<PlatformIds::PlatformId>& getPlatformIds() const { return mPlatformIds; }
	inline bool hasPlatformId(PlatformIds::PlatformId id) { return std::find(mPlatformIds.begin(), mPlatformIds.end(), id) != mPlatformIds.end(); }

	inline const std::shared_ptr<ThemeData>& getTheme() const { return mTheme; }

	std::string getGamelistPath(bool forWrite) const;
	//bool hasGamelist() const;
	std::string getThemePath() const;
	
	unsigned int getGameCount() const;
	unsigned int getFavoritesCount() const;
	unsigned int getHiddenCount() const;

	void launchGame(Window* window, FileData* game, std::string netplay = "", std::string core = "", std::string ip = "", std::string port = "");

	static void deleteSystems();
	static bool loadConfig(); //Load the system config file at getConfigPath(). Returns true if no errors were encountered. An example will be written if the file doesn't exist.
	static void writeExampleConfig(const std::string& path);

	static std::string getUserConfigPath();
	static std::string getTemplateConfigPath();
	/*!
	 * Get user configuration path or if it does not exist, the template configuration path
	 * @param forWrite Force user configuration path to be returned
	 * @return Selected configuration path
	 */
	//static std::string getConfigPath(bool forWrite);

	static std::vector<SystemData*> sSystemVector;
	static SystemData *getFavoriteSystem();
	static SystemData* getSystem(std::string& name);
	static int getSystemIndex(const std::string& name);

	inline std::vector<SystemData*>::const_iterator getIterator() const { return std::find(sSystemVector.begin(), sSystemVector.end(), this); };
	inline std::vector<SystemData*>::const_reverse_iterator getRevIterator() const { return std::find(sSystemVector.rbegin(), sSystemVector.rend(), this); };
	
	inline SystemData* getNext() const
	{
		auto it = getIterator();
		it++;
		if(it == sSystemVector.end()) it = sSystemVector.begin();
		return *it;
	}

	inline SystemData* getPrev() const
	{
		auto it = getRevIterator();
		it++;
		if(it == sSystemVector.rend()) it = sSystemVector.rbegin();
		return *it;
	}

	// Load or re-load theme.
	void loadTheme();

	std::map<std::string, std::vector<std::string> *> * getEmulators();
	std::vector<std::string> getCores(const std::string& emulatorName);

    //! convenient ptree type access
    typedef boost::property_tree::ptree Tree;
    typedef std::pair<std::string, Tree> TreeNode;

    //! Convenient alias for system collision map
    typedef std::map<std::string, int> XmlNodeCollisionMap;
    //! Convenient alias for XML node list
    typedef std::vector<Tree> XmlNodeList;

private:
	std::string mName;
	std::string mFullName;
	std::string mStartPath;
	std::vector<std::string> mSearchExtensions;
	std::string mLaunchCommand;
	std::vector<PlatformIds::PlatformId> mPlatformIds;
	std::string mThemeFolder;
	std::shared_ptr<ThemeData> mTheme;

	bool mHasFavorites;
	bool mIsFavorite;
	unsigned int mSortId;

	void populateFolder(FileData* folder);

	FileData* mRootFolder;
	std::map<std::string, std::vector<std::string> *> *mEmulators;

	 /*!
	  * Run though the system list and store system nodes into the given store.
	  * If a system already exists in the store, the new system node is ignored
	  * @param collisionMap Collision map to keep track of nodes in the store
 	  * @param nodeStore Node store to fill with unique system nodes
 	  * @param document Source document
	  * @return true if the system list contains one or more system. False if the systemList node does not exist or is empty.
	  */
	static bool loadSystemNodes(XmlNodeCollisionMap& collisionMap, XmlNodeList& nodeStore, const Tree& document);

	/*!
	 */

	/*!
	 * Load systemList file into the given XML Document, then pase and store systems into the given node store
	 * Perform all file/xml checks.
	 * @param document Xml Document to store content to
     * @param collisionMap Collision map to keep track of nodes in the store
	 * @param nodeStore System node store
	 * @return true if the operation is successful and at least one system has been processed. False otherwise
	 */
    static bool loadSystemList(Tree& document, XmlNodeCollisionMap& collisionMap, XmlNodeList& nodeStore, const std::string& filepath);

    /*!
     * Load and parse the given file to populate a property tree
     * @param document Document to populate
     * @param filepath Filepath to load & parse
     * @return false if the file does not exist or if the file is not parsable.
     */
    static bool loadXmlFile(Tree& document, const std::string& filepath);

    static constexpr const char* mUserConfigurationRelativePath = "/.emulationstation/es_systems.cfg";
	static constexpr const char* mTemplateConfigurationAbsolutePath = "/etc/emulationstation/es_systems.cfg";
};
