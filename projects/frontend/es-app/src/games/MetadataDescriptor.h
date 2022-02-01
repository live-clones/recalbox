#pragma once

#include <utils/os/fs/Path.h>
#include <utils/Xml.h>
#include <utils/datetime/DateTime.h>
#include <utils/Strings.h>
#include <games/classifications/Regions.h>
#include "ItemType.h"
#include "games/classifications/Genres.h"

//#define _METADATA_STATS_

// Forward declaration (MetadataFieldDescriptor must indlude MetadataDescriptor)
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
    static const std::string DefaultValuePlaycount;
    //static const std::string DefaultValueUnknown;
    static const std::string DefaultValueFavorite;
    static const std::string DefaultValueHidden;
    static const Path        DefaultEmptyPath;

    //! Game node <game></game>
    static const std::string GameNodeIdentifier;
    //! Folder node <folder></folder>
    static const std::string FolderNodeIdentifier;

    // Please keep field ordered by type size to reduce alignment padding
    std::string  mName;         //!< Name as simple string
    std::string  mDescription;  //!< Description, multiline text
    Path         mImage;        //!< Image path
    std::string  mDeveloper;    //!< Developer name
    std::string  mPublisher;    //!< Publisher name
    std::string* mGenre;        //!< Genres, comma separated
    std::string* mEmulator;     //!< Specific emulator
    std::string* mCore;         //!< Specific core
    std::string* mRatio;        //!< Specific screen ratio
    Path*        mThumbnail;    //!< Thumbnail path
    Path*        mVideo;        //!< Video path
    float        mRating;       //!< Rating from 0.0 to 1.0
    GameGenres   mGenreId;      //!< Normalized Genre
    int          mRegion;       //!< Rom/Game Region
    int          mPlayers;      //!< Players range: LSW:from - MSW:to (allow sorting by max players)
    int          mReleaseDate;  //!< Release data (epoch)
    int          mPlaycount;    //!< Play counter
    unsigned int mLastPlayed;   //!< Last time played (epoch)
    int          mRomCrc32;     //!< Rom Crc32
    bool         mFavorite;     //!< Favorite game
    bool         mHidden;       //!< Hidden game
    bool         mAdult;        //!< Adult state
    bool         mDirty;        //!< Dirty flag (modified data flag)

    ItemType     mType;         //!< Metadata type

    /*!
     * Build an empty object filled with default values
     * @return Object filled with default values
     */
    static MetadataDescriptor BuildDefaultValueMetadataDescriptor();

    /*!
     * Free the PString if non null
     * @param string Pointer to std::string
     */
    static void FreePString(std::string*& string)
    {
      if (string != nullptr)
      {
        delete string;
        string = nullptr;
      }
    }

    /*!
     * Free the PPath if non null
     * @param path Pointer to Path
     */
    static void FreePPath(Path*& path)
    {
      if (path != nullptr)
      {
        delete path;
        path = nullptr;
      }
    }

    /*!
     * Assign a value to the given PString.
     * the PString is created/destroyed if required
     * @param string PString to assign value to
     * @param value Value to assign
     */
    static void AssignPString(std::string*& string, const std::string& value)
    {
      if (value.empty()) FreePString(string);
      else
      {
        if (string == nullptr) string = new std::string();
        *string = value;
      }
    }

    /*!
     * Assign a value to the given PPath.
     * the PPath is created/destroyed if required
     * @param string PPath to assign value to
     * @param value Value to assign
     */
    static void AssignPPath(Path*& path, const Path& value)
    {
      if (value.IsEmpty()) FreePPath(path);
      else
      {
        if (path == nullptr) path = new Path();
        *path = value;
      }
    }

    /*!
     * Read PString content. Return static empty string if the PString is null
     * @param string PString to read
     * @return read value
     */
    static const std::string& ReadPString(const std::string* string, const std::string& defaultvalue)
    {
      if (string == nullptr) return defaultvalue;
      return *string;
    }

    /*!
     * Read PPath content. Return static empty path if the PPath is null
     * @param path PPath to read
     * @return read value
     */
    static const Path& ReadPPath(const Path* path, const Path& defaultvalue)
    {
      if (path == nullptr) return defaultvalue;
      return *path;
    }

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

    /*!
     * Free all allocated objects and return thoses objects to uninitialized state
     */
    void FreeAll();

  public:
    /*
     * Destructor
     */
    ~MetadataDescriptor();

    /*!
     * Default constructor
     */
    explicit MetadataDescriptor(const std::string& defaultName, ItemType type)
      : mName(defaultName),
        mDescription(),
        mImage(),
        mDeveloper(),
        mPublisher(),
        mGenre(),
        mEmulator(nullptr),
        mCore(nullptr),
        mRatio(nullptr),
        mThumbnail(nullptr),
        mVideo(nullptr),
        mRating(0.0f),
        mGenreId(GameGenres::None),
        mRegion(0),
        mPlayers((1 << 16) + 1),
        mReleaseDate(0),
        mPlaycount(0),
        mLastPlayed(0),
        mRomCrc32(0),
        mFavorite(false),
        mHidden(false),
        mAdult(false),
        mDirty(false),
        mType(type)
    {
      #ifdef _METADATA_STATS_
      LivingClasses++;
      if (_Type == ItemType::Game) LivingGames++;
      if (_Type == ItemType::Folder) LivingFolders++;
      #endif
    }

    /*!
     * Copy constructor
     * @param source Source to copy data from
     */
    #ifdef _METADATA_STATS_
    MetadataDescriptor(const MetadataDescriptor& source)
      : _Name        (source._Name       ),
        _Description (source._Description),
        _Image       (source._Image      ),
        _Developer   (source._Developer  ),
        _Publisher   (source._Publisher  ),
        _Genre       (source._Genre      ),
        _Emulator    (source._Emulator   ),
        _Core        (source._Core       ),
        _Ratio       (source._Ratio      ),
        _Thumbnail   (source._Thumbnail  ),
        _Video       (source._Video      ),
        _Region      (source._Region     ),
        _Rating      (source._Rating     ),
        _GenreId     (source._GenreId    ),
        _Players     (source._Players    ),
        _ReleaseDate (source._ReleaseDate),
        _Playcount   (source._Playcount  ),
        _LastPlayed  (source._LastPlayed ),
        _RomCrc32    (source._RomCrc32   ),
        _Favorite    (source._Favorite   ),
        _Hidden      (source._Hidden     ),
        _Adult       (source._Adult      ),
        _Dirty       (source._Dirty      ),
        _Type        (source._Type       )
    {
      LivingClasses++;
      if (_Type == ItemType::Game) LivingGames++;
      if (_Type == ItemType::Folder) LivingFolders++;
    }
    #endif

    /*!
     * Move constructor
     * @param source  Source to move data from
     */
    MetadataDescriptor(MetadataDescriptor&& source) noexcept
      : mName(std::move(source.mName)),
        mDescription(std::move(source.mDescription)),
        mImage(std::move(source.mImage)),
        mDeveloper(std::move(source.mDeveloper)),
        mPublisher(std::move(source.mPublisher)),
        mGenre(source.mGenre),
        mEmulator(source.mEmulator),
        mCore(source.mCore),
        mRatio(source.mRatio),
        mThumbnail(source.mThumbnail),
        mVideo(source.mVideo),
        mRating(source.mRating),
        mGenreId(source.mGenreId),
        mRegion(source.mRegion),
        mPlayers(source.mPlayers),
        mReleaseDate(source.mReleaseDate),
        mPlaycount(source.mPlaycount),
        mLastPlayed(source.mLastPlayed),
        mRomCrc32(source.mRomCrc32),
        mFavorite(source.mFavorite),
        mHidden(source.mHidden),
        mAdult(source.mAdult),
        mDirty(source.mDirty),
        mType(source.mType)
    {
      #ifdef _METADATA_STATS_
      LivingClasses++;
      if (_Type == ItemType::Game) LivingGames++;
      if (_Type == ItemType::Folder) LivingFolders++;
      #endif
      source.mEmulator  = nullptr;
      source.mCore      = nullptr;
      source.mRatio     = nullptr;
      source.mThumbnail = nullptr;
      source.mVideo     = nullptr;
      source.mGenre     = nullptr;
    }

    /*!
     * Assignment operator - Required by STL objects since a copy operator is defined
     * @param source Source to copy data from
     */
    MetadataDescriptor& operator = (const MetadataDescriptor& source)
    {
      if (&source == this) return *this;

      #ifdef _METADATA_STATS_
      if (_Type == ItemType::Game) LivingGames--;
      if (_Type == ItemType::Folder) LivingFolders--;
      #endif

      FreeAll();
      if (source.mEmulator != nullptr)  mEmulator  = new std::string(*source.mEmulator);
      if (source.mCore != nullptr)      mCore      = new std::string(*source.mCore);
      if (source.mRatio != nullptr)     mRatio     = new std::string(*source.mRatio);
      if (source.mThumbnail != nullptr) mThumbnail = new Path(*source.mThumbnail);
      if (source.mVideo != nullptr)     mVideo     = new Path(*source.mVideo);
      if (source.mGenre != nullptr)     mGenre     = new std::string(*source.mGenre);
      mName        = source.mName       ;
      mDescription = source.mDescription;
      mImage       = source.mImage      ;
      mDeveloper   = source.mDeveloper  ;
      mPublisher   = source.mPublisher  ;
      mRegion      = source.mRegion     ;
      mRating      = source.mRating     ;
      mGenreId     = source.mGenreId    ;
      mPlayers     = source.mPlayers    ;
      mReleaseDate = source.mReleaseDate;
      mPlaycount   = source.mPlaycount  ;
      mLastPlayed  = source.mLastPlayed ;
      mRomCrc32    = source.mRomCrc32   ;
      mFavorite    = source.mFavorite   ;
      mHidden      = source.mHidden     ;
      mAdult       = source.mAdult      ;
      mDirty       = source.mDirty      ;
      mType        = source.mType       ;

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
      if (_Type == ItemType::Game) LivingGames--;
      if (_Type == ItemType::Folder) LivingFolders--;
      #endif

      FreeAll();
      mName        = std::move(source.mName);
      mEmulator    = source.mEmulator   ; source.mEmulator  = nullptr;
      mCore        = source.mCore       ; source.mCore      = nullptr;
      mRatio       = source.mRatio      ; source.mRatio     = nullptr;
      mThumbnail   = source.mThumbnail  ; source.mThumbnail = nullptr;
      mVideo       = source.mVideo      ; source.mVideo     = nullptr;
      mGenre       = source.mGenre      ; source.mGenre     = nullptr;
      mDescription = std::move(source.mDescription);
      mImage       = std::move(source.mImage);
      mDeveloper   = std::move(source.mDeveloper);
      mPublisher   = std::move(source.mPublisher);
      mRating      = source.mRating     ;
      mGenreId     = source.mGenreId    ;
      mPlayers     = source.mPlayers    ;
      mReleaseDate = source.mReleaseDate;
      mPlaycount   = source.mPlaycount  ;
      mLastPlayed  = source.mLastPlayed ;
      mRomCrc32    = source.mRomCrc32   ;
      mFavorite    = source.mFavorite   ;
      mHidden      = source.mHidden     ;
      mAdult       = source.mAdult      ;
      mDirty       = source.mDirty      ;
      mType        = source.mType       ;

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

    const std::string& Name()        const { return mName;                                        }
    const std::string& Emulator()    const { return ReadPString(mEmulator, DefaultValueEmpty);    }
    const std::string& Core()        const { return ReadPString(mCore, DefaultValueEmpty);        }
    const std::string& Ratio()       const { return ReadPString(mRatio, DefaultValueRatio);       }
    const std::string& Description() const { return mDescription;                                 }
    const Path&        Image()       const { return mImage;                                       }
    const Path&        Thumbnail()   const { return ReadPPath(mThumbnail, DefaultEmptyPath);    }
    const Path&        Video()       const { return ReadPPath(mVideo, DefaultEmptyPath);        }
    const std::string& Developer()   const { return mDeveloper;                                   }
    const std::string& Publisher()   const { return mPublisher;                                   }
    const std::string& Genre()       const { return ReadPString(mGenre, DefaultValueEmpty);       }

    float              Rating()          const { return mRating;                           }
    int                PlayerRange()     const { return mPlayers;                          }
    int                PlayerMax()       const { return mPlayers >> 16;                    }
    int                PlayerMin()       const { return mPlayers & 0xFFFF;                 }
    int                ReleaseDateEpoc() const { return mReleaseDate;                      }
    DateTime           ReleaseDate()     const { return DateTime((long long)mReleaseDate); }
    int                PlayCount()       const { return mPlaycount;                        }
    unsigned int       LastPlayedEpoc()  const { return mLastPlayed;                       }
    DateTime           LastPlayed()      const { return DateTime((long long)mLastPlayed);  }
    int                Region()          const { return mRegion;                           }
    int                RomCrc32()        const { return mRomCrc32;                         }
    bool               Favorite()        const { return mFavorite;                         }
    bool               Hidden()          const { return mHidden;                           }
    bool               Adult()           const { return mAdult;                            }
    GameGenres         GenreId()         const { return mGenreId;                          }

    /*
     * String accessors
     */

    std::string NameAsString()        const { return mName;                                        }
    std::string EmulatorAsString()    const { return ReadPString(mEmulator, DefaultValueEmpty);    }
    std::string CoreAsString()        const { return ReadPString(mCore, DefaultValueEmpty);        }
    std::string RatioAsString()       const { return ReadPString(mRatio, DefaultValueRatio);       }
    std::string DescriptionAsString() const { return mDescription;                                 }
    std::string ImageAsString()       const { return mImage.ToString();                            }
    std::string ThumbnailAsString()   const { return ReadPPath(mThumbnail, Path::Empty).ToString(); }
    std::string VideoAsString()       const { return ReadPPath(mVideo, Path::Empty).ToString();  }
    std::string DeveloperAsString()   const { return mDeveloper;                                   }
    std::string PublisherAsString()   const { return mPublisher;                                   }
    std::string GenreAsString()       const { return ReadPString(mGenre, DefaultValueEmpty);       }
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

    void SetName(const std::string& name)               { mName = name; mDirty = true;                                  }
    void SetEmulator(const std::string& emulator)       { AssignPString(mEmulator, emulator); mDirty = true;            }
    void SetCore(const std::string& core)               { AssignPString(mCore, core); mDirty = true;                    }
    void SetRatio(const std::string& ratio)             { AssignPString(mRatio, ratio); mDirty = true;                  }
    void SetDescription(const std::string& description) { mDescription = description; mDirty = true;                    }
    void SetImagePath(const Path& image)                { mImage = image; mDirty = true;                                }
    void SetThumbnailPath(const Path& thumbnail)        { AssignPPath(mThumbnail, thumbnail); mDirty = true;            }
    void SetVideoPath(const Path& video)                { AssignPPath(mVideo, video); mDirty = true;                    }
    void SetReleaseDate(const DateTime& releasedate)    { mReleaseDate = (int)releasedate.ToEpochTime(); mDirty = true; }
    void SetDeveloper(const std::string& developer)     { mDeveloper = developer; mDirty = true;                        }
    void SetPublisher(const std::string& publisher)     { mPublisher = publisher; mDirty = true;                        }
    void SetGenre(const std::string& genre)             { AssignPString(mGenre, genre); mDirty = true;                  }
    void SetRating(float rating)                        { mRating = rating; mDirty = true;                              }
    void SetPlayers(int min, int max)
    {
      mPlayers = (max << 16) + min;
      mDirty = true;
    }
    void SetRegion(int regions)                         { mRegion = regions; mDirty = true;                             }
    void SetRomCrc32(int romcrc32)                      { mRomCrc32 = romcrc32; mDirty = true;                          }
    void SetFavorite(bool favorite)                     { mFavorite = favorite; mDirty = true;                          }
    void SetHidden(bool hidden)                         { mHidden = hidden; mDirty = true;                              }
    void SetAdult(bool adult)                           { mAdult = adult; mDirty = true;                                }
    void SetGenreId(GameGenres genre)                   { mGenreId = genre; mDirty = true;                              }

    // Special setter to force dirty
    void SetDirty() { mDirty = true; }

    /*
     * Volatile setters - do not set the Dirty flag for auto-saving
     */

    void SetVolatileDescription(const std::string& description) { mDescription = description; }
    void SetVolatileImagePath(const Path& image) { mImage = image; }

    /*
     * String setters
     */

    void SetImagePathAsString(const std::string& image)                { mImage = image; mDirty = true;                                }
    void SetThumbnailPathAsString(const std::string& thumbnail)        { AssignPPath(mThumbnail, Path(thumbnail)); mDirty = true;            }
    void SetVideoPathAsString(const std::string& video)                { AssignPPath(mVideo, Path(video)); mDirty = true;                    }
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
    void SetPlayCountAsString(const std::string& playcount)     { int p = 0; if (StringToInt(playcount, p)) { mPlaycount = p; mDirty = true; } }
    void SetGenreIdAsString(const std::string& genre)           { int g = 0; if (StringToInt(genre, g)) { mGenreId = (GameGenres)g; mDirty = true; } }
    void SetRegionAsString(const std::string& region)           { mRegion = (int)Regions::Deserialize4Regions(region); mDirty = true; }

    /*
     * Defaults
     */

    bool IsDefaultName()            const { return sDefault.mName == mName;        }
    bool IsDefaultEmulator()        const { return sDefault.Emulator() == Emulator();   }
    bool IsDefaultCore()            const { return sDefault.Core() == Core();       }
    bool IsDefaultRatio()           const { return sDefault.Ratio() == Ratio();      }
    bool IsDefaultDescription()     const { return sDefault.mDescription == mDescription; }
    bool IsDefaultImage()           const { return sDefault.mImage == mImage;       }
    bool IsDefaultThumbnail()       const { return sDefault.Thumbnail() == Thumbnail();  }
    bool IsDefaultVideo()           const { return sDefault.Video() == Video();      }
    bool IsDefaultDeveloper()       const { return sDefault.mDeveloper == mDeveloper;   }
    bool IsDefaultPublisher()       const { return sDefault.mPublisher == mPublisher;   }
    bool IsDefaultGenre()           const { return sDefault.mGenre == mGenre;       }
    bool IsDefaultRegion()          const { return sDefault.Region() == Region();     }
    bool IsDefaultRating()          const { return sDefault.mRating == mRating;      }
    bool IsDefaultPlayerRange()     const { return sDefault.mPlayers == mPlayers;     }
    bool IsDefaultReleaseDateEpoc() const { return sDefault.mReleaseDate == mReleaseDate; }
    bool IsDefaultPlayCount()       const { return sDefault.mPlaycount == mPlaycount;   }
    bool IsDefaultLastPlayedEpoc()  const { return sDefault.mLastPlayed == mLastPlayed;  }
    bool IsDefaultRomCrc32()        const { return sDefault.mRomCrc32 == mRomCrc32;    }
    bool IsDefaultFavorite()        const { return sDefault.mFavorite == mFavorite;    }
    bool IsDefaultHidden()          const { return sDefault.mHidden == mHidden;      }
    bool IsDefaultAdult()           const { return sDefault.mAdult == mAdult;       }
    bool IsDefaultGenreId()         const { return sDefault.mGenreId == mGenreId;     }

    /*
     * Convenient Accessors
     */

    //bool IsGame()   const { return _Type == ObjectType::Game;   }
    //bool IsFolder() const { return _Type == ObjectType::Folder; }
    bool IsDirty()  const { return mDirty;                }

    /*
     * Convenient Methods
     */

    //static bool AreGames(const MetadataDescriptor& md1, const MetadataDescriptor& md2) { return (md1._Type == md2._Type) && md1.IsGame(); }
    //static bool AreFolders(const MetadataDescriptor& md1, const MetadataDescriptor& md2) { return (md1._Type == md2._Type) && md1.IsFolder(); }

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
};

