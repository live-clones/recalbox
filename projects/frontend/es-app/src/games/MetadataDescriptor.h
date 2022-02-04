#pragma once

#include <utils/os/fs/Path.h>
#include <utils/Xml.h>
#include <utils/datetime/DateTime.h>
#include <utils/Strings.h>
#include <games/classifications/Regions.h>
#include "ItemType.h"
#include "games/classifications/Genres.h"
#include "MetadataStringHolder.h"

//#define _METADATA_STATS_

// Forward declaration (MetadataFieldDescriptor must include MetadataDescriptor)
class MetadataFieldDescriptor;

class MetadataDescriptor
{
  private:
    //! Default value storage for fast default detection
    static MetadataDescriptor sDefault;

    #ifdef _METADATA_STATS_
    static int LivingClasses;
    static int LivingFolders;
    static int LivingGames;
    #endif

    // Static default values
    static const std::string DefaultValueRatio;
    static const std::string DefaultValueEmpty;
    static const std::string DefaultValueRating;
    static const std::string DefaultValuePlayers;
    static const std::string DefaultValuePlayCount;
    static const std::string DefaultValueFavorite;
    static const std::string DefaultValueHidden;

    //! Game node <game></game>
    static const std::string GameNodeIdentifier;
    //! Folder node <folder></folder>
    static const std::string FolderNodeIdentifier;

    //! Game name string holder
    static MetadataStringHolder sNameHolder;
    //! Description string holder
    static MetadataStringHolder sDescriptionHolder;
    //! Developer string holder
    static MetadataStringHolder sDeveloperHolder;
    //! Publisher string holder
    static MetadataStringHolder sPublisherHolder;
    //! Publisher string holder
    static MetadataStringHolder sGenreHolder;
    //! Publisher string holder
    static MetadataStringHolder sEmulatorHolder;
    //! Publisher string holder
    static MetadataStringHolder sCoreHolder;
    //! Publisher string holder
    static MetadataStringHolder sRatioHolder;
    //! Path string holder
    static MetadataStringHolder sPathHolder;
    //! File string holder
    static MetadataStringHolder sFileHolder;

    // Please keep field ordered by type size to reduce alignment padding
    MetadataStringHolder::Index32 mName;         //!< Name as simple string
    MetadataStringHolder::Index32 mDescription;  //!< Description, multiline text
    MetadataStringHolder::Index32 mImageFile;    //!< Image file
    MetadataStringHolder::Index32 mThumbnailFile;//!< Thumbnail file
    MetadataStringHolder::Index32 mVideoFile;    //!< Video file
    float                         mRating;       //!< Rating from 0.0 to 1.0
    unsigned int                  mReleaseDate;  //!< Release data (epoch)
    unsigned int                  mLastPlayed;   //!< Last time played (epoch)
    MetadataStringHolder::Index32 mGenre;        //!< Genres, comma separated
    MetadataStringHolder::Index32 mDeveloper;    //!< Developer name index
    MetadataStringHolder::Index32 mPublisher;    //!< Publisher name index
    MetadataStringHolder::Index32 mRegion;       //!< Rom/Game Region
    int                           mPlayers;      //!< Players range: LSW:from - MSW:to (allow sorting by max players)
    int                           mRomCrc32;     //!< Rom Crc32
    MetadataStringHolder::Index16 mImagePath;    //!< Image path
    MetadataStringHolder::Index16 mThumbnailPath;//!< Thumbnail path
    MetadataStringHolder::Index16 mVideoPath;    //!< Video path
    MetadataStringHolder::Index16 mEmulator;     //!< Specific emulator
    MetadataStringHolder::Index16 mCore;         //!< Specific core
    short                         mPlaycount;    //!< Play counter
    GameGenres                    mGenreId;      //!< Normalized Genre
    MetadataStringHolder::Index8  mRatio;        //!< Specific screen ratio
    ItemType                      mType:4;       //!< Metadata type
    bool                          mFavorite:1;   //!< Favorite game
    bool                          mHidden:1;     //!< Hidden game
    bool                          mAdult:1;      //!< Adult state
    bool                          mDirty:1;      //!< Dirty flag (modified data flag)

