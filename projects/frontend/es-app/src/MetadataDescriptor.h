#pragma once

#include <boost/property_tree/ptree.hpp>
#include "utils/datetime/DateTime.h"
#include "ItemType.h"

//#define _METADATA_STATS_

// Forward declaration (MetadataFieldDescriptor must indlude MetadataDescriptor)
class MetadataFieldDescriptor;

class MetadataDescriptor
{
  private:
    //! Default value storage for fast default detection
    static MetadataDescriptor _Default;

    #ifdef _METADATA_STATS_
    static int LivingClasses;
    static int LivingNone;
    static int LivingFolders;
    static int LivingGames;
    #endif

    // Static default values
    static const std::string DefaultValueEmulator;
    static const std::string DefaultValueCore;
    static const std::string DefaultValueRatio;
    static const std::string DefaultValueEmpty;
    static const std::string DefaultValueRating;
    static const std::string DefaultValuePlayers;
    static const std::string DefaultValuePlaycount;
    //static const std::string DefaultValueUnknown;
    static const std::string DefaultValueFavorite;
    static const std::string DefaultValueHidden;

    //! Game node <game></game>
    static const std::string GameNodeIdentifier;
    //! Folder node <folder></folder>
    static const std::string FolderNodeIdentifier;

    // Please keep field ordered by typpe size to reduce alignment padding
    std::string  _Name;         //!< Name as simple string
    std::string  _Description;  //!< Description, multiline text
    std::string  _Image;        //!< Image path
    std::string  _Developer;    //!< Developer name
    std::string  _Publisher;    //!< Publisher name
    std::string  _Genre;        //!< Genres, comma separated
    std::string* _Emulator;     //!< Specific emulator
    std::string* _Core;         //!< Specific core
    std::string* _Ratio;        //!< Specific screen ratio
    std::string* _Thumbnail;    //!< Thumbnail path
    std::string* _Video;        //!< Video path
    std::string* _Region;       //!< Rom/Game Region
    float        _Rating;       //!< Rating from 0.0 to 1.0
    int          _Players;      //!< Players range: LSW:from - MSW:to (allow sorting by max players)
    int          _ReleaseDate;  //!< Release data (epoch)
    int          _Playcount;    //!< Play counter
    int          _LastPlayed;   //!< Last time played (epoch)
    int          _RomCrc32;     //!< Rom Crc32
    bool         _Favorite;     //!< Favorite gale
    bool         _Hidden;       //!< Hidden game
    bool         _Dirty;        //!< Dirty flag (modified data flag)

    ItemType     _Type;         //!< Metadata type

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
     * Return the first static internal field descriptor reference
     * @param Type ObjectType from which to retrieve
     * @param count Number of actual field descriptor available
     * @return first static internal field descriptor reference
     */
    static const MetadataFieldDescriptor* GetMetadataFieldDescriptors(ItemType type, int& count);

    static std::string FloatToString(float source, int precision);
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
    //! convenient ptree type access
    typedef boost::property_tree::ptree Tree;
    typedef std::pair<std::string, Tree> TreeNode;

    /*
     * Destructor
     */
    ~MetadataDescriptor();

