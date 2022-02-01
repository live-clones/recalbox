#pragma once

#include <utils/storage/Set.h>
#include "FileData.h"
#include "IFilter.h"

class FolderData : public FileData
{
  private:
    //! Blacklist
    typedef HashSet<std::string> FileSet;

    using FileData::CalculateHash;

  protected:
    //! Current folder child list
    FileData::List mChildren;

    /*!
     * Constructor
     */
    FolderData(RootFolderData& topAncestor, const Path& startpath)
      : FileData(ItemType::Root, startpath, topAncestor)
      , mChildren()
    {
    }

    /*!
     * Clear the internal child lists without destroying them.
     * Used by inherited class that store children object without ownership
     */
    void ClearChildList() { mChildren.clear(); }

    /*!
     * @brief Clear the internal child list recusively but the folders
     * keeping the folder structure
     */
    void ClearSubChildList();

    /*!
     * @brief Get all folders recursively
     * @param Folder list
     * @return Total amount of games
     */
    int getAllFoldersRecursively(FileData::List& to) const;

    /*!
     * Get all items recursively.
     * @param to List to fill
     * @param includes Get only items matching these filters
     * @param includefolders Include folder as regular item, or just get their children
     * @return Total amount of items (not including folders!)
     */
    int getItemsRecursively(FileData::List& to, Filter includes, bool includefolders, bool includeadult) const;
    /*!
     * Get filtered items recursively.
     * @param to List to fill
     * @param includes Get only items matching these filters
     * @param includefolders Include folder as regular item, or just get their children
     * @return Total amount of items (not including folders!)
     */
    int getItemsRecursively(FileData::List& to, IFilter* filter, bool includefolders, bool includeadult) const;
    /*!
     * Count all items recursively
     * @param includes Count only items matching these filters
     * @return Total amount of items (not including folders!)
     */
    int countItemsRecursively(Filter includes, bool includefolders, bool includeadult) const;

    /*!
     * Get all items
     * @param to List to fill
     * @param includes Get only items matching these filters
     * @param includefolders Include folder as regular item, or just get their children
     * @return Total amount of items (not including folders!)
     */
    int getItems(FileData::List& to, Filter includes, bool includefolders, bool includeadult) const;
    /*!
     * Count all items
     * @param includes Count only items matching these filters
     * @return Total amount of items (not including folders!)
     */
    int countItems(Filter includes, bool includefolders, bool includeadult) const;

    /*!
     * Lookup for a given game in the current tree
     * @param gameNameOrHash item to search for, either a hash or a path (w/ or w/o file extension)
     * @param attributes Compare item against hashes, filenames or filenames with extension
     * @param path Current tree path
     * @return First mathing game or nullptr
     */
    FileData* LookupGame(const std::string& item, SearchAttributes attributes, const std::string& path) const;

    /*!
     * Highly optimized Quicksort, inpired from original Delphi 7 code
     * @param low Lowest element
     * @param high Highest element
     * @param comparer Compare method
     */
    static void QuickSortAscending(FileData::List& items, int low, int high, FileData::Comparer comparer);

    /*!
     * Highly optimized Quicksort, inpired from original Delphi 7 code
     * @param low Lowest element
     * @param high Highest element
     * @param comparer Compare method
     */
    static void QuickSortDescending(FileData::List& items, int low, int high, FileData::Comparer comparer);

    /*!
     * @brief Search text into the game name, returning the index of the text
     * @param text text to search for
     * @param fd FileData to search in
     * @return Index of the text found, or -1
     */
    static int FastSearchText(const std::string& text, const std::string& into);

    static bool ContainsMultiDiskFile(const std::string& extensions)
    {
      return Strings::Contains(extensions, ".m3u") || Strings::Contains(extensions, ".cue") ||
      Strings::Contains(extensions, ".ccd") || Strings::Contains(extensions, ".gdi");
    }

    static void ExtractUselessFiles(const Path::PathList& items, FileSet& blacklist);

    static void ExtractUselessFilesFromCue(const Path& path, FileSet& list);
    static void ExtractUselessFilesFromCcd(const Path& path, FileSet& list);
    static void ExtractUselessFilesFromM3u(const Path& path, FileSet& list);
    static void ExtractUselessFilesFromGdi(const Path& path, FileSet& list);
    static void ExtractFileNameFromLine(const std::string& line, FileSet& list);

    static constexpr int sMaxGdiFileSize = (10 << 10); // 10 Kb

  public:
    typedef std::vector<FolderData*> List;
    typedef std::vector<const FolderData*> ConstList;

    //! Search context
    enum class FastSearchContext
    {
        Path,
        Name,
        Description,
        Developer,
        Publisher,
        All,
    };

