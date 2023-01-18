#pragma once

#include <utils/os/fs/Path.h>
#include <utils/Xml.h>
#include <utils/datetime/DateTime.h>
#include <utils/Strings.h>
#include <games/classifications/Regions.h>
#include "ItemType.h"
#include "games/classifications/Genres.h"
#include "MetadataStringHolder.h"
#include "hardware/RotationType.h"

//#define _METADATA_STATS_

// Forward declaration (MetadataFieldDescriptor must include MetadataDescriptor)
class MetadataFieldDescriptor;

class MetadataDescriptor
{
  private:
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
    static const std::string DefaultValueRotation;

    //! Game node <game></game>
    static const std::string GameNodeIdentifier;
    //! Folder node <folder></folder>
    static const std::string FolderNodeIdentifier;

    //! Game name string holder
    static MetadataStringHolder sNameHolder;

    static MetadataStringHolder sAliasHolder;
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
    //! Path string holder
    static MetadataStringHolder sLastPatchPathHolder;
    //! File string holder
    static MetadataStringHolder sLastPatchFileHolder;
    //! Rotation string holder
    static MetadataStringHolder sRotationHolder;

    // Please keep field ordered by type size to reduce alignment padding
    unsigned int                  mTimeStamp;    //!< Scraping timestamp
    MetadataStringHolder::Index32 mRomFile;      //!< Rom file
    MetadataStringHolder::Index32 mName;         //!< Name as simple string
    MetadataStringHolder::Index32 mAlias;        //!< Alias as simple string
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
    int                           mPlayers;        //!< Players range: LSW:from - MSW:to (allow sorting by max players)
    int                           mRomCrc32;       //!< Rom Crc32
    Regions::RegionPack           mRegion;         //!< Rom/Game Region
    MetadataStringHolder::Index16 mRomPath;        //!< Rom path
    MetadataStringHolder::Index16 mImagePath;      //!< Image path
    MetadataStringHolder::Index16 mThumbnailPath;  //!< Thumbnail path
    MetadataStringHolder::Index16 mVideoPath;      //!< Video path
    MetadataStringHolder::Index16 mEmulator;       //!< Specific emulator
    MetadataStringHolder::Index16 mCore;           //!< Specific core
    MetadataStringHolder::Index16 mLastPatchPath;  //!< Last patch path
    MetadataStringHolder::Index16 mLastPatchFile;  //!< Last patch file
    short                         mPlayCount;      //!< Play counter
    GameGenres                    mGenreId;        //!< Normalized Genre
    MetadataStringHolder::Index8  mRatio;          //!< Specific screen ratio
    ItemType                      mType:4;         //!< Metadata type
    bool                          mFavorite:1;     //!< Favorite game
    bool                          mHidden:1;       //!< Hidden game
    bool                          mAdult:1;        //!< Adult state
    bool                          mLatestVerion:1; //!< Lastest version of this game?
    bool                          mPreinstalled:1; //!< Preinstalled game?
    bool                          mNoGame:1;       //!< ?!
    bool                          mDirty:1;        //!< Dirty flag (modified data flag)
    RotationType                  mRotation:2;     //<! Rotation flag