    /*!
     * Build an empty object filled with default values
     * @return Object filled with default values
     */
    static MetadataDescriptor BuildDefaultValueMetadataDescriptor();

    /*!
     * Return the first static internal field descriptor reference
     * @param Type ObjectType from which to retrieve
     * @param count Number of actual field descriptor available
     * @return first static internal field descriptor reference
     */
    static const MetadataFieldDescriptor* GetMetadataFieldDescriptors(ItemType type, int& count);

    /*!
     * Convert integer range to string: low-high
     * @param range Integer range: Highest into MSB, Lowest into LSB
     * @return Converted string
     */
    static std::string IntToRange(int range);
    /*!
     * Convert a range X-Y to an int: Highest into MSB, Lowest into LSB (allow sorting by highest value)
     * @param range Range string
     * @param to destination int
     * @return True if the operation is successful. False otherwise.
     */
    static bool RangeToInt(const std::string& range, int& to);
    /*!
     * Convert int32 to Hexadecimal string
     * @param from Int32 value to convert to string
     * @param to Hexadecimal result string
     * @return True if the operation is successful. False otherwise.
     */
    static bool IntToHex(int from, std::string& to);
    /*!
     * Convert Hexa string into int32
     * @param from Hexadecimal string
     * @param to Target int32
     * @return True if the operation is successful. False otherwise.
     */
    static bool HexToInt(const std::string& from, int& to);
    /*!
     * Fast string to int conversion
     * @param from source string
     * @param to destination int
     * @param offset offset in source string
     * @param stop Stop char
     * @return True if the operation is successful. False otherwise.
     */
    static bool StringToInt(const std::string& from, int& to, int offset, char stop);
    /*!
     * Fast string to int conversion
     * @param from source string
     * @param to destination int
     * @return True if the operation is successful. False otherwise.
     */
    static bool StringToInt(const std::string& from, int& to);
    /*!
     * Fast string to float conversion
     * @param from source string
     * @param to destination float
     * @return True if the operation is successful. False otherwise.
     */
    static bool StringToFloat(const std::string& from, float& to);

  public:
    /*!
     * Destructor
     */
    ~MetadataDescriptor() = default;

    /*!
     * @brief Cleanup temporary data
     * @return
     */
    static void CleanupHolders();

    /*!
     * Default constructor
     */
    explicit MetadataDescriptor(const std::string& defaultName, ItemType type)
      : mName(-1)
      , mDescription(-1)
      , mImageFile(-1)
      , mThumbnailFile(-1)
      , mVideoFile(-1)
      , mRating(0.0f)
      , mReleaseDate(0)
      , mLastPlayed(0)
      , mGenre(-1)
      , mDeveloper(-1)
      , mPublisher(-1)
      , mRegion(0)
      , mPlayers((1 << 16) + 1)
      , mRomCrc32(0)
      , mImagePath(-1)
      , mThumbnailPath(-1)
      , mVideoPath(-1)
      , mEmulator(-1)
      , mCore(-1)
      , mPlaycount(0)
      , mGenreId(GameGenres::None)
      , mRatio(-1)
      , mType(type)
      , mFavorite(false)
      , mHidden(false)
      , mAdult(false)
      , mDirty(false)
    {
      SetName(defaultName);
      #ifdef _METADATA_STATS_
      LivingClasses++;
      if (_Type == ItemType::Game) LivingGames++;
      if (_Type == ItemType::Folder) LivingFolders++;
      #endif
    }

    /*!
     * Move constructor
     * @param source  Source to move data from
     */
    MetadataDescriptor(const MetadataDescriptor& source) noexcept
    #ifdef _METADATA_STATS_
      : mName(source.mName),
        mDescription(source.mDescription),
        mImageFile(source.mImageFile),
        mThumbnailFile(source.mThumbnailFile),
        mVideoFile(source.mVideoFile),
        mRating(source.mRating),
        mReleaseDate(source.mReleaseDate),
        mLastPlayed(source.mLastPlayed),
        mGenre(source.mGenre),
        mDeveloper(source.mDeveloper),
        mPublisher(source.mPublisher),
        mRegion(source.mRegion),
        mPlayers(source.mPlayers),
        mRomCrc32(source.mRomCrc32),
        mImagePath(source.mImagePath),
        mThumbnailPath(source.mThumbnailPath),
        mVideoPath(source.mVideoPath),
        mEmulator(source.mEmulator),
        mCore(source.mCore),
        mPlaycount(source.mPlaycount),
        mGenreId(source.mGenreId),
        mRatio(source.mRatio),
        mFavorite(source.mFavorite),
        mHidden(source.mHidden),
        mAdult(source.mAdult),
        mDirty(source.mDirty),
        mType(source.mType)
    {
      LivingClasses++;
      if (_Type == ItemType::Game) LivingGames++;
      if (_Type == ItemType::Folder) LivingFolders++;
    }
    #else
      = default;
    #endif

