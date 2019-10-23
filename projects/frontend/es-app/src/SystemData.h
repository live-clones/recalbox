#pragma once

#include <vector>
#include <string>
#include "RootFolderData.h"
#include "Window.h"
#include "PlatformId.h"
#include "themes/ThemeData.h"
#include "FileSorts.h"
#include "Log.h"

#include <boost/property_tree/ptree.hpp>

class SystemData
{
  private:
    static constexpr int sMaximumPlatformIds = 8;
    static constexpr int sMaximumEmulators   = 4;
    static constexpr int sMaximumCores       = 8;

  public:
    class EmulatorDescriptor
    {
      private:
        std::string mEmulator;
        int mCoreCount;
        std::string mCores[sMaximumCores];

      public:
        EmulatorDescriptor()
          : mCoreCount(0)
        {
        }

        explicit EmulatorDescriptor(const std::string& emulator)
          : mEmulator(emulator),
            mCoreCount(0)
        {
        }

        const std::string& Name() const { return mEmulator; }

        int CoreCount() const { return mCoreCount; }

        bool HasAny() const { return mCoreCount != 0; }

        bool HasCore(const std::string& name) const
        {
          for(int i=mCoreCount; --i>=0; )
            if (name == mCores[i])
              return true;
          return false;
        }

        const std::string& Core(int index) const { return (unsigned int)index < (unsigned int)mCoreCount ? mCores[index] : mCores[0]; }

        void AddCore(const std::string& core)
        {
          if (mCoreCount < sMaximumCores)
            mCores[mCoreCount++] = core;
          else
            LOG(LogError) << "Core " << core << " cannot be added to emulator " << mEmulator;
        }
    };

    class EmulatorList
    {
      private:
        int mEmulatorCount;
        EmulatorDescriptor mEmulators[sMaximumEmulators];

        static EmulatorDescriptor sEmptyEmulator;

      public:
        EmulatorList()
          : mEmulatorCount(0)
        {
        }

        int Count() const { return mEmulatorCount; }

        const EmulatorDescriptor& At(int index) const { return (unsigned int)index < (unsigned int)mEmulatorCount ? mEmulators[index] : sEmptyEmulator; }

        const EmulatorDescriptor& Named(const std::string& name) const
        {
          for(int i=mEmulatorCount; --i>=0; )
            if (name == mEmulators[i].Name())
              return mEmulators[i];
          return sEmptyEmulator;
        }

        bool HasAny() const { return mEmulatorCount != 0; }

        bool HasNamed(const std::string& name) const
        {
          for(int i=mEmulatorCount; --i>=0; )
            if (name == mEmulators[i].Name())
              return true;
          return false;
        }

        void AddEmulator(const EmulatorDescriptor& emulatorDescriptor)
        {
          if (mEmulatorCount < sMaximumEmulators)
            mEmulators[mEmulatorCount++] = emulatorDescriptor;
          else
            LOG(LogError) << emulatorDescriptor.Name() << " cannot be added to current system.";
        }
    };

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
		//! Platform count
		int mPlatformCount;
		//! Platform identifiers (for scraping only)
		PlatformIds::PlatformId mPlatformIds[sMaximumPlatformIds];
		//! Theme data folder
		std::string mThemeFolder;
		//! Theme object
		std::shared_ptr<ThemeData> mTheme;
		//! Emulator/Cores tree
    EmulatorList mEmulators;
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
               const EmulatorList* emus, bool childOwnership, bool favorite);

  public:
    //! Return the root folder object
    RootFolderData* getRootFolder() { return &mRootFolder; };
    const std::string& getName() const { return mName; }
    const std::string& getFullName() const { return mFullName; }
    const std::string& getStartPath() const { return mStartPath; }
    const std::string& getThemeFolder() const { return mThemeFolder; }
    bool getHasFavoritesInTheme() const { return mTheme->getHasFavoritesInTheme(); }
    bool isFavorite() const { return mIsFavorite; }
    FileData::List getFavorites() const { return mRootFolder.getAllFavoritesRecursively(false); }
    unsigned int getSortId() const { return mSortId; };
    FileSorts::SortType getSortType(bool forFavorites) const { return forFavorites ? FileSorts::SortTypesForFavorites.at(mSortId) : FileSorts::SortTypes.at(mSortId); };
    void setSortId(const unsigned int sortId = 0) { mSortId = sortId; };

    PlatformIds::PlatformId PlatformIds(int index) const { return (unsigned int)index < (unsigned int)mPlatformCount ? mPlatformIds[index] : PlatformIds::PlatformId::PLATFORM_UNKNOWN; }
    int PlatformCount() const { return mPlatformCount; }
    bool HasPlatform() const { return mPlatformCount != 0; }
    bool hasPlatformId(PlatformIds::PlatformId id)
    {
      for(int i = mPlatformCount; --i >= 0;)
        if (id == mPlatformIds[i])
          return true;
      return false;
    }

    inline const std::shared_ptr<ThemeData>& getTheme() const { return mTheme; }

    std::string getGamelistPath(bool forWrite) const;
    std::string getThemePath() const;

    bool HasGame() const { return mRootFolder.hasGame(); }
    int GameCount() const { return mRootFolder.countAll(false); };
    int FavoritesCount() const{ return mRootFolder.countAllFavorites(false); };
    int HiddenCount() const{ return mRootFolder.countAllHidden(false); };

    void RunGame(Window& window, FileData& game, const std::string& netplay, const std::string& core, const std::string& ip, const std::string& port);

    // Load or re-load theme.
    void loadTheme();

    const EmulatorList& Emulators() const { return mEmulators; }

    static std::string demoInitialize(Window& window);
    static void demoFinalize(Window& window);
    bool DemoRunGame(const FileData& game, int duration, int infoscreenduration, const std::string& controlersConfig);
};
