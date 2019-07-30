#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <boost/filesystem.hpp>
#include <components/IList.h>
#include "MetadataDescriptor.h"
#include "ItemType.h"

// Bitflag operator for use on strongly-typed enums - Pass enum type and underlying cardinal type
#define DEFINE_BITFLAG_ENUM(enumtype, ut) \
    inline enumtype operator  |  (enumtype lhs, enumtype rhs)  { return (enumtype)((ut)lhs | (ut)rhs); } \
    inline enumtype operator  &  (enumtype lhs, enumtype rhs)  { return (enumtype)((ut)lhs & (ut)rhs); } \
    inline enumtype operator  ^  (enumtype lhs, enumtype rhs)  { return (enumtype)((ut)lhs ^ (ut)rhs); } \
    inline enumtype operator  ~  (enumtype lhs)                { return (enumtype)(~(ut)lhs); } \
    inline enumtype& operator |= (enumtype& lhs, enumtype rhs) { lhs = (enumtype)((ut)lhs | (ut)rhs); return lhs; } \
    inline enumtype& operator &= (enumtype& lhs, enumtype rhs) { lhs = (enumtype)((ut)lhs & (ut)rhs); return lhs; } \
    inline enumtype& operator ^= (enumtype& lhs, enumtype rhs) { lhs = (enumtype)((ut)lhs ^ (ut)rhs); return lhs; } \
    inline bool operator      == (enumtype lhs, ut rhs)        { return (ut)lhs == rhs; } \
    inline bool operator      != (enumtype lhs, ut rhs)        { return (ut)lhs != rhs; }

// Forward declarations
class SystemData;
class FolderData;

// A tree node that holds information for a file.
class FileData
{
  public:
    typedef std::unordered_map<std::string, FileData*> StringMap;
    typedef std::vector<FileData*> List;

    //! Game filters
    enum class Filter
    {
      None     = 0, //!< Include nothing
      Normal   = 1, //!< Include normal files (not hidden, not favorite)
      Favorite = 2, //!< Include favorites
      Hidden   = 4, //!< Include hidden
      All      = 7, //!< Include all
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
    //! System the current item is attached to
    SystemData* mSystem;
    //! Parent folder
    FolderData* mParent;
    //! Item type - Const ensure mType cannot be modified after being set by the constructor, so that it's alays safe to use c-style cast for FolderData sub-class.
    const ItemType mType;

  private:
    //! Item path on the filesystem
    boost::filesystem::path mPath;
    //! Metadata
    MetadataDescriptor mMetadata;

  protected:
    /*!
     * Constructor for subclasses only
     * @param type
     * @param path Item path
     * @param system Parent system
     */
    FileData(ItemType type, const boost::filesystem::path& path, SystemData* system);

  public:
    /*!
     * Constructor
     * @param path Item path on filesystem
     * @param system system to attach to
     */
    FileData(const boost::filesystem::path& path, SystemData* system);

    /*
     * Getters
     */

    inline const std::string& getName() const { return mMetadata.Name(); }
    inline const std::string getHash() const { return mMetadata.RomCrc32AsString(); }
    inline ItemType getType() const { return mType; }
    inline const boost::filesystem::path& getPath() const { return mPath; }
    inline FolderData* getParent() const { return mParent; }
    inline SystemData* getSystem() const { return mSystem; }

    /*
     * Booleans
     */

    inline bool isEmpty() const { return mType == ItemType::Empty; }
    inline bool isGame() const { return mType == ItemType::Game; }
    inline bool isFolder() const { return mType == ItemType::Folder; }

    /*
     * Setters
     */

    inline void setParent(FolderData* parent) { mParent = parent; }

    /*!
     * Get Thumbnail path if there is one, or Image path.
     * @return file path (may be empty)
     */
    inline const std::string& getThumbnailOrImagePath() const { return mMetadata.Thumbnail().empty() ? mMetadata.Image() : mMetadata.Thumbnail(); }

    /*!
     * Return true if at least one image is available (thumbnail or regular image)
     * @return Boolean result
     */
    inline bool hasThumbnailOrImage() const { return !(mMetadata.Thumbnail().empty() & mMetadata.Image().empty()); }

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
     * Get clean default name, by reoving parenthesis and braqueted words
     * @return Clean name
     */
    std::string getScrappableName() const;

    /*!
     * Get smart default name, when available, depending of the file/folder name
     * Mainly used to get smart naming from arcade zipped roms
     * @return Smart name of the current item, or file/folder name
     */
    std::string getDisplayName() const;
};

DEFINE_BITFLAG_ENUM(FileData::Filter, int)
DEFINE_BITFLAG_ENUM(FileData::SearchAttributes, int)