    /*!
     * Assignment operator - Required by STL objects since a copy operator is defined
     * @param source Source to copy data from
     */
    MetadataDescriptor& operator = (const MetadataDescriptor& source)
    {
      if (&source == this) return *this;

      #ifdef _METADATA_STATS_
      LivingClasses--;
      if (_Type == ItemType::Game) LivingGames--;
      if (_Type == ItemType::Folder) LivingFolders--;
      #endif

      mName          = source.mName         ;
      mDescription   = source.mDescription  ;
      mImagePath     = source.mImagePath    ;
      mImageFile     = source.mImageFile    ;
      mThumbnailPath = source.mThumbnailPath;
      mThumbnailFile = source.mThumbnailFile;
      mVideoPath     = source.mVideoPath    ;
      mVideoFile     = source.mVideoFile    ;
      mGenre         = source.mGenre        ;
      mEmulator      = source.mEmulator     ;
      mCore          = source.mCore         ;
      mRatio         = source.mRatio        ;
      mDeveloper     = source.mDeveloper    ;
      mPublisher     = source.mPublisher    ;
      mRegion        = source.mRegion       ;
      mRating        = source.mRating       ;
      mGenreId       = source.mGenreId      ;
      mPlayers       = source.mPlayers      ;
      mReleaseDate   = source.mReleaseDate  ;
      mPlaycount     = source.mPlaycount    ;
      mLastPlayed    = source.mLastPlayed   ;
      mRomCrc32      = source.mRomCrc32     ;
      mFavorite      = source.mFavorite     ;
      mHidden        = source.mHidden       ;
      mAdult         = source.mAdult        ;
      mDirty         = source.mDirty        ;
      mType          = source.mType         ;

      #ifdef _METADATA_STATS_
      if (_Type == ItemType::Game) LivingGames++;
      if (_Type == ItemType::Folder) LivingFolders++;
      #endif

      return *this;
    }

    /*!
     * Move operator
     * @param source Source to move data from
     */
    MetadataDescriptor& operator = (MetadataDescriptor&& source) noexcept
    {
      #ifdef _METADATA_STATS_
      LivingClasses--;
      if (_Type == ItemType::Game) LivingGames--;
      if (_Type == ItemType::Folder) LivingFolders--;
      #endif

      mName          = source.mName         ;
      mDescription   = source.mDescription  ;
      mImagePath     = source.mImagePath    ;
      mImageFile     = source.mImageFile    ;
      mThumbnailPath = source.mThumbnailPath;
      mThumbnailFile = source.mThumbnailFile;
      mVideoPath     = source.mVideoPath    ;
      mVideoFile     = source.mVideoFile    ;
      mEmulator      = source.mEmulator     ;
      mCore          = source.mCore         ;
      mRatio         = source.mRatio        ;
      mGenre         = source.mGenre        ;
      mDeveloper     = source.mDeveloper    ;
      mPublisher     = source.mPublisher    ;
      mRating        = source.mRating       ;
      mGenreId       = source.mGenreId      ;
      mPlayers       = source.mPlayers      ;
      mReleaseDate   = source.mReleaseDate  ;
      mPlaycount     = source.mPlaycount    ;
      mLastPlayed    = source.mLastPlayed   ;
      mRomCrc32      = source.mRomCrc32     ;
      mFavorite      = source.mFavorite     ;
      mHidden        = source.mHidden       ;
      mAdult         = source.mAdult        ;
      mDirty         = source.mDirty        ;
      mType          = source.mType         ;

      #ifdef _METADATA_STATS_
      if (_Type == ItemType::Game) LivingGames++;
      if (_Type == ItemType::Folder) LivingFolders++;
      #endif

      return *this;
    }

