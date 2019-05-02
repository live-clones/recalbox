#pragma once

#include "FileData.h"

class FolderData : public FileData
{
  private:
    //! Current folder child list
    FileData::List mChildren;

  protected:
    /*!
     * Clear the internal child lists without destroying them.
     * Used by inherited class that store children object without ownership
     */
    void ClearChildList() { mChildren.clear(); }

    /*!
     * Get all items recursively.
     * @param to List to fill
     * @param includes Get only items matching these filters
     * @param includefolders Include folder as regular item, or just get their children
     * @return Total amount of items (not including folders!)
     */
    int getItemsRecursively(FileData::List& to, Filter includes, bool includefolders) const;
    /*!
     * Count all items recursively
     * @param includes Count only items matching these filters
     * @return Total amount of items (not including folders!)
     */
    int countItemsRecursively(Filter includes, bool includefolders) const;

    /*!
     * Get all items recursively.
     * @param to List to fill
     * @param includes Get only items matching these filters
     * @param includefolders Include folder as regular item, or just get their children
     * @return Total amount of items (not including folders!)
     */
    int getItems(FileData::List& to, Filter includes, bool includefolders) const;
    /*!
     * Count all items recursively
     * @param includes Count only items matching these filters
     * @return Total amount of items (not including folders!)
     */
    int countItems(Filter includes, bool includefolders) const;

    /*!
     * Lookup for a given game in the current tree
     * @param gameNameOrHash item to search for, either a hash or a path (w/ or w/o file extension)
     * @param attributes Compare item against hashes, filenames or filenames with extension
     * @param path Current tree path
     * @return First mathing game or nullptr
     */
    FileData* LookupGame(const std::string& item, SearchAttributes attributes, const std::string& path);

    /*!
     * Highly optimized Quicksort, inpired from original Delphi 7 code
     * @param low Lowest element
     * @param high Highest element
     * @param comparer Compare method
     */
    static void QuickSortAscending(FileData::List& items, int low, int high, int (*comparer)(FileData* const a, FileData* const b));

    /*!
     * Highly optimized Quicksort, inpired from original Delphi 7 code
     * @param low Lowest element
     * @param high Highest element
     * @param comparer Compare method
     */
    static void QuickSortDescending(FileData::List& items, int low, int high, int (*comparer)(FileData* const a, FileData* const b));

  public:
    /*!
     * Constructor
     */
    FolderData(const std::string& startpath, SystemData* system)
      : FileData(ItemType::Folder, startpath, system)
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
    void addChild(FileData* file, bool lukeImYourFather);
    /*!
     * Try to remove a child item from the current folder
     * @param file Item to remove
     */
    void removeChild(FileData* file);

    /*!
     * Return true if this FileData is a folder and has at lease one child
     * @return Boolean result
     */
    bool hasChildren() const { return !mChildren.empty(); }

    /*!
     * Run through filesystem's folders, seeking for games, and when found, add them into the current tree
     * @param filteredExtensions Filter files that do not match this extension list (casee sensitive)
     * @param systemData System to attach to
     * @param doppelgangerWatcher Map used to check duplicate games
     */
    void populateRecursiveFolder(const std::string& filteredExtensions, SystemData* systemData, FileData::StringMap& doppelgangerWatcher);

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
    FileData* LookupGame(const std::string& item, SearchAttributes attributes) { return LookupGame(item, attributes, std::string()); }

    /*!
     * Get total games in all folders, incuding hidden
     * @return Game count
     */
    int countAll(bool includefolders) const { return countItemsRecursively(Filter::All, includefolders); }

    /*!
     * Get favorite games in all folders
     * @return Favorite game count
     */
    int countAllFavorites(bool includefolders) const { return countItemsRecursively(Filter::Favorite, includefolders); }

    /*!
     * Get hidden games in all folders
     * @return Hidden game count
     */
    int countAllHidden(bool includefolders) const { return countItemsRecursively(Filter::Hidden, includefolders); }

    /*!
     * Return true if at least one game in the three has more metadata than just path & names
     * @return True if detailed data are available
     */
    bool hasDetailedData() const;

    /*!
     * Return true if the current folder contains the given item
     * @param item item to seek for
     * @param recurse True to search in subfolders
     * @return True if the item has been found
     */
    bool Contains(const FileData* item, bool recurse);

    /*!
     * Quick sort items in the given list.
     * @param items Items to sort
     * @param comparer Comparison function
     * @param ascending True for ascending sort, false for descending.
     */
    static void Sort(FileData::List& items, int (*comparer)(FileData* const a, FileData* const b), bool ascending);

    /*!
     * Count filtered items recursively starting from the current folder
     * @param filters Filter to apply
     * @param includefolders True to include subfolders in the result
     * @return List of filtered items
     */
    int countFilteredItemsRecursively(Filter filters, bool includefolders) const { return countItemsRecursively(filters, includefolders); }
    /*!
     * Count filtered items from the current folder
     * @param filters Filter to apply
     * @param includefolders True to include subfolders in the result
     * @return List of filtered items
     */
    int countFilteredItems(Filter filters, bool includefolders) const { return countItems(filters, includefolders); }
    /*!
     * Count displayable items (normal + favorites) recursively starting from the current folder
     * @param includefolders True to include subfolders in the resulting list
     * @return Number of displayable items
     */
    int countAllDisplayableItemsRecursively(bool includefolders) const { return countItemsRecursively(Filter::Normal | Filter::Favorite, includefolders); }

    /*!
     * Get filtered items recursively starting from the current folder
     * @param filters Filter to apply
     * @param includefolders True to include subfolders in the resulting list
     * @return List of filtered items
     */
    FileData::List getFilteredItemsRecursively(Filter filters, bool includefolders) const;
    /*!
     * Get all items recursively starting from the current folder
     * @param includefolders True to include subfolders in the resulting list
     * @return List of filtered items
     */
    FileData::List getAllItemsRecursively(bool includefolders) const;
    /*!
     * Get all displayable items (normal + favorites) recursively starting from the current folder
     * @param includefolders True to include subfolders in the resulting list
     * @return List of filtered items
     */
    FileData::List getAllDisplayableItemsRecursively(bool includefolders) const;
    /*!
     * Get all favorite items recursively starting from the current folder
     * @param includefolders True to include subfolders in the resulting list
     * @return List of filtered items
     */
    FileData::List getAllFavoritesRecursively(bool includefolders) const;

    /*!
     * Get filtered items from the current folder
     * @param filters Filter to apply
     * @param includefolders True to include subfolders in the resulting list
     * @return List of filtered items
     */
    FileData::List getFilteredItems(Filter filters, bool includefolders) const;
    /*!
     * Get all items from the current folder
     * @param includefolders True to include subfolders in the resulting list
     * @return List of filtered items
     */
    FileData::List getAllItems(bool includefolders) const;
    /*!
     * Get all displayable items (normal + favorites) from the current folder
     * @param includefolders True to include subfolders in the resulting list
     * @return List of filtered items
     */
    FileData::List getAllDisplayableItems(bool includefolders) const;
    /*!
     * Get all favorite items from the current folder
     * @param includefolders True to include subfolders in the resulting list
     * @return List of filtered items
     */
    FileData::List getAllFavorites(bool includefolders) const;
};