    /*!
     * Default constructor
     */
    explicit MetadataDescriptor(const std::string& defaultName, ItemType type)
      : _Name(defaultName),
        _Description(),
        _Image(),
        _Developer(),
        _Publisher(),
        _Genre(),
        _Emulator(nullptr),
        _Core(nullptr),
        _Ratio(nullptr),
        _Thumbnail(nullptr),
        _Video(nullptr),
        _Region(nullptr),
        _Rating(0.0f),
        _Players((1<<16)+1),
        _ReleaseDate(0),
        _Playcount(0),
        _LastPlayed(0),
        _RomCrc32(0),
        _Favorite(false),
        _Hidden(false),
        _Dirty(false),
        _Type(type)
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
        _Players     (source._Players    ),
        _ReleaseDate (source._ReleaseDate),
        _Playcount   (source._Playcount  ),
        _LastPlayed  (source._LastPlayed ),
        _RomCrc32    (source._RomCrc32   ),
        _Favorite    (source._Favorite   ),
        _Hidden      (source._Hidden     ),
        _Dirty       (source._Dirty      ),
        _Type        (source._Type       )
    {
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
    MetadataDescriptor(MetadataDescriptor&& source) noexcept
      : _Name        (std::move(source._Name       )),
        _Description (std::move(source._Description)),
        _Image       (std::move(source._Image      )),
        _Developer   (std::move(source._Developer  )),
        _Publisher   (std::move(source._Publisher  )),
        _Genre       (std::move(source._Genre      )),
        _Emulator    (          source._Emulator   ),
        _Core        (          source._Core       ),
        _Ratio       (          source._Ratio      ),
        _Thumbnail   (          source._Thumbnail  ),
        _Video       (          source._Video      ),
        _Region      (          source._Region     ),
        _Rating      (          source._Rating     ),
        _Players     (          source._Players    ),
        _ReleaseDate (          source._ReleaseDate),
        _Playcount   (          source._Playcount  ),
        _LastPlayed  (          source._LastPlayed ),
        _RomCrc32    (          source._RomCrc32   ),
        _Favorite    (          source._Favorite   ),
        _Hidden      (          source._Hidden     ),
        _Dirty       (          source._Dirty      ),
        _Type        (          source._Type       )
    {
      #ifdef _METADATA_STATS_
      LivingClasses++;
      if (_Type == ItemType::Game) LivingGames++;
      if (_Type == ItemType::Folder) LivingFolders++;
      #endif
      source._Emulator  = nullptr;
      source._Core      = nullptr;
      source._Ratio     = nullptr;
      source._Thumbnail = nullptr;
      source._Region    = nullptr;
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
      _Name        = source._Name       ;
      _Description = source._Description;
      _Image       = source._Image      ;
      _Developer   = source._Developer  ;
      _Publisher   = source._Publisher  ;
      _Genre       = source._Genre      ;
      if (source._Emulator  != nullptr) _Emulator  = new std::string(*source._Emulator );
      if (source._Core      != nullptr) _Core      = new std::string(*source._Core     );
      if (source._Ratio     != nullptr) _Ratio     = new std::string(*source._Ratio    );
      if (source._Thumbnail != nullptr) _Thumbnail = new std::string(*source._Thumbnail);
      if (source._Video     != nullptr) _Video     = new std::string(*source._Video    );
      if (source._Region    != nullptr) _Region    = new std::string(*source._Region   );
      _Rating      = source._Rating     ;
      _Players     = source._Players    ;
      _ReleaseDate = source._ReleaseDate;
      _Playcount   = source._Playcount  ;
      _LastPlayed  = source._LastPlayed ;
      _RomCrc32    = source._RomCrc32   ;
      _Favorite    = source._Favorite   ;
      _Hidden      = source._Hidden     ;
      _Dirty       = source._Dirty      ;
      _Type        = source._Type       ;

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
      _Name        = std::move(source._Name       );
      _Emulator    = source._Emulator   ; source._Emulator = nullptr;
      _Core        = source._Core       ; source._Core     = nullptr;
      _Ratio       = source._Ratio      ; source._Ratio    = nullptr;
      _Description = std::move(source._Description);
      _Image       = std::move(source._Image      );
      _Thumbnail   = source._Thumbnail  ; source._Thumbnail = nullptr;
      _Video       = source._Video      ; source._Video     = nullptr;
      _Developer   = std::move(source._Developer  );
      _Publisher   = std::move(source._Publisher  );
      _Genre       = std::move(source._Genre      );
      _Region      = source._Region     ; source._Region   = nullptr;
      //_RomType     = source._RomType  ;
      _Rating      = source._Rating     ;
      _Players     = source._Players    ;
      _ReleaseDate = source._ReleaseDate;
      _Playcount   = source._Playcount  ;
      _LastPlayed  = source._LastPlayed ;
      _RomCrc32    = source._RomCrc32   ;
      _Favorite    = source._Favorite   ;
      _Hidden      = source._Hidden     ;
      _Dirty       = source._Dirty      ;
      _Type        = source._Type       ;

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
    bool Deserialize(const TreeNode& from, const std::string& relativeTo);

    /*!
     * Serialize internal data to XML node
     * @param relativeTo Root path
     * @return Serialized XML node
     */
    void Serialize(Tree& parentTree, const std::string& filePath, const std::string& relativeTo) const;

    /*!
     * Merge value from the source metadata object into the current object
     * current fields ate replaced only if they have their default value.
     * @param source Metadata object from which to merge data
     */
    void Merge(const MetadataDescriptor& source);

    /*
     * Accessors
     */

    ItemType Type() const { return _Type; }

    const std::string& Name()        const { return _Name;                                        }
    const std::string& Emulator()    const { return ReadPString(_Emulator, DefaultValueEmulator); }
    const std::string& Core()        const { return ReadPString(_Core, DefaultValueCore);         }
    const std::string& Ratio()       const { return ReadPString(_Ratio, DefaultValueRatio);       }
    const std::string& Description() const { return _Description;                                 }
    const std::string& Image()       const { return _Image;                                       }
    const std::string& Thumbnail()   const { return ReadPString(_Thumbnail, DefaultValueEmpty);   }
    const std::string& Video()       const { return ReadPString(_Video, DefaultValueEmpty);       }
    const std::string& Developer()   const { return _Developer;                                   }
    const std::string& Publisher()   const { return _Publisher;                                   }
    const std::string& Genre()       const { return _Genre;                                       }
    const std::string& Region()      const { return ReadPString(_Region, DefaultValueEmpty);      }

    float              Rating()          const { return _Rating;                           }
    int                PlayerRange()     const { return _Players;                          }
    int                PlayerMax()       const { return _Players >> 16;                    }
    int                PlayerMin()       const { return _Players & 0xFFFF;                 }
    int                ReleaseDateEpoc() const { return _ReleaseDate;                      }
    DateTime           ReleaseDate()     const { return DateTime((long long)_ReleaseDate); }
    int                PlayCount()       const { return _Playcount;                        }
    int                LastPlayedEpoc()  const { return _LastPlayed;                       }
    DateTime           LastPlayed()      const { return DateTime((long long)_LastPlayed);  }
    int                RomCrc32()        const { return _RomCrc32;                         }
    bool               Favorite()        const { return _Favorite;                         }
    bool               Hidden()          const { return _Hidden;                           }

    /*
     * String accessors
     */

    std::string NameAsString()        const { return _Name;                                        }
    std::string EmulatorAsString()    const { return ReadPString(_Emulator, DefaultValueEmulator); }
    std::string CoreAsString()        const { return ReadPString(_Core, DefaultValueCore);         }
    std::string RatioAsString()       const { return ReadPString(_Ratio, DefaultValueRatio);       }
    std::string DescriptionAsString() const { return _Description;                                 }
    std::string ImageAsString()       const { return _Image;                                       }
    std::string ThumbnailAsString()   const { return ReadPString(_Thumbnail, DefaultValueEmpty);   }
    std::string VideoAsString()       const { return ReadPString(_Video, DefaultValueEmpty);       }
    std::string DeveloperAsString()   const { return _Developer;                                   }
    std::string PublisherAsString()   const { return _Publisher;                                   }
    std::string GenreAsString()       const { return _Genre;                                       }
    std::string RegionAsString()      const { return ReadPString(_Region, DefaultValueEmpty);      }

    std::string RatingAsString()      const { return FloatToString(_Rating, 4);                            }
    std::string PlayersAsString()     const { return IntToRange(_Players);                                 }
    std::string ReleaseDateAsString() const { return _ReleaseDate != 0 ? DateTime((long long)_ReleaseDate).ToCompactISO8601() : ""; }
    std::string PlayCountAsString()   const { return std::to_string(_Playcount);                           }
    std::string LastPlayedAsString()  const { return _LastPlayed != 0 ? DateTime((long long)_LastPlayed).ToCompactISO8601() : ""; }
    std::string FavoriteAsString()    const { return _Favorite ? "true" : "false";                         }
    std::string RomCrc32AsString()    const { std::string r; IntToHex(_RomCrc32, r); return r;             }
    std::string HiddenAsString()      const { return _Hidden ? "true" : "false";                           }

    /*
     * Setters
     */

    void SetName(const std::string& name) { _Name = name; _Dirty = true; }
    void SetEmulator(const std::string& emulator) { AssignPString(_Emulator, emulator); _Dirty = true; }
    void SetCore(const std::string& core) { AssignPString(_Core, core); _Dirty = true; }
    void SetRatio(const std::string& ratio) { AssignPString(_Ratio, ratio); _Dirty = true; }
    void SetDescription(const std::string& description) { _Description = description; _Dirty = true; }
    void SetImagePath(const std::string& image) { _Image = image; _Dirty = true; }
    void SetThumbnailPath(const std::string& thumbnail) { AssignPString(_Thumbnail, thumbnail); _Dirty = true; }
    void SetVideoPath(const std::string& video) { AssignPString(_Video, video); _Dirty = true; }
    void SetReleaseDate(const DateTime& releasedate) { _ReleaseDate = (int)releasedate.ToEpochTime(); _Dirty = true; }
    void SetDeveloper(const std::string& developer) { _Developer = developer; _Dirty = true; }
    void SetPublisher(const std::string& publisher) { _Publisher = publisher; _Dirty = true; }
    void SetGenre(const std::string& genre) { _Genre = genre; _Dirty = true; }
    void SetRating(float rating) { _Rating = rating; _Dirty = true; }
    void SetPlayers(int min, int max)
    {
      _Players = (max << 16) + min;
      _Dirty = true;
    }
    void SetRegion(const std::string& region) { AssignPString(_Region, region); _Dirty = true; }
    void SetRomCrc32(int romcrc32) { _RomCrc32 = romcrc32; _Dirty = true; }
    void SetFavorite(bool favorite) { _Favorite = favorite; _Dirty = true; }
    void SetHidden(bool hidden) { _Hidden = hidden; _Dirty = true; }

    // Special setter to force dirty
    void SetDirty() { _Dirty = true; }

    /*
     * String setters
     */

    void SetReleaseDateAsString(const std::string& releasedate)
    {
      DateTime st;
      _ReleaseDate = DateTime::FromCompactISO6801(releasedate, st) ? (int)st.ToEpochTime() : 0;
      _Dirty = true;
    }
    void SetLastPlayedAsString(const std::string& lastplayed)
    {
      DateTime st;
      _LastPlayed = DateTime::FromCompactISO6801(lastplayed, st) ? (int)st.ToEpochTime() : 0;
      _Dirty = true;
    }
    void SetRatingAsString(const std::string& rating)           { float f = 0.0f; if (StringToFloat(rating, f)) SetRating(f);              }
    void SetPlayersAsString(const std::string& players)         { if (!RangeToInt(players, _Players)) SetPlayers(1, 1);                    }
    void SetFavoriteAsString(const std::string& favorite)       { SetFavorite(favorite == "true");                                         }
    void SetHiddenAsString(const std::string& hidden)           { SetHidden(hidden == "true");                                             }
    void SetRomCrc32AsString(const std::string& romcrc32)       { int c; if (HexToInt(romcrc32, c)) SetRomCrc32(c);                        }
    void SetPlayCountAsString(const std::string& playcount)     { int p; if (StringToInt(playcount, p)) { _Playcount = p; _Dirty = true; } }

    /*
     * Defaults
     */

    bool IsDefaultName()            const { return _Default._Name        == _Name;        }
    bool IsDefaultEmulator()        const { return _Default.Emulator()   == Emulator();   }
    bool IsDefaultCore()            const { return _Default.Core()       == Core();       }
    bool IsDefaultRatio()           const { return _Default.Ratio()      == Ratio();      }
    bool IsDefaultDescription()     const { return _Default._Description == _Description; }
    bool IsDefaultImage()           const { return _Default._Image       == _Image;       }
    bool IsDefaultThumbnail()       const { return _Default.Thumbnail()  == Thumbnail();  }
    bool IsDefaultVideo()           const { return _Default.Video()      == Video();      }
    bool IsDefaultDeveloper()       const { return _Default._Developer   == _Developer;   }
    bool IsDefaultPublisher()       const { return _Default._Publisher   == _Publisher;   }
    bool IsDefaultGenre()           const { return _Default._Genre       == _Genre;       }
    bool IsDefaultRegion()          const { return _Default.Region()     == Region();     }
    bool IsDefaultRating()          const { return _Default._Rating      == _Rating;      }
    bool IsDefaultPlayerRange()     const { return _Default._Players     == _Players;     }
    bool IsDefaultReleaseDateEpoc() const { return _Default._ReleaseDate == _ReleaseDate; }
    bool IsDefaultPlayCount()       const { return _Default._Playcount   == _Playcount;   }
    bool IsDefaultLastPlayedEpoc()  const { return _Default._LastPlayed  == _LastPlayed;  }
    bool IsDefaultRomCrc32()        const { return _Default._RomCrc32    == _RomCrc32;    }
    bool IsDefaultFavorite()        const { return _Default._Favorite    == _Favorite;    }
    bool IsDefaultHidden()          const { return _Default._Hidden      == _Hidden;      }

    /*
     * Convenient Accessors
     */

    //bool IsGame()   const { return _Type == ObjectType::Game;   }
    //bool IsFolder() const { return _Type == ObjectType::Folder; }
    bool IsDirty()  const { return _Dirty;                }

    /*
     * Convenient Methods
     */

    //static bool AreGames(const MetadataDescriptor& md1, const MetadataDescriptor& md2) { return (md1._Type == md2._Type) && md1.IsGame(); }
    //static bool AreFolders(const MetadataDescriptor& md1, const MetadataDescriptor& md2) { return (md1._Type == md2._Type) && md1.IsFolder(); }

    /*
     * Special modifiers
     */

    void IncPlaycount() { _Playcount++; _Dirty = true; }
    void SetLastplayedNow() { _LastPlayed = (int)DateTime().ToEpochTime(); _Dirty = true; }

    /*
     * Metadata FieldManagement Methods
     */

    /*!
     * Return the first static internal field descriptor reference
     * @param count Number of actual field descriptor available
     * @return first static internal field descriptor reference
     */
    const MetadataFieldDescriptor* GetMetadataFieldDescriptors(int& count) { return GetMetadataFieldDescriptors(_Type, count); }
};