    /*!
     * Deserialize data from a given Xml node
     * @param from XML Node to deserialize from
     * @param relativeTo Root path
     * @return True of the node has been successfully deserialized
     */
    bool Deserialize(XmlNode from, const Path& relativeTo);

    /*!
     * Serialize internal data to XML node
     * @param relativeTo Root path
     * @return Serialized XML node
     */
    void Serialize(XmlNode parentTree, const Path& filePath, const Path& relativeTo) const;

    /*!
     * Merge value from the source metadata object into the current object
     * current fields ate replaced only if they have their default value.
     * @param source Metadata object from which to merge data
     */
    void Merge(const MetadataDescriptor& source);

    /*
     * Accessors
     */

    ItemType Type() const { return mType; }

    std::string  Name()        const { return sNameHolder.GetString(mName);                 }
    std::string  Description() const { return sDescriptionHolder.GetString(mDescription);   }
    Path         Image()       const { return Path(sPathHolder.GetString(mImagePath)) / sFileHolder.GetString(mImageFile); }
    Path         Thumbnail()   const { return Path(sPathHolder.GetString(mThumbnailPath)) / sFileHolder.GetString(mThumbnailFile); }
    Path         Video()       const { return Path(sPathHolder.GetString(mVideoPath)) / sFileHolder.GetString(mVideoFile); }
    std::string  Developer()   const { return sDeveloperHolder.GetString(mDeveloper);       }
    std::string  Publisher()   const { return sPublisherHolder.GetString(mPublisher);       }
    std::string  Genre()       const { return sGenreHolder.GetString(mGenre);               }
    std::string  Emulator()    const { return sEmulatorHolder.GetString(mEmulator);         }
    std::string  Core()        const { return sCoreHolder.GetString(mCore);                 }
    std::string  Ratio()       const { return sRatioHolder.GetString(mRatio);               }

    float              Rating()          const { return mRating;                           }
    int                PlayerRange()     const { return mPlayers;                          }
    int                PlayerMax()       const { return mPlayers >> 16;                    }
    int                PlayerMin()       const { return mPlayers & 0xFFFF;                 }
    unsigned int       ReleaseDateEpoc() const { return mReleaseDate;                      }
    DateTime           ReleaseDate()     const { return DateTime((long long)mReleaseDate); }
    int                PlayCount()       const { return mPlaycount;                        }
    unsigned int       LastPlayedEpoc()  const { return mLastPlayed;                       }
    DateTime           LastPlayed()      const { return DateTime((long long)mLastPlayed);  }
    unsigned int       Region()          const { return mRegion;                           }
    int                RomCrc32()        const { return mRomCrc32;                         }
    bool               Favorite()        const { return mFavorite;                         }
    bool               Hidden()          const { return mHidden;                           }
    bool               Adult()           const { return mAdult;                            }
    GameGenres         GenreId()         const { return mGenreId;                          }

    /*
     * Validators
     */

    bool HasImage()     const { return (mImagePath     | mImageFile    ) >= 0; }
    bool HasThumnnail() const { return (mThumbnailPath | mThumbnailFile) >= 0; }
    bool HasVideo()     const { return (mVideoPath     | mVideoFile    ) >= 0; }

    /*
     * String accessors
     */

    std::string NameAsString()        const { return sNameHolder.GetString(mName);                 }
    std::string EmulatorAsString()    const { return sEmulatorHolder.GetString(mEmulator);         }
    std::string CoreAsString()        const { return sCoreHolder.GetString(mCore);                 }
    std::string RatioAsString()       const { return sRatioHolder.GetString(mRatio);               }
    std::string DescriptionAsString() const { return sDescriptionHolder.GetString(mDescription);   }
    std::string ImageAsString()       const { return (Path(sPathHolder.GetString(mImagePath)) / sFileHolder.GetString(mImageFile)).ToString(); }
    std::string ThumbnailAsString()   const { return (Path(sPathHolder.GetString(mThumbnailPath)) / sFileHolder.GetString(mThumbnailFile)).ToString(); }
    std::string VideoAsString()       const { return (Path(sPathHolder.GetString(mVideoPath)) / sFileHolder.GetString(mVideoFile)).ToString(); }
    std::string DeveloperAsString()   const { return sDeveloperHolder.GetString(mDeveloper);       }
    std::string PublisherAsString()   const { return sPublisherHolder.GetString(mPublisher);       }
    std::string GenreAsString()       const { return sGenreHolder.GetString(mGenre);               }
    std::string RegionAsString()      const { return Regions::Serialize4Regions(mRegion);          }