    /*!
     * @brief Fast search data holder
     */
    struct FastSearchItem
    {
      int Distance;   // Distance of the found text
      FileData* Data; // FileData object

      FastSearchItem(int distance, FileData* data) : Distance(distance), Data(data) {}
    };
    //! Result list alias
    typedef std::vector<FastSearchItem> ResultList;

    /*!
     * Constructor
     */
    FolderData(const Path& startpath, RootFolderData& topAncestor)
      : FileData(ItemType::Folder, startpath, topAncestor)
      , mChildren()
    {
    }

    /*!
     * Desctructor
     */
    ~FolderData();

    /*!
     * Add a child item to the current folder
     * @param file Item to add
     * @param lukeImYourFather Set the current folder as the item's parent
     */
    void AddChild(FileData* file, bool lukeImYourFather);
    /*!
     * Try to remove a child item from the current folder
     * @param file Item to remove
     */
    void RemoveChild(FileData* file);

    /*!
     * Return true if this FileData is a folder and has at lease one child
     * @return Boolean result
     */
    bool HasChildren() const { return !mChildren.empty(); }

    /*!
     * Run through filesystem's folders, seeking for games, and when found, add them into the current tree
     * @param filteredExtensions Filter files that do not match this extension list (casee sensitive)
     * @param systemData System to attach to
     * @param doppelgangerWatcher Map used to check duplicate games
     */
    void PopulateRecursiveFolder(RootFolderData& root, const std::string& filteredExtensions, FileData::StringMap& doppelgangerWatcher);

    /*!
     * Get next favorite game, starting from the reference entry
     * The method seek for next favorite forth, then back.
     * @param reference Position to start from
     * @return Next favorite FileData, or nullptr if no favorite is available
     */
    FileData* GetNextFavoriteTo(FileData* reference);

    /*!
     * Lookup for a given game in the current tree
     * @param item item to search for, either a hash or a path (w/ or w/o file extension)
     * @param attributes Compare item against hashes, filenames or filenames with extension
     * @return First mathing game or nullptr
     */
    FileData* LookupGame(const std::string& item, SearchAttributes attributes) const { return LookupGame(item, attributes, std::string()); }

    /*!
     * Return true if contain at least one game
     */
    bool HasGame() const;

    /*!
     * Return true if contain at least one visible game
     */
    bool HasVisibleGame() const;

    /*!
     * Return true if contain at least one visible game with a video md
     */
    bool HasVisibleGameWithVideo() const;

    /*!
     * @brief Check if there are one or more missing hash recursively
     */
    bool HasMissingHashRecursively();

    /*!
     * @brief Compute all missing hashes recursively
     */
    void CalculateMissingHashRecursively();

    /*!
     * @brief Get all non-hashed roms
     * @param to FileData list to receive non-hashed roms
     * @return Total non-hashed rom count
     */
    int getMissingHashRecursively(FileData::List& to) const;

    /*!
     * Get total games in all folders, including hidden
     * @param includefolders True to include subfolders in the result
     * @return Game count
     */
    int CountAll(bool includefolders, bool includeadult) const { return countItemsRecursively(Filter::All, includefolders, includeadult); }

    /*!
     * Get favorite games in all folders
     * @param includefolders True to include subfolders in the result
     * @return Favorite game count
     */
    int CountAllFavorites(bool includefolders, bool includeadult) const { return countItemsRecursively(Filter::Favorite, includefolders, includeadult); }

    /*!
     * Get hidden games in all folders
     * @param includefolders True to include subfolders in the result
     * @return Hidden game count
     */
    int CountAllHidden(bool includefolders, bool includeadult) const { return countItemsRecursively(Filter::Hidden, includefolders, includeadult); }

    /*!
     * Return true if at least one game in the three has more metadata than just path & names
     * @return True if detailed data are available
     */
    bool HasDetailedData() const;

    /*!
     * Return true if the current folder contains the given item
     * @param item item to seek for
     * @param recurse True to search in subfolders
     * @return True if the item has been found
     */
    bool Contains(const FileData* item, bool recurse) const;

    /*!
     * Quick sort items in the given list.
     * @param items Items to sort
     * @param comparer Comparison function
     * @param ascending True for ascending sort, false for descending.
     */
    static void Sort(FileData::List& items, FileData::Comparer comparer, bool ascending);

