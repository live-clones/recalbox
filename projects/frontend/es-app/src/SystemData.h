#pragma once

#include <vector>
#include <string>
#include "RootFolderData.h"
#include "Window.h"
#include "PlatformId.h"
#include "themes/ThemeData.h"
#include "FileSorts.h"

#include <boost/property_tree/ptree.hpp>

class SystemData
{
	private:
    friend class SystemManager;

    //! convenient ptree type access
    typedef boost::property_tree::ptree Tree;

    //! Short name
    std::string mName;
    //! Long name
		std::string mFullName;
		//! Rom path
		std::string mStartPath;
		//! Extension list
		std::string mSearchExtensions;
		//! Cmmand to run a game
		std::string mLaunchCommand;
		//! Platform identifiers (for scraping only)
		std::vector<PlatformIds::PlatformId> mPlatformIds;
		//! Theme data folder
		std::string mThemeFolder;
		//! Theme object
		std::shared_ptr<ThemeData> mTheme;
		//! Emulator/Cores tree
    std::map<std::string, std::vector<std::string> *> mEmulators;
    //! Root folder - Children are top level visible gale/folder of the system
    RootFolderData mRootFolder;
    //! Sorting index
    unsigned int mSortId;
    //! Is this system the favorite system?
		bool mIsFavorite;

    /*!
     * @brief Populate the system using all available folder/games by gathering recursively
     * all files mathing the extension list
     * @param folder Root folder to recurse in
     * @param doppelgangerWatcher full path map to avoid symlink to inject a game more than once
     */
    void populateFolder(FolderData* folder, FileData::StringMap& doppelgangerWatcher);

    /*!
     * @brief Private constructor, called from SystemManager
     * @param name Short name (e.g: snes)
     * @param fullName Long name (e.g: Super Nintendo Entertainment System)
     * @param startPath Rom path (e.g: /recalbox/share/roms/snes)
     * @param filteredExtensions Rom extensions (e.g: "zip 7z smc sfc")
     * @param command Command to run a single game from this system
     * @param platformIds Platform identifier list used for scraping only
     * @param themeFolder Theme data folder
     * @param emuNodes XML emulator node (e.g: a tree of emulator/cores)
     * @param childOwnership True og the system own all its children game/folder.
     * False if the system is a virtual system. Avoid games to be destroyed more than once
     * @param favorite True if this is the favorite system.
     */
    SystemData(const std::string& name, const std::string& fullName, const std::string& startPath,
               const std::string& filteredExtensions, const std::string& command,
               const std::vector<PlatformIds::PlatformId>& platformIds, const std::string& themeFolder,
               const Tree* emuNodes, bool childOwnership, bool favorite);

  public:
    /*!
     * @brief Destructor
     */
    ~SystemData();

    //! Return the root folder object
    inline RootFolderData* getRootFolder() { return &mRootFolder; };
    inline const std::string& getName() const { return mName; }
    inline const std::string& getFullName() const { return mFullName; }
    inline const std::string& getStartPath() const { return mStartPath; }
    inline const std::string& getThemeFolder() const { return mThemeFolder; }
    inline bool getHasFavoritesInTheme() const { return mTheme->getHasFavoritesInTheme(); }
    inline bool isFavorite() const { return mIsFavorite; }
    inline FileData::List getFavorites() const { return mRootFolder.getAllFavoritesRecursively(false); }
    inline unsigned int getSortId() const { return mSortId; };
    inline FileSorts::SortType getSortType(bool forFavorites) const { return forFavorites ? FileSorts::SortTypesForFavorites.at(mSortId) : FileSorts::SortTypes.at(mSortId); };
    inline void setSortId(const unsigned int sortId = 0) { mSortId = sortId; };

    inline const std::vector<PlatformIds::PlatformId>& getPlatformIds() const { return mPlatformIds; }
    inline bool hasPlatformId(PlatformIds::PlatformId id) { return std::find(mPlatformIds.begin(), mPlatformIds.end(), id) != mPlatformIds.end(); }

    inline const std::shared_ptr<ThemeData>& getTheme() const { return mTheme; }

    std::string getGamelistPath(bool forWrite) const;
    //bool hasGamelist() const;
    std::string getThemePath() const;

    bool hasGame() const { return mRootFolder.hasGame(); }
    int getGameCount() const { return mRootFolder.countAll(false); };
    int getFavoritesCount() const{ return mRootFolder.countAllFavorites(false); };
    int getHiddenCount() const{ return mRootFolder.countAllHidden(false); };

    void launchGame(Window* window, FileData* game, const std::string& netplay, const std::string& core, const std::string& ip, const std::string& port);

    // Load or re-load theme.
    void loadTheme();

    std::map<std::string, std::vector<std::string> *> * getEmulators() { return &mEmulators; }

    std::vector<std::string> getCores(const std::string& emulatorName);

    static std::string demoInitialize(Window& window);
    static void demoFinalize(Window& window);
    bool demoLaunchGame(FileData* game, int duration, int infoscreenduration, const std::string& controlersConfig);
};