    std::string RatingAsString()      const { return Strings::ToString(mRating, 2);                        }
    std::string PlayersAsString()     const { return IntToRange(mPlayers);                                 }
    std::string ReleaseDateAsString() const { return mReleaseDate != 0 ? DateTime((long long)mReleaseDate).ToCompactISO8601() : ""; }
    std::string PlayCountAsString()   const { return Strings::ToString(mPlaycount);                           }
    std::string LastPlayedAsString()  const { return mLastPlayed != 0 ? DateTime((long long)mLastPlayed).ToCompactISO8601() : ""; }
    std::string FavoriteAsString()    const { return mFavorite ? "true" : "false";                         }
    std::string RomCrc32AsString()    const { std::string r; IntToHex(mRomCrc32, r); return r;             }
    std::string HiddenAsString()      const { return mHidden ? "true" : "false";                           }
    std::string AdultAsString()       const { return mAdult ? "true" : "false";                            }
    std::string GenreIdAsString()     const { return Strings::ToString((int)mGenreId);                           }

    /*
     * Setters
     */

    void SetImagePath(const Path& image)
    {
      mImagePath = sPathHolder.AddString16(image.Directory().ToString());
      mImageFile = sFileHolder.AddString32(image.Filename());
      mDirty = true;
    }
    void SetThumbnailPath(const Path& thumbnail)
    {
      mThumbnailPath = sPathHolder.AddString16(thumbnail.Directory().ToString());
      mThumbnailFile = sFileHolder.AddString32(thumbnail.Filename());
      mDirty = true;
    }
    void SetVideoPath(const Path& video)
    {
      mVideoPath = sPathHolder.AddString16(video.Directory().ToString());
      mVideoFile = sFileHolder.AddString32(video.Filename());
      mDirty = true;
    }
    void SetEmulator(const std::string& emulator)       { mEmulator    = sEmulatorHolder.AddString16(emulator);       mDirty = true; }
    void SetCore(const std::string& core)               { mCore        = sCoreHolder.AddString16(core);               mDirty = true; }
    void SetRatio(const std::string& ratio)             { mRatio       = sRatioHolder.AddString8(ratio);              mDirty = true; }
    void SetGenre(const std::string& genre)             { mGenre       = sGenreHolder.AddString32(genre);             mDirty = true; }
    void SetName(const std::string& name)               { mName        = sNameHolder.AddString32(name);               mDirty = true; }
    void SetDescription(const std::string& description) { mDescription = sDescriptionHolder.AddString32(description); mDirty = true; }
    void SetReleaseDate(const DateTime& releasedate)    { mReleaseDate = (int)releasedate.ToEpochTime();              mDirty = true; }
    void SetDeveloper(const std::string& developer)     { mDeveloper   = sDeveloperHolder.AddString32(developer);     mDirty = true; }
    void SetPublisher(const std::string& publisher)     { mPublisher   = sPublisherHolder.AddString32(publisher);     mDirty = true; }
    void SetRating(float rating)                        { mRating      = rating;                                      mDirty = true; }
    void SetPlayers(int min, int max)                   { mPlayers     = (max << 16) + min;                           mDirty = true; }
    void SetRegion(int regions)                         { mRegion      = regions;                                     mDirty = true; }
    void SetRomCrc32(int romcrc32)                      { mRomCrc32    = romcrc32;                                    mDirty = true; }
    void SetFavorite(bool favorite)                     { mFavorite    = favorite;                                    mDirty = true; }
    void SetHidden(bool hidden)                         { mHidden      = hidden;                                      mDirty = true; }
    void SetAdult(bool adult)                           { mAdult       = adult;                                       mDirty = true; }
    void SetGenreId(GameGenres genre)                   { mGenreId     = genre;                                       mDirty = true; }