    /*!
     * Count filtered items recursively starting from the current folder
     * @param filters Filter to apply
     * @param includefolders True to include subfolders in the result
     * @return List of filtered items
     */
    int CountFilteredItemsRecursively(Filter filters, bool includefolders, bool includeadult) const { return countItemsRecursively(filters, includefolders, includeadult); }
    /*!
     * Count filtered items from the current folder
     * @param filters Filter to apply
     * @param includefolders True to include subfolders in the result
     * @return List of filtered items
     */
    int CountFilteredItems(Filter filters, bool includefolders, bool includeadult) const { return countItems(filters, includefolders, includeadult); }
    /*!
     * Count displayable items (normal + favorites) recursively starting from the current folder
     * @param includefolders True to include subfolders in the resulting list
     * @return Number of displayable items
     */
    int CountAllDisplayableItemsRecursively(bool includefolders, bool includeadult) const { return countItemsRecursively(Filter::Normal | Filter::Favorite, includefolders, includeadult); }

    /*!
     * Get filtered items recursively starting from the current folder
     * @param filters Filter to apply
     * @param includefolders True to include subfolders in the resulting list
     * @return List of filtered items
     */
    FileData::List GetFilteredItemsRecursively(Filter filters, bool includefolders, bool includeadult) const;
    /*!
     * Get filtered items recursively starting from the current folder
     * @param includefolders True to include subfolders in the resulting list
     * @return List of filtered items
     */
    FileData::List GetFilteredItemsRecursively(IFilter* filter, bool includefolders, bool includeadult) const;
    /*!
     * Get all items recursively starting from the current folder
     * @param includefolders True to include subfolders in the resulting list
     * @return List of filtered items
     */
    FileData::List GetAllItemsRecursively(bool includefolders, bool includeadult) const;
    /*!
     * Get all displayable items (normal + favorites) recursively starting from the current folder
     * @param includefolders True to include subfolders in the resulting list
     * @return List of filtered items
     */
    FileData::List GetAllDisplayableItemsRecursively(bool includefolders, bool includeadult) const;
    /*!
     * Get all favorite items recursively starting from the current folder
     * @param includefolders True to include subfolders in the resulting list
     * @return List of filtered items
     */
    FileData::List GetAllFavoritesRecursively(bool includefolders, bool includeadult) const;

    /*!
     * Get filtered items from the current folder
     * @param filters Filter to apply
     * @param includefolders True to include subfolders in the resulting list
     * @return List of filtered items
     */
    FileData::List GetFilteredItems(Filter filters, bool includefolders, bool includeadult) const;
    /*!
     * Get all items from the current folder
     * @param includefolders True to include subfolders in the resulting list
     * @return List of filtered items
     */
    FileData::List GetAllItems(bool includefolders, bool includeadult) const;
    /*!
     * Get all displayable items (normal + favorites) from the current folder
     * @param includefolders True to include subfolders in the resulting list
     * @return List of filtered items
     */
    FileData::List GetAllDisplayableItems(bool includefolders, bool includeadult) const;
    /*!
     * Get all favorite items from the current folder
     * @param includefolders True to include subfolders in the resulting list
     * @return List of filtered items
     */
    FileData::List GetAllFavorites(bool includefolders, bool includeadult) const;

    /*!
     * @brief Get all folders recursively
     * @return Folder list
     */
    FileData::List GetAllFolders() const;

    /*!
     * @brief Get all folders recursively - Root hierarchy aware methods!
     * @param to List to fill in
     * @return Folder count
     */
    int GetFoldersRecursivelyTo(FileData::List& to) const;
    /*!
     * Get filtered items recursively. - Root hierarchy aware methods!
     * @param to List to fill
     * @param includes Get only items matching these filters
     * @param includefolders Include folder as regular item, or just get their children
     * @return Total amount of items (not including folders!)
     */
    int GetItemsRecursivelyTo(FileData::List& to, Filter includes, bool includefolders, bool includeadult) const;
    /*!
     * Get all items - Root hierarchy aware methods!
     * @param to List to fill
     * @param includes Get only items matching these filters
     * @param includefolders Include folder as regular item, or just get their children
     * @return Total amount of items (not including folders!)
     */
    int GetItemsTo(FileData::List& to, Filter includes, bool includefolders, bool includeadult) const;

    /*!
     * @brief Check if the folder (or one of its subfolders) contains at least one game
     * with dirty metadata (modified)
     * @return True if the folder is "dirty"
     */
    bool IsDirty() const;

    /*!
     * @brief Rebuild a complete map path/FileData recursively
     * @param doppelganger Map to fill in
     * @param includefolder include folder or not
     */
    void BuildDoppelgangerMap(FileData::StringMap& doppelganger, bool includefolder) const;

    /*!
     * @brief Search for all games containing 'text' and add them to 'result'
     * @param context Field in which to search text for
     * @param text Test to search for
     * @param results Result list
     * @param remaining Maximum results
     */
    void FastSearch(FastSearchContext context, const std::string& text, ResultList& results, int& remaining) const;
};


