#pragma once

#include <string>
#include <utils/cplusplus/INoCopy.h>
#include <emulators/EmulatorList.h>
#include <games/RootFolderData.h>
#include <WindowManager.h>
#include <systems/SystemDescriptor.h>
#include <games/FileSorts.h>
#include <themes/ThemeData.h>

class SystemManager;

class SystemData : private INoCopy
{
  public:
    //! System properties
    enum class Properties
    {
      None       =  0, //!< No properties
      Ports      =  1, //!< This system is the special "Ports" system
      Favorite   =  2, //!< This system is the special "Favorite" system
      Virtual    =  4, //!< This system is not a real system
      FixedSort  =  8, //!< This system has its own fixed sort
      AlwaysFlat = 16, //!< This system is presented always flat
      Searchable = 32, //!< We can search games in this system
      GameInPng  = 64, //!< Game is its image and vice-versa :)
      ScreenShots = 128, //! < This system is the special "Screenshots" system
    };

	private:
    // Allow manager to access this class
    friend class SystemManager;

    //! Parent manager
    SystemManager& mSystemManager;

    //! Descriptor
    SystemDescriptor mDescriptor;
    //! Theme object
    ThemeData mTheme;
    //! Root folders - Children are top level visible game/folder of the system
    RootFolderData mRootOfRoot;
    //! Is this system the favorite system?
    Properties mProperties;
    //! Fixed sort
    FileSorts::Sorts mFixedSort;

    /*!
     * @brief Populate the system using all available folder/games by gathering recursively
     * all files mathing the extension list
     * @param folder Root folder to recurse in
     * @param doppelgangerWatcher full path map to avoid adding a game more than once
     */
    void populateFolder(RootFolderData& folder, FileData::StringMap& doppelgangerWatcher);

    /*!
     * @brief Private constructor, called from SystemManager
     * @param System descriptor
     * @param childOwnership Type of children management
     * @param properties System properties
     */
    SystemData(SystemManager& systemManager, const SystemDescriptor& systemDescriptor, Properties properties, FileSorts::Sorts fixedSort = FileSorts::Sorts::FileNameAscending);

    /*!
     * @brief Lookup an existing game entry (or create it) in the current system.
     * @param root Game root path (usually system root path)
     * @param path Game path
     * @param type Type (folder/game)
     * @param doppelgangerWatcher Maps to avoid duplicate entries
     * @return Existing or newly created FileData
     */
    FileData* LookupOrCreateGame(RootFolderData& topAncestor, const Path& rootPath, const Path& path, ItemType type, FileData::StringMap& doppelgangerWatcher) const;

    /*!
     * @brief Parse xml gamelist files and add games to the current system
     * @param root Root rom folder
     * @param doppelgangerWatcher Maps to avoid duplicate entries
     * @param forceCheckFile True to force to check if file exists
     */
    void ParseGamelistXml(RootFolderData& root, FileData::StringMap& doppelgangerWatcher, bool forceCheckFile);

    /*!
     * @brief Get root folder of the given type
     * @param type root type
     * @return Found root or nullptr
     */
    RootFolderData* GetRootFolder(RootFolderData::Types type);

    /*!
     * @brief Get root folder of the given type
     * @param root root path
     * @return Found root or nullptr
     */
    RootFolderData* GetRootFolder(const Path& root);

    /*!
     * @brief Create new root folder
     * @param startpath Path
     * @param childownership Child ownership type
     * @param type Type of root
     * @return New root folder
     */
    RootFolderData& CreateRootFolder(const Path& startpath, RootFolderData::Ownership childownership, RootFolderData::Types type);

    /*!
     * @brief Lookup an existig root folder or create a new one using the given configuration
     * @param startpath Path
     * @param childownership Child ownership type
     * @param type Type of root
     * @return Existing or new root folder
     */
    RootFolderData& LookupOrCreateRootFolder(const Path& startpath, RootFolderData::Ownership childownership, RootFolderData::Types type);

  public:
    /*!
     * @brief Check if we must include adult games or not
     * @return True to include adult games in game lists
     */
    [[nodiscard]] bool IncludeAdultGames() const;

    /*!
     * @brief Get master root
     * @return Master root
     */
    RootFolderData& MasterRoot() { return mRootOfRoot; }
    /*!
     * @brief Get master root - const version
     * @return Master root
     */
    [[nodiscard]] const RootFolderData& MasterRoot() const { return mRootOfRoot; }