    // Special setter to force dirty
    void SetDirty() { mDirty = true; }

    /*
     * Volatile setters - do not set the Dirty flag for auto-saving
     */

    void SetVolatileDescription(const std::string& description) { bool dirty = mDirty; SetDescription(description); mDirty = dirty; }
    void SetVolatileImagePath(const Path& image) { bool dirty = mDirty; SetImagePath(image); mDirty = dirty; }

    /*
     * String setters
     */

    void SetImagePathAsString(const std::string& image)         { SetImagePath(Path(image));         }
    void SetThumbnailPathAsString(const std::string& thumbnail) { SetThumbnailPath(Path(thumbnail)); }
    void SetVideoPathAsString(const std::string& video)         { SetVideoPath(Path(video));         }
    void SetReleaseDateAsString(const std::string& releasedate)
    {
      DateTime st;
      mReleaseDate = DateTime::FromCompactISO6801(releasedate, st) ? (int)st.ToEpochTime() : 0;
      mDirty = true;
    }
    void SetLastPlayedAsString(const std::string& lastplayed)
    {
      DateTime st;
      mLastPlayed = DateTime::FromCompactISO6801(lastplayed, st) ? (int)st.ToEpochTime() : 0;
      mDirty = true;
    }
    void SetRatingAsString(const std::string& rating)           { float f = 0.0f; if (StringToFloat(rating, f)) SetRating(f);              }
    void SetPlayersAsString(const std::string& players)         { if (!RangeToInt(players, mPlayers)) SetPlayers(1, 1);                    }
    void SetFavoriteAsString(const std::string& favorite)       { SetFavorite(favorite == "true");                                         }
    void SetHiddenAsString(const std::string& hidden)           { SetHidden(hidden == "true");                                             }
    void SetAdultAsString(const std::string& adult)             { SetAdult(adult == "true");                                             }
    void SetRomCrc32AsString(const std::string& romcrc32)       { int c = 0; if (HexToInt(romcrc32, c)) SetRomCrc32(c);                        }
    void SetPlayCountAsString(const std::string& playcount)     { int p = 0; if (StringToInt(playcount, p)) { mPlaycount = (short)p; mDirty = true; } }
    void SetGenreIdAsString(const std::string& genre)           { int g = 0; if (StringToInt(genre, g)) { mGenreId = (GameGenres)g; mDirty = true; } }
    void SetRegionAsString(const std::string& region)           { mRegion = (int)Regions::Deserialize4Regions(region); mDirty = true; }

    /*
     * Defaults
     */

    bool IsDefaultName()            const { return sDefault.mName == mName;               }
    bool IsDefaultEmulator()        const { return sDefault.Emulator() == Emulator();     }
    bool IsDefaultCore()            const { return sDefault.Core() == Core();             }
    bool IsDefaultRatio()           const { return sDefault.Ratio() == Ratio();           }
    bool IsDefaultDescription()     const { return sDefault.mDescription == mDescription; }
    bool IsDefaultImage()           const { return sDefault.Image() == Image();           }
    bool IsDefaultThumbnail()       const { return sDefault.Thumbnail() == Thumbnail();   }
    bool IsDefaultVideo()           const { return sDefault.Video() == Video();           }
    bool IsDefaultDeveloper()       const { return sDefault.mDeveloper == mDeveloper;     }
    bool IsDefaultPublisher()       const { return sDefault.mPublisher == mPublisher;     }
    bool IsDefaultGenre()           const { return sDefault.mGenre == mGenre;             }
    bool IsDefaultRegion()          const { return sDefault.Region() == Region();         }
    bool IsDefaultRating()          const { return sDefault.mRating == mRating;           }
    bool IsDefaultPlayerRange()     const { return sDefault.mPlayers == mPlayers;         }
    bool IsDefaultReleaseDateEpoc() const { return sDefault.mReleaseDate == mReleaseDate; }
    bool IsDefaultPlayCount()       const { return sDefault.mPlaycount == mPlaycount;     }
    bool IsDefaultLastPlayedEpoc()  const { return sDefault.mLastPlayed == mLastPlayed;   }
    bool IsDefaultRomCrc32()        const { return sDefault.mRomCrc32 == mRomCrc32;       }
    bool IsDefaultFavorite()        const { return sDefault.mFavorite == mFavorite;       }
    bool IsDefaultHidden()          const { return sDefault.mHidden == mHidden;           }
    bool IsDefaultAdult()           const { return sDefault.mAdult == mAdult;             }
    bool IsDefaultGenreId()         const { return sDefault.mGenreId == mGenreId;         }

