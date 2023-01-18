#pragma once

#include <vector>
#include <utils/storage/HashMap.h>
#include <string>
#include <utils/os/fs/Path.h>
#include "MetadataDescriptor.h"
#include "ItemType.h"
#include "utils/cplusplus/Bitflags.h"

// Forward declarations
class SystemData;
class FolderData;
class RootFolderData;

// A tree node that holds information for a file.
class FileData
{
  public:
    typedef HashMap<std::string, FileData*> StringMap;
    typedef std::vector<FileData*> List;
    typedef std::vector<const FileData*> ConstList;
    typedef int (*Comparer)(const FileData& a, const FileData& b);

    //! Game filters
    enum class Filter
    {
      None     = 0, //!< Include nothing
      Normal   = 1, //!< Include normal files (not hidden, not favorite)
      Favorite = 2, //!< Include favorites
      Hidden   = 4, //!< Include hidden
      Adult    = 8,
      NotLatest= 16,
      NoGame   = 32,
      PreInstalled = 64,
      All      = 127, //!< Include all
    };

    //! Search attribute enumeration
    enum class SearchAttributes
    {
      ByName        = 1, //!< Search by name, excluding extension
      ByNameWithExt = 2, //!< Search by name, including extension
      ByHash        = 4, //!< Search by hash
      All           = 7, //!< All attributes
    };

  protected:
    //! Top ancestor (link to system)
    RootFolderData& mTopAncestor;
    //! Parent folder
    FolderData* mParent;
    //! Item type - Const ensure mType cannot be modified after being set by the constructor, so that it's alays safe to use c-style cast for FolderData sub-class.
    const ItemType mType;

    /*!
     * Constructor for subclasses only
     * @param type
     * @param path Item path
     * @param system Parent system
     */
    FileData(ItemType type, const Path& path, RootFolderData& ancestor);

  private:
    //! Metadata
    MetadataDescriptor mMetadata;

  public:
    /*!
     * Constructor
     * @param path Item path on filesystem
     * @param system system to attach to
     */
    FileData(const Path& path, RootFolderData& ancestor);

    /*
     * Getters
     */

    inline std::string Name() const { return mMetadata.Name(); }
    inline std::string Hash() const { return mMetadata.RomCrc32AsString(); }
    inline ItemType Type() const { return mType; }
    inline Path RomPath() const { return mMetadata.Rom(); }
    inline FolderData* Parent() const { return mParent; }
    inline RootFolderData& TopAncestor() const { return mTopAncestor; }
    SystemData& System() const;

    /*
     * Booleans
     */

    inline bool IsEmpty() const { return mType == ItemType::Empty; }
    inline bool IsGame() const { return mType == ItemType::Game; }
    inline bool IsFolder() const { return mType == ItemType::Folder || mType == ItemType::Root; }
    inline bool IsRoot() const { return mType == ItemType::Root; }
    inline bool IsTopMostRoot() const { return mType == ItemType::Root && mParent == nullptr; }

    /*
     * Setters
     */

    inline void SetParent(FolderData* parent) { mParent = parent; }

    /*!
     * Get Thumbnail path if there is one, or Image path.
     * @return file path (may be empty)
     */
    inline Path ThumbnailOrImagePath() const { return mMetadata.HasThumnnail() ? mMetadata.Thumbnail() : mMetadata.Image(); }

    /*!
     * Return true if at least one image is available (thumbnail or regular image)
     * @return Boolean result
     */
    inline bool HasThumbnailOrImage() const { return (mMetadata.HasThumnnail() || mMetadata.HasImage()); }

    /*!
     * const Metadata accessor for Read operations
     * @return const Metadata object
     */
    const MetadataDescriptor& Metadata() const { return mMetadata; }

    /*!
     * Metadata accessor for Write operations only
     * @return Writable Metadata object
     */
    MetadataDescriptor& Metadata() { return mMetadata; }

    /*!
     * Get smart default name, when available, depending of the file/folder name
     * Mainly used to get smart naming from arcade zipped roms
     * @return Smart name of the current item, or file/folder name
     */
    std::string DisplayName(const Path& romPath) const;

    /*!
     * @brief Get Pad2Keyboard configuration file path
     * @return Pad2Keyboard configuration file path
     */
    Path P2KPath() const { Path p(RomPath()); return p.ChangeExtension(p.Extension() + ".p2k.cfg"); }

    /*!
     * @brief Get recalbox.conf configuration file path
     * @return recalbox.conf configuration file path
     */
    Path RecalboxConfPath() const { Path p(RomPath()); return p.ChangeExtension(p.Extension() + ".recalbox.conf"); }

      /*!
       * @brief Check if Pad2Keyboard configuration file exists
       * @return Trie if the Pad2Keyboard configuration file exists
       */
    bool HasP2K() const;

    /*!
     * @brief Calculate rom CRC32 and store it in metadata
     * @return This
     */
    FileData& CalculateHash();

    /*!
     * @brief Get region string
     * @return
     */
    std::string Regions();

    /*!
     * @brief Check if file data can be displayable
     * @return displayable state
     */
    bool IsDisplayable() const;

    /*!
     * @brief Check if file data is not a game
     * @return no game state
     */
    bool IsNoGame() const{
      return Strings::StartsWith(Name(), "ZZZ") || Strings::Contains(RomPath().ToString(), "[BIOS]");
    }

    /*!
     * @brief Check if file data is preinstalled game
     * @return is preinstalled state
     */
    bool IsPreinstalled() const{
      return Strings::Contains(RomPath().ToString(), "share_init");
    }

    /*!
     * @brief Update metadata from the given FileData.
     * This method update only if both rom path are equals!
     * @param from Source data
     * @return True if metadata have been updated, false otherwise
     */
    bool UpdateMetadataFrom(FileData& from);

    /*!
     * @brief Check if rom path equals the rom path of the given filedata
     * @param other Other filedata to compare rom path
     * @return True if rom path are equal, false otherwise
     */
    bool AreRomEqual(const FileData& other) { return mMetadata.AreRomEqual(other.mMetadata); }

    std::string DisplayableName() const;
};

DEFINE_BITFLAG_ENUM(FileData::Filter, int)
DEFINE_BITFLAG_ENUM(FileData::SearchAttributes, int)