    /*!
     * @brief Get system descriptor
     * @return System descriptor
     */
    [[nodiscard]] const SystemDescriptor& Descriptor() const { return mDescriptor; }

    //! Get system name
    [[nodiscard]] const std::string& Name() const { return mDescriptor.Name(); }
    //! Get full name
    [[nodiscard]] const std::string& FullName() const { return mDescriptor.FullName(); }
    //! Theme folder
    //! Get system rotation option in system view (tate mode)
    bool Rotatable() const { return mDescriptor.Name() == "tate"; }
    [[nodiscard]] const std::string& ThemeFolder() const { return mDescriptor.ThemeFolder(); }

    //! Has favorite in theme?
    // TODO: Please kill me asap!
    [[nodiscard]] bool HasFavoritesInTheme() const { return mTheme.getHasFavoritesInTheme(); }

    [[nodiscard]] FileData::List getFavorites() const;
    [[nodiscard]] FileData::List getGames() const;
    [[nodiscard]] FileData::List getAllGames() const;
    [[nodiscard]] FileData::List getFolders() const;
    [[nodiscard]] FileData::List getTopGamesAndFolders() const;

    inline const ThemeData& Theme() const { return mTheme; }

    static Path getGamelistPath(const RootFolderData& root, bool forWrite);

    /*!
     * @brief Get list of writable Gamelists
     * @return List of writable gamelists
     */
    Path::PathList WritableGamelists();
    [[nodiscard]] Path getThemePath() const;

    [[nodiscard]] bool HasGame() const;
    [[nodiscard]] bool HasVisibleGame() const;

    /*!
    * @brief Check if system has no only RO games
    * @return if has no only RO games
    */
    [[nodiscard]] bool HasScrapableGame() const;
    [[nodiscard]] int GameCount() const;
    [[nodiscard]] int GameAndFolderCount() const;
    [[nodiscard]] int FavoritesCount() const;
    [[nodiscard]] int HiddenCount() const;

    // Load or re-load theme.
    void loadTheme();

    [[nodiscard]] const EmulatorList& Emulators() const { return mDescriptor.EmulatorTree(); }

    //! Is this system auto scrapable? (Games in png?)
    [[nodiscard]] bool IsAutoScrapable() const;

    //! Is this system the "Favorite" system?
    [[nodiscard]] bool IsFavorite() const;

    //! Is this system the "Ports" system?ScreenShots
    [[nodiscard]] bool IsPorts() const;

    [[nodiscard]] bool IsScreenshots() const;

    //! Is this system virtual?
    [[nodiscard]] bool IsVirtual() const;

    //! Is this system selt sorted
    [[nodiscard]] bool IsSelfSorted() const;

    //! Is this system always flat?
    [[nodiscard]] bool IsAlwaysFlat() const;

    //! Is this system always flat?
    [[nodiscard]] bool IsSearchable() const;

    /*!
     * @brief Get or create pure virtual root - USE IT ONLY ON FAVORITE SYSTEM
     * @return Virtual root
     */
    FolderData& GetFavoriteRoot();

    [[nodiscard]] FileSorts::Sorts FixedSort() const { return mFixedSort; }

    /*!
     * @brief Write modified games back to the gamelist xml file
     */
    void UpdateGamelistXml();

    /*!
     * @brief Update game list with a single game on top of the list
     * @param game game to insert or move
     */
    void UpdateLastPlayedGame(FileData& game);

    /*!
     * @brief Rebuild a complete map path/FileData recursively
     * @param doppelganger Map to fill in
     * @param includefolder Include folder or not
     */
    void BuildDoppelgangerMap(FileData::StringMap& doppelganger, bool includefolder) const;

    /*!
     * @brief Get parent system manager
     * @return
     */
    [[nodiscard]] SystemManager& Manager() const { return mSystemManager; }

    /*!
     * @brief Fill in the given Item series with all filedata recursively
     * @param into Item series to fill in
     * @param context Context for indexes
     */
    void BuildFastSearchSeries(FolderData::FastSearchItemSerie& into, FolderData::FastSearchContext context) const;

    /*!
     * @brief Get excludes filter
     * @return excludes Filter
     */
    [[nodiscard]] FileData::Filter Excludes() const;
};

DEFINE_BITFLAG_ENUM(SystemData::Properties, int)