    /*
     * Convenient Accessors
     */

    bool IsDirty()  const { return mDirty; }

    /*
     * Special modifiers
     */

    void IncPlaycount() { mPlaycount++; mDirty = true; }
    void SetLastplayedNow() { mLastPlayed = (unsigned int)DateTime().ToEpochTime(); mDirty = true; }

    /*
     * Metadata FieldManagement Methods
     */

    /*!
     * Return the first static internal field descriptor reference
     * @param count Number of actual field descriptor available
     * @return first static internal field descriptor reference
     */
    const MetadataFieldDescriptor* GetMetadataFieldDescriptors(int& count) { return GetMetadataFieldDescriptors(mType, count); }

    /*
     * Index accessors
     */

    MetadataStringHolder::Index32 FileIndex() const { return -1; }
    MetadataStringHolder::Index32 NameIndex() const { return mName; }
    MetadataStringHolder::Index32 DescriptionIndex() const { return mDescription; }
    MetadataStringHolder::Index32 DeveloperIndex() const { return mDeveloper; }
    MetadataStringHolder::Index32 PublisherIndex() const { return mPublisher; }

    static int FileIndexCount() { return sFileHolder.ObjectCount(); }
    static int NameIndexCount() { return sNameHolder.ObjectCount(); }
    static int DescriptionIndexCount() { return sDescriptionHolder.ObjectCount(); }
    static int DeveloperIndexCount() { return sDeveloperHolder.ObjectCount(); }
    static int PublisherIndexCount() { return sPublisherHolder.ObjectCount(); }

    /*
     * Search
     */

    bool IsMatchingFileIndex(MetadataStringHolder::Index32 index) const { (void)index; return false; }
    bool IsMatchingNameIndex(MetadataStringHolder::Index32 index) const { return mName == index; }
    bool IsMatchingDescriptionIndex(MetadataStringHolder::Index32 index) const { return mDescription == index; }
    bool IsMatchingDeveloperIndex(MetadataStringHolder::Index32 index) const { return mDeveloper == index; }
    bool IsMatchingPublisherIndex(MetadataStringHolder::Index32 index) const { return mPublisher == index; }

    /*!
     * @brief Search text in game names
     * @param originaltext Text to search for
     * @param output Result container
     */
    static void SearchInNames(const std::string& originaltext, MetadataStringHolder::FoundTextList& output, int context) { return sNameHolder.FindText(originaltext, output, context); }

    /*!
     * @brief Search text in descriptions
     * @param originaltext Text to search for
     * @param output Result container
     */
    static void SearchInDescription(const std::string& originaltext, MetadataStringHolder::FoundTextList& output, int context) { return sDescriptionHolder.FindText(originaltext, output, context); }

    /*!
     * @brief Search text in developer names
     * @param originaltext Text to search for
     * @param output Result container
     */
    static void SearchInDeveloper(const std::string& originaltext, MetadataStringHolder::FoundTextList& output, int context) { return sDeveloperHolder.FindText(originaltext, output, context); }

    /*!
     * @brief Search text in publisher names
     * @param originaltext Text to search for
     * @param output Result container
     */
    static void SearchInPublisher(const std::string& originaltext, MetadataStringHolder::FoundTextList& output, int context) { return sPublisherHolder.FindText(originaltext, output, context); }

    /*!
     * @brief Search text in file names
     * @param originaltext Text to search for
     * @param output Result container
     */
    static void SearchInPath(const std::string& originaltext, MetadataStringHolder::FoundTextList& output, int context) { (void)originaltext; (void)output; (void)context; }
};