    //! Default value storage for fast default detection
    static const MetadataDescriptor& Default();

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
    explicit MetadataDescriptor(const Path& path, const std::string& defaultName, ItemType type)
      : mTimeStamp(0)
      , mRomFile(0)
      , mName(0)
      , mAlias(0)
      , mDescription(0)
      , mImageFile(0)
      , mThumbnailFile(0)
      , mVideoFile(0)
      , mRating(0.0f)
      , mReleaseDate(0)
      , mLastPlayed(0)
      , mGenre(0)
      , mDeveloper(0)
      , mPublisher(0)
      , mPlayers((1 << 16) + 1)
      , mRomCrc32(0)
      , mRegion()
      , mRomPath(0)
      , mImagePath(0)
      , mThumbnailPath(0)
      , mVideoPath(0)
      , mEmulator(0)
      , mCore()
      , mLastPatchPath(0)
      , mLastPatchFile(0)
      , mPlayCount(0)
      , mGenreId(GameGenres::None)
      , mRatio(0)
      , mType(type)
      , mFavorite(false)
      , mHidden(false)
      , mAdult(false)
      , mLatestVerion(false)
      , mPreinstalled(false)
      , mNoGame(false)
      , mDirty(false)
      , mRotation(RotationType::None)
    {
      SetRomPath(path);
      SetName(defaultName);
      #ifdef _METADATA_STATS_
      LivingClasses++;
      if (_Type == ItemType::Game) LivingGames++;
      if (_Type == ItemType::Folder) LivingFolders++;
      #endif
      UnsetDirty();
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
        mTimeStamp(source.mTimeStamp),
        mFavorite(source.mFavorite),
        mHidden(source.mHidden),
        mAdult(source.mAdult),
        mLatestVerion(source.mLatestVerion),
        mNoGame(source.mNoGame),
        mDirty(source.mDirty),
        mType(source.mType),
        mRotation(source.mRotation),
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

      mRomFile       = source.mRomFile      ;
      mRomPath       = source.mRomPath      ;
      mName          = source.mName         ;
      mAlias         = source.mAlias        ;
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
      mPlayCount     = source.mPlayCount    ;
      mLastPlayed    = source.mLastPlayed   ;
      mRomCrc32      = source.mRomCrc32     ;
      mFavorite      = source.mFavorite     ;
      mHidden        = source.mHidden       ;
      mAdult         = source.mAdult        ;
      mDirty         = source.mDirty        ;
      mLatestVerion  = source.mLatestVerion ;
      mPreinstalled  = source.mPreinstalled ;
      mNoGame        = source.mNoGame       ;
      mType          = source.mType         ;
      mRotation      = source.mRotation     ;

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

      mRomFile       = source.mRomFile      ;
      mRomPath       = source.mRomPath      ;
      mName          = source.mName         ;
      mAlias         = source.mAlias        ;
      mDescription   = source.mDescription  ;
      mImagePath     = source.mImagePath    ;
      mImageFile     = source.mImageFile    ;
      mThumbnailPath = source.mThumbnailPath;
      mThumbnailFile = source.mThumbnailFile;
      mVideoPath     = source.mVideoPath    ;
      mVideoFile     = source.mVideoFile    ;
      mEmulator      = source.mEmulator     ;
      mCore          = source.mCore         ;
      mLastPatchPath = source.mLastPatchPath;
      mLastPatchFile = source.mLastPatchFile;
      mRatio         = source.mRatio        ;
      mGenre         = source.mGenre        ;
      mDeveloper     = source.mDeveloper    ;
      mPublisher     = source.mPublisher    ;
      mRating        = source.mRating       ;
      mGenreId       = source.mGenreId      ;
      mPlayers       = source.mPlayers      ;
      mReleaseDate   = source.mReleaseDate  ;
      mPlayCount     = source.mPlayCount    ;
      mLastPlayed    = source.mLastPlayed   ;
      mRomCrc32      = source.mRomCrc32     ;
      mFavorite      = source.mFavorite     ;
      mHidden        = source.mHidden       ;
      mAdult         = source.mAdult        ;
      mDirty         = source.mDirty        ;
      mLatestVerion  = source.mLatestVerion ;
      mPreinstalled  = source.mPreinstalled ;
      mNoGame        = source.mNoGame       ;
      mType          = source.mType         ;
      mRotation      = source.mRotation     ;

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
     * @param timestamp last scraping timestamp
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
     * Last scraping Timestamp
     */

    [[nodiscard]] unsigned int TimeStamp() const { return mTimeStamp; }
    void SetTimeStamp() { mTimeStamp = (unsigned int)DateTime().ToEpochTime(); }

    /*
     * Accessors
     */

    [[nodiscard]] ItemType Type() const { return mType; }

    [[nodiscard]] Path         Rom()         const { return sPathHolder.GetPath(mRomPath) / sFileHolder.GetString(mRomFile); }
    [[nodiscard]] Path         RomFileOnly() const { return sFileHolder.GetPath(mRomFile);        }
    [[nodiscard]] std::string  Name()        const { return sNameHolder.GetString(mName);                 }
    [[nodiscard]] std::string  Alias()       const { return sAliasHolder.GetString(mAlias);    }
    [[nodiscard]] std::string  Description() const { return sDescriptionHolder.GetString(mDescription);   }
    [[nodiscard]] Path         Image()       const { return sPathHolder.GetPath(mImagePath) / sFileHolder.GetString(mImageFile); }
    [[nodiscard]] Path         Thumbnail()   const { return sPathHolder.GetPath(mThumbnailPath) / sFileHolder.GetString(mThumbnailFile); }
    [[nodiscard]] Path         Video()       const { return sPathHolder.GetPath(mVideoPath) / sFileHolder.GetString(mVideoFile); }
    [[nodiscard]] std::string  Developer()   const { return sDeveloperHolder.GetString(mDeveloper);       }
    [[nodiscard]] std::string  Publisher()   const { return sPublisherHolder.GetString(mPublisher);       }
    [[nodiscard]] std::string  Genre()       const { return sGenreHolder.GetString(mGenre);               }
    [[nodiscard]] std::string  Emulator()    const { return sEmulatorHolder.GetString(mEmulator);         }
    [[nodiscard]] std::string  Core()        const { return sCoreHolder.GetString(mCore);                 }
    [[nodiscard]] std::string  Ratio()       const { return sRatioHolder.GetString(mRatio);               }
    [[nodiscard]] Path         LastPatch()   const { return sPathHolder.GetPath(mRomPath) / sFileHolder.GetString(mRomFile); }

    [[nodiscard]] float              Rating()          const { return mRating;                           }
    [[nodiscard]] int                PlayerRange()     const { return mPlayers;                          }
    [[nodiscard]] int                PlayerMax()       const { return mPlayers >> 16;                    }
    [[nodiscard]] int                PlayerMin()       const { return mPlayers & 0xFFFF;                 }
    [[nodiscard]] unsigned int       ReleaseDateEpoc() const { return mReleaseDate;                      }
    [[nodiscard]] DateTime           ReleaseDate()     const { return DateTime((long long)mReleaseDate); }
    [[nodiscard]] int                PlayCount()       const { return mPlayCount;                        }
    [[nodiscard]] unsigned int       LastPlayedEpoc()  const { return mLastPlayed;                       }
    [[nodiscard]] DateTime           LastPlayed()      const { return DateTime((long long)mLastPlayed);  }
    [[nodiscard]] Regions::RegionPack Region()          const { return mRegion;                           }
    [[nodiscard]] int                RomCrc32()        const { return mRomCrc32;                         }
    [[nodiscard]] bool               Favorite()        const { return mFavorite;                         }
    [[nodiscard]] bool               Hidden()          const { return mHidden;                           }
    [[nodiscard]] bool               Adult()           const { return mAdult;                            }
    [[nodiscard]] bool               LatestVersion()   const { return mLatestVerion;                     }
    [[nodiscard]] bool               NoGame()          const { return mNoGame;                           }
    [[nodiscard]] GameGenres         GenreId()         const { return mGenreId;                          }
    [[nodiscard]] RotationType       Rotation()        const { return mRotation;                         }

    /*
     * Validators
     */

    [[nodiscard]] bool HasImage()     const { return (mImagePath     | mImageFile    ) >= 0; }
    [[nodiscard]] bool HasThumnnail() const { return (mThumbnailPath | mThumbnailFile) >= 0; }
    [[nodiscard]] bool HasVideo()     const { return (mVideoPath     | mVideoFile    ) >= 0; }

    /*
     * String accessors
     */

    [[nodiscard]] std::string RomAsString()         const { return (sPathHolder.GetPath(mRomPath) / sFileHolder.GetString(mRomFile)).ToString(); }
    [[nodiscard]] std::string NameAsString()        const { return sNameHolder.GetString(mName);                 }
    [[nodiscard]] std::string AliasAsString()       const { return sAliasHolder.GetString(mAlias);                 }
    [[nodiscard]] std::string EmulatorAsString()    const { return sEmulatorHolder.GetString(mEmulator);         }
    [[nodiscard]] std::string CoreAsString()        const { return sCoreHolder.GetString(mCore);                 }
    [[nodiscard]] std::string RatioAsString()       const { return sRatioHolder.GetString(mRatio, DefaultValueRatio); }
    [[nodiscard]] std::string DescriptionAsString() const { return sDescriptionHolder.GetString(mDescription);   }
    [[nodiscard]] std::string ImageAsString()       const { return (sPathHolder.GetPath(mImagePath) / sFileHolder.GetString(mImageFile)).ToString(); }
    [[nodiscard]] std::string ThumbnailAsString()   const { return (sPathHolder.GetPath(mThumbnailPath) / sFileHolder.GetString(mThumbnailFile)).ToString(); }
    [[nodiscard]] std::string VideoAsString()       const { return (sPathHolder.GetPath(mVideoPath) / sFileHolder.GetString(mVideoFile)).ToString(); }
    [[nodiscard]] std::string DeveloperAsString()   const { return sDeveloperHolder.GetString(mDeveloper);       }
    [[nodiscard]] std::string PublisherAsString()   const { return sPublisherHolder.GetString(mPublisher);       }
    [[nodiscard]] std::string GenreAsString()       const { return sGenreHolder.GetString(mGenre);               }
    [[nodiscard]] std::string RegionAsString()      const { return Regions::Serialize4Regions(mRegion);          }

    [[nodiscard]] std::string RatingAsString()      const { return Strings::ToString(mRating, 2);                        }
    [[nodiscard]] std::string PlayersAsString()     const { return IntToRange(mPlayers);                                 }
    [[nodiscard]] std::string ReleaseDateAsString() const { return mReleaseDate != 0 ? DateTime((long long)mReleaseDate).ToCompactISO8601() : ""; }
    [[nodiscard]] std::string PlayCountAsString()   const { return Strings::ToString(mPlayCount);                           }
    [[nodiscard]] std::string LastPlayedAsString()  const { return mLastPlayed != 0 ? DateTime((long long)mLastPlayed).ToCompactISO8601() : ""; }
    [[nodiscard]] std::string FavoriteAsString()    const { return mFavorite ? "true" : "false";                         }
    [[nodiscard]] std::string RomCrc32AsString()    const { std::string r; IntToHex(mRomCrc32, r); return r;             }
    [[nodiscard]] std::string HiddenAsString()      const { return mHidden ? "true" : "false";                           }
    [[nodiscard]] std::string AdultAsString()       const { return mAdult ? "true" : "false";                            }
    [[nodiscard]] std::string GenreIdAsString()     const { return Strings::ToString((int)mGenreId);                           }
    [[nodiscard]] std::string LastPatchAsString()   const { return (sPathHolder.GetPath(mLastPatchPath) / sFileHolder.GetString(mLastPatchFile)).ToString(); }
    [[nodiscard]] std::string RotationAsString()    const { return RotationUtils::StringValue(mRotation); }


    /*
     * Setters
     */

    void SetRomPath(const Path& image)
    {
      mRomPath = sPathHolder.AddString16(image.Directory().ToString());
      mRomFile = sFileHolder.AddString32(image.Filename());
      mDirty = true;
    }
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
    void SetLastPatch(const Path& patch)
    {
      mLastPatchPath = sLastPatchPathHolder.AddString16(patch.Directory().ToString());
      mLastPatchFile = sLastPatchFileHolder.AddString16(patch.Filename());
      mDirty = true;
    }
    void SetEmulator(const std::string& emulator)       { mEmulator     = sEmulatorHolder.AddString16(emulator);       mDirty = true; }
    void SetCore(const std::string& core)               { mCore         = sCoreHolder.AddString16(core);               mDirty = true; }
    void SetRatio(const std::string& ratio)             { mRatio        = sRatioHolder.AddString8(ratio);              mDirty = true; }
    void SetGenre(const std::string& genre)             { mGenre        = sGenreHolder.AddString32(genre);             mDirty = true; }
    void SetName(const std::string& name)               { mName         = sNameHolder.AddString32(name);               mDirty = true; }
    void SetAlias(const std::string& alias)             { mAlias        = sAliasHolder.AddString32(alias);              mDirty = true; }
    void SetDescription(const std::string& description) { mDescription  = sDescriptionHolder.AddString32(description); mDirty = true; }
    void SetReleaseDate(const DateTime& releasedate)    { mReleaseDate  = (int)releasedate.ToEpochTime();              mDirty = true; }
    void SetDeveloper(const std::string& developer)     { mDeveloper    = sDeveloperHolder.AddString32(developer);     mDirty = true; }
    void SetPublisher(const std::string& publisher)     { mPublisher    = sPublisherHolder.AddString32(publisher);     mDirty = true; }
    void SetRating(float rating)                        { mRating       = rating;                                      mDirty = true; }
    void SetPlayers(int min, int max)                   { mPlayers      = (max << 16) + min;                           mDirty = true; }
    void SetRegion(Regions::RegionPack regions)         { mRegion       = regions;                                     mDirty = true; }
    void SetRomCrc32(int romcrc32)                      { mRomCrc32     = romcrc32;                                    mDirty = true; }
    void SetFavorite(bool favorite)                     { mFavorite     = favorite;                                    mDirty = true; }
    void SetHidden(bool hidden)                         { mHidden       = hidden;                                      mDirty = true; }
    void SetAdult(bool adult)                           { mAdult        = adult;                                       mDirty = true; }
    void SetGenreId(GameGenres genre)                   { mGenreId      = genre;                                       mDirty = true; }
    void SetRotation(RotationType rotation)             { mRotation     = rotation;                                    mDirty = true; }
    // Volatiles flags - no dirtiness
    void SetPreinstalled(bool preinstalled)             { mPreinstalled = preinstalled;                                               }
    void SetLatestVersion(bool latestVersion)           { mLatestVerion = latestVersion;                                              }
    void SetNoGame(bool noGame)                         { mNoGame       = noGame;                                                     }

    // Special setter to force dirty
    void SetDirty() { mDirty = true; }
    void UnsetDirty() { mDirty = false; }

    /*
     * Volatile setters - do not set the Dirty flag for auto-saving
     */

    void SetVolatileDescription(const std::string& description) { bool dirty = mDirty; SetDescription(description); mDirty = dirty; }
    void SetVolatileImagePath(const Path& image) { bool dirty = mDirty; SetImagePath(image); mDirty = dirty; }

    /*
     * String setters
     */

    void SetRomPathAsString(const std::string& image)           { SetRomPath(Path(image));           }
    void SetLastPatchAsString(const std::string& patch)         { SetLastPatch(Path(patch)); }
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
    void SetPlayCountAsString(const std::string& playcount)     { int p = 0; if (StringToInt(playcount, p)) { mPlayCount = (short)p; mDirty = true; } }
    void SetGenreIdAsString(const std::string& genre)           { int g = 0; if (StringToInt(genre, g)) { mGenreId = (GameGenres)g; mDirty = true; } }
    void SetRegionAsString(const std::string& region)           { mRegion = Regions::Deserialize4Regions(region); mDirty = true; }
    void SetRotationAsString(const std::string& rotation)       { mRotation = RotationUtils::FromString(rotation); mDirty = true;}
    /*
     * Defaults
     */

    [[nodiscard]] bool IsDefaultRom()             const { return Default().mRomFile == mRomFile && Default().mRomPath == mRomPath; }
    [[nodiscard]] bool IsDefaultName()            const { return Default().mName == mName;               }
    [[nodiscard]] bool IsDefaultAlias()           const { return Default().mAlias == mAlias;               }
    [[nodiscard]] bool IsDefaultEmulator()        const { return Default().mEmulator == mEmulator;       }
    [[nodiscard]] bool IsDefaultCore()            const { return Default().mCore == mCore;               }
    [[nodiscard]] bool IsDefaultRatio()           const { return Default().mRatio == mRatio;           }
    [[nodiscard]] bool IsDefaultDescription()     const { return Default().mDescription == mDescription; }
    [[nodiscard]] bool IsDefaultImage()           const { return Default().mImageFile == mImageFile && Default().mImagePath == mImagePath; }
    [[nodiscard]] bool IsDefaultThumbnail()       const { return Default().mThumbnailFile == mThumbnailFile && Default().mThumbnailPath == mThumbnailPath; }
    [[nodiscard]] bool IsDefaultVideo()           const { return Default().mVideoFile == mVideoFile && Default().mVideoPath == mVideoPath; }
    [[nodiscard]] bool IsDefaultDeveloper()       const { return Default().mDeveloper == mDeveloper;     }
    [[nodiscard]] bool IsDefaultPublisher()       const { return Default().mPublisher == mPublisher;     }
    [[nodiscard]] bool IsDefaultGenre()           const { return Default().mGenre == mGenre;             }
    [[nodiscard]] bool IsDefaultRegion()          const { return Default().mRegion.Pack == mRegion.Pack; }
    [[nodiscard]] bool IsDefaultRating()          const { return Default().mRating == mRating;           }
    [[nodiscard]] bool IsDefaultPlayerRange()     const { return Default().mPlayers == mPlayers;         }
    [[nodiscard]] bool IsDefaultReleaseDateEpoc() const { return Default().mReleaseDate == mReleaseDate; }
    [[nodiscard]] bool IsDefaultPlayCount()       const { return Default().mPlayCount == mPlayCount;     }
    [[nodiscard]] bool IsDefaultLastPlayedEpoc()  const { return Default().mLastPlayed == mLastPlayed;   }
    [[nodiscard]] bool IsDefaultRomCrc32()        const { return Default().mRomCrc32 == mRomCrc32;       }
    [[nodiscard]] bool IsDefaultFavorite()        const { return Default().mFavorite == mFavorite;       }
    [[nodiscard]] bool IsDefaultHidden()          const { return Default().mHidden == mHidden;           }
    [[nodiscard]] bool IsDefaultAdult()           const { return Default().mAdult == mAdult;             }
    [[nodiscard]] bool IsDefaultGenreId()         const { return Default().mGenreId == mGenreId;         }
    [[nodiscard]] bool IsDefaultLastPath()        const { return Default().mLastPatchPath == mLastPatchPath && Default().mLastPatchFile == mLastPatchFile; }
    [[nodiscard]] bool IsDefaultRotation()        const { return Default().mRotation == mRotation; }


    /*
     * Convenient Accessors
     */

    [[nodiscard]] bool IsDirty()  const { return mDirty; }

    /*
     * Special modifiers
     */

    void IncPlayCount() { mPlayCount++; mDirty = true; }
    void SetLastPlayedNow() { mLastPlayed = (unsigned int)DateTime().ToEpochTime(); mDirty = true; }


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

    [[nodiscard]] MetadataStringHolder::Index32 FileIndex() const { return mRomFile; }
    [[nodiscard]] MetadataStringHolder::Index32 NameIndex() const { return mName; }
    [[nodiscard]] MetadataStringHolder::Index32 AliasIndex() const { return mAlias; }
    [[nodiscard]] MetadataStringHolder::Index32 DescriptionIndex() const { return mDescription; }
    [[nodiscard]] MetadataStringHolder::Index32 DeveloperIndex() const { return mDeveloper; }
    [[nodiscard]] MetadataStringHolder::Index32 PublisherIndex() const { return mPublisher; }

    static int FileIndexCount() { return sFileHolder.ObjectCount(); }
    static int NameIndexCount() { return sNameHolder.ObjectCount(); }
    static int AliasIndexCount() { return sAliasHolder.ObjectCount(); }
    static int DescriptionIndexCount() { return sDescriptionHolder.ObjectCount(); }
    static int DeveloperIndexCount() { return sDeveloperHolder.ObjectCount(); }
    static int PublisherIndexCount() { return sPublisherHolder.ObjectCount(); }

    /*
     * Search
     */

    [[nodiscard]] bool IsMatchingFileIndex(MetadataStringHolder::Index32 index) const { return mRomFile == index; }
    [[nodiscard]] bool IsMatchingNameIndex(MetadataStringHolder::Index32 index) const { return mName == index; }
    [[nodiscard]] bool IsMatchingAliasIndex(MetadataStringHolder::Index32 index) const { return mAlias == index; }
    [[nodiscard]] bool IsMatchingDescriptionIndex(MetadataStringHolder::Index32 index) const { return mDescription == index; }
    [[nodiscard]] bool IsMatchingDeveloperIndex(MetadataStringHolder::Index32 index) const { return mDeveloper == index; }
    [[nodiscard]] bool IsMatchingPublisherIndex(MetadataStringHolder::Index32 index) const { return mPublisher == index; }

    /*!
     * @brief Search text in game names
     * @param originaltext Text to search for
     * @param output Result container
     */
    static void SearchInNames(const std::string& originaltext, MetadataStringHolder::FoundTextList& output, int context) { return sNameHolder.FindText(originaltext, output, context); }

    /*!
     * @brief Search text in game aliases
     * @param originaltext Text to search for
     * @param output Result container
     */
    static void SearchInAlias(const std::string& originaltext, MetadataStringHolder::FoundTextList& output, int context) { return sAliasHolder.FindText(originaltext, output, context); }

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
    static void SearchInPath(const std::string& originaltext, MetadataStringHolder::FoundTextList& output, int context) { return sFileHolder.FindText(originaltext, output, context); }

    /*
     * Part comparers
     */

    [[nodiscard]] bool AreRomEqual(const MetadataDescriptor& other) const { return mRomFile == other.mRomFile && mRomPath == other.mRomPath; }
};

