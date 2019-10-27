#pragma once

#include <string>
#include "games/RootFolderData.h"
#include "Window.h"
#include "PlatformId.h"
#include "themes/ThemeData.h"
#include "FileSorts.h"
#include "EmulatorList.h"
#include "SystemDescriptor.h"

class SystemData
{
	private:
    friend class SystemManager;

    //! convenient ptree type access
    typedef boost::property_tree::ptree Tree;

    //! Descriptor
    SystemDescriptor mDescriptor;
    //! Theme object
    ThemeData mTheme;
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
     * @param System descriptor
     * @param childOwnership True og the system own all its children game/folder.
     * False if the system is a virtual system. Avoid games to be destroyed more than once
     * @param favorite True if this is the favorite system.
     */
    SystemData(const SystemDescriptor& systemDescriptor, bool childOwnership, bool favorite);

    /*!
     * @brief Get localized text inside a text. Look for [lg] tags to mark start/end of localized texts
     * if the current language is not found, the method looks for [en].
     * if still not found, the whole text is returned
     * @param source Source text
     * @return localized text
     */
    static std::string getLocalizedText(const std::string& source);

    /*!
     * @brief Looks for folder override files in the given folder.
     * If overriden images/texts are found, thay are loaded to override empty or gamelist information
     * The methods looks for:
     * - .folder.picture.svg or .folder.picture.png
     * - .folder.description.txt
     * @param folderdata folder data to override
     */
    static void overrideFolderInformation(FileData* folderdata);

    /*!
     * @brief Lookup an existing game entry (or create it) in the current system.
     * @param path Game path
     * @param type Type (folder/game)
     * @param doppelgangerWatcher Maps to avoid duplicate entries
     * @return Existing or newly created FileData
     */
    FileData* LookupOrCreateGame(const boost::filesystem::path& path, ItemType type, FileData::StringMap& doppelgangerWatcher);

    /*!
     * @brief Parse xml gamelist files and add games to the current system
     * @param doppelgangerWatcher Maps to avoid duplicate entries
     */
    void ParseGamelistXml(FileData::StringMap& doppelgangerWatcher);

  public:
    //! Return the root folder object
    RootFolderData* getRootFolder() { return &mRootFolder; };
    const std::string& getName() const { return mDescriptor.Name(); }
    const std::string& getFullName() const { return mDescriptor.FullName(); }
    const Path& getStartPath() const { return mDescriptor.RomPath(); }
    const std::string& ThemeFolder() const { return mDescriptor.ThemeFolder(); }
    bool getHasFavoritesInTheme() const { return mTheme.getHasFavoritesInTheme(); }
    bool isFavorite() const { return mIsFavorite; }
    FileData::List getFavorites() const { return mRootFolder.getAllFavoritesRecursively(false); }
    unsigned int getSortId() const { return mSortId; };
    FileSorts::SortType getSortType(bool forFavorites) const { return forFavorites ? FileSorts::SortTypesForFavorites.at(mSortId) : FileSorts::SortTypes.at(mSortId); };
    void setSortId(const unsigned int sortId = 0) { mSortId = sortId; };

    PlatformIds::PlatformId PlatformIds(int index) const { return mDescriptor.Platform(index); }
    int PlatformCount() const { return mDescriptor.PlatformCount(); }
    bool HasPlatform() const { return mDescriptor.PlatformCount() != 0; }
    bool hasPlatformId(PlatformIds::PlatformId id) { return mDescriptor.HasPlatform(id); }

    inline const ThemeData& getTheme() const { return mTheme; }

    std::string getGamelistPath(bool forWrite) const;
    std::string getThemePath() const;

    bool HasGame() const { return mRootFolder.hasGame(); }
    int GameCount() const { return mRootFolder.countAll(false); };
    int FavoritesCount() const{ return mRootFolder.countAllFavorites(false); };
    int HiddenCount() const{ return mRootFolder.countAllHidden(false); };

    void RunGame(Window& window, FileData& game, const std::string& netplay, const std::string& core, const std::string& ip, const std::string& port);

    // Load or re-load theme.
    void loadTheme();

    const EmulatorList& Emulators() const { return mDescriptor.EmulatorTree(); }

    static std::string demoInitialize(Window& window);
    static void demoFinalize(Window& window);
    bool DemoRunGame(const FileData& game, int duration, int infoscreenduration, const std::string& controlersConfig);

    /*!
     * @brief Write modified games back to the gamelist xml file
     */
    void UpdateGamelistXml();
};
