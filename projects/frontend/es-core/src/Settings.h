#pragma once

#include <string>

//This is a singleton for storing settings.
class Settings
{
  private:
    enum class DataType
    {
      Bool,
      Int,
      Float,
      String,
    };

    struct DataDescriptor
    {
      DataType      mType;
      unsigned long mTarget;
      const char*   mName;
      const char*   mDefaultValue;
      bool          mPrivate;
    };

    static DataDescriptor sDescriptors[];

    struct Data
    {
      bool mBackgroundJoystickInput;
      bool mDrawFramerate;
      bool mShowExit;
      bool mWindowed;
      bool mUseOSK;
      bool mShowClock;
      bool mVSync;
      bool mShowHelpPrompts;
      bool mScrapeRatings;
      bool mMixImages;
      bool mIgnoreGamelist;
      bool mHideConsole;
      bool mQuickSystemSelect;
      bool mFavoritesOnly;
      bool mShowHidden;
      bool mDebug;
      bool mDebugGrid;
      bool mDebugText;
      bool mMoveCarousel;
      bool mOverscan;

      bool mThemeHasMenuView;
      bool mThemeChanged;
      bool mThemeHasHelpSystem;

      int mScreenSaverTime;
      int mMusicPopupTime;
      int mScraperResizeWidth;
      int mScraperResizeHeight;
      int mSystemVolume;
      int mHelpPopupTime;
      int mNetplayPopupTime;
      int mMaxVRAM;

      std::string mTransitionStyle;
      std::string mPopupPosition;
      std::string mThemeSet;
      std::string mThemeColorSet;
      std::string mThemeIconSet;
      std::string mThemeMenu;
      std::string mThemeSystemView;
      std::string mThemeGamelistView;
      std::string mThemeRegionName;
      std::string mScreenSaverBehavior;
      std::string mScraper;
      std::string mLang;
      std::string mInputName[5];
      std::string mInputGuid[5];
      std::string mOverclock;
      std::string mUpdateCommand;
      std::string mUpdateServer;
      std::string mVersionFile;
      std::string mUpdateVersionFile;
      std::string mSharePartition;
      std::string mRecalboxSettingScript;
      std::string mRecalboxUpgradeScript;
      std::string mLastChangelog;
      std::string mUpdateChangelog;
      std::string mChangelog;
      std::string mMusicDirectory;

      std::string mArch;
      std::string mDefaultRomsPath;
    };

    Data mData;

    static const char* DataTypeToString(DataType type)
    {
      switch(type)
      {
        case DataType::Bool: return "bool";
        case DataType::Int: return "int";
        case DataType::Float: return "float";
        case DataType::String: return "string";
        default: break;
      }
      return "unknown";
    }

    static const DataDescriptor& LookupDescriptor(const char* name) ;

    Settings();

    //Clear everything and load default values.
    void setDefaults();

    void loadFile();

  public:
    static Settings& Instance();
    void saveFile();

    /*
     * Accessors
     */
  	
    bool BackgroundJoystickInput() const { return mData.mBackgroundJoystickInput; }   
    bool DrawFramerate          () const { return mData.mDrawFramerate;           }   
    bool ShowExit               () const { return mData.mShowExit;                }   
    bool Windowed               () const { return mData.mWindowed;                }   
    bool UseOSK                 () const { return mData.mUseOSK;                  }   
    bool ShowClock              () const { return mData.mShowClock;               }   
    bool VSync                  () const { return mData.mVSync;                   }   
    bool ShowHelpPrompts        () const { return mData.mShowHelpPrompts;         }   
    bool ScrapeRatings          () const { return mData.mScrapeRatings;           }   
    bool MixImages              () const { return mData.mMixImages;               }   
    bool IgnoreGamelist         () const { return mData.mIgnoreGamelist;          }   
    bool HideConsole            () const { return mData.mHideConsole;             }   
    bool QuickSystemSelect      () const { return mData.mQuickSystemSelect;       }   
    bool FavoritesOnly          () const { return mData.mFavoritesOnly;           }   
    bool ShowHidden             () const { return mData.mShowHidden;              }   
    bool Debug                  () const { return mData.mDebug;                   }   
    bool DebugGrid              () const { return mData.mDebugGrid;               }   
    bool DebugText              () const { return mData.mDebugText;               }   
    bool MoveCarousel           () const { return mData.mMoveCarousel;            }   
    bool Overscan               () const { return mData.mOverscan;                }   
                                                               
    bool ThemeHasMenuView       () const { return mData.mThemeHasMenuView;        }
    bool ThemeChanged           () const { return mData.mThemeChanged;            }
    bool ThemeHasHelpSystem     () const { return mData.mThemeHasHelpSystem;      }
                                                                         
    int ScreenSaverTime         () const { return mData.mScreenSaverTime;          }
    int MusicPopupTime          () const { return mData.mMusicPopupTime;           }
    int ScraperResizeWidth      () const { return mData.mScraperResizeWidth;       }
    int ScraperResizeHeight     () const { return mData.mScraperResizeHeight;      }
    int SystemVolume            () const { return mData.mSystemVolume;             }
    int HelpPopupTime           () const { return mData.mHelpPopupTime;            }
    int NetplayPopupTime        () const { return mData.mNetplayPopupTime;         }
    int MaxVRAM                 () const { return mData.mMaxVRAM;                  }
                                                                         
    const std::string& TransitionStyle      () const { return mData.mTransitionStyle;       }
    const std::string& PopupPosition        () const { return mData.mPopupPosition;         }
    const std::string& ThemeSet             () const { return mData.mThemeSet;              }
    const std::string& ThemeColorSet        () const { return mData.mThemeColorSet;         }
    const std::string& ThemeIconSet         () const { return mData.mThemeIconSet;          }
    const std::string& ThemeMenu            () const { return mData.mThemeMenu;             }
    const std::string& ThemeSystemView      () const { return mData.mThemeSystemView;       }
    const std::string& ThemeGamelistView    () const { return mData.mThemeGamelistView;     }
    const std::string& ThemeRegionName      () const { return mData.mThemeRegionName;       }
    const std::string& ScreenSaverBehavior  () const { return mData.mScreenSaverBehavior;   }
    const std::string& Scraper              () const { return mData.mScraper;               }
    const std::string& Lang                 () const { return mData.mLang;                  }
    const std::string& Overclock            () const { return mData.mOverclock;             }
    const std::string& UpdateCommand        () const { return mData.mUpdateCommand;         }
    const std::string& UpdateServer         () const { return mData.mUpdateServer;          }
    const std::string& VersionFile          () const { return mData.mVersionFile;           }
    const std::string& UpdateVersionFile    () const { return mData.mUpdateVersionFile;     }
    const std::string& SharePartition       () const { return mData.mSharePartition;        }
    const std::string& RecalboxSettingScript() const { return mData.mRecalboxSettingScript; }
    const std::string& RecalboxUpgradeScript() const { return mData.mRecalboxUpgradeScript; }
    const std::string& LastChangelog        () const { return mData.mLastChangelog;         }
    const std::string& UpdateChangelog      () const { return mData.mUpdateChangelog;       }
    const std::string& Changelog            () const { return mData.mChangelog;             }
    const std::string& MusicDirectory 	    () const { return mData.mMusicDirectory;	      }

    const std::string& Arch                 () const { return mData.mArch;                  }
    const std::string& DefaultRomsPath      () const { return mData.mDefaultRomsPath;       }

    std::string InputName(int index) const { return (index < (int)sizeof(mData.mInputName)) ? mData.mInputName[index] : ""; }
    std::string InputGuid(int index) const { return (index < (int)sizeof(mData.mInputGuid)) ? mData.mInputGuid[index] : ""; }

    void SetBackgroundJoystickInput(bool value) { mData.mBackgroundJoystickInput = value; }   
    void SetDrawFramerate          (bool value) { mData.mDrawFramerate           = value; }   
    void SetShowExit               (bool value) { mData.mShowExit                = value; }   
    void SetWindowed               (bool value) { mData.mWindowed                = value; }   
    void SetUseOSK                 (bool value) { mData.mUseOSK                  = value; }   
    void SetShowClock              (bool value) { mData.mShowClock               = value; }   
    void SetVSync                  (bool value) { mData.mVSync                   = value; }   
    void SetShowHelpPrompts        (bool value) { mData.mShowHelpPrompts         = value; }   
    void SetScrapeRatings          (bool value) { mData.mScrapeRatings           = value; }   
    void SetMixImages              (bool value) { mData.mMixImages               = value; }   
    void SetIgnoreGamelist         (bool value) { mData.mIgnoreGamelist          = value; }   
    void SetHideConsole            (bool value) { mData.mHideConsole             = value; }   
    void SetQuickSystemSelect      (bool value) { mData.mQuickSystemSelect       = value; }   
    void SetFavoritesOnly          (bool value) { mData.mFavoritesOnly           = value; }   
    void SetShowHidden             (bool value) { mData.mShowHidden              = value; }   
    void SetDebug                  (bool value) { mData.mDebug                   = value; }   
    void SetDebugGrid              (bool value) { mData.mDebugGrid               = value; }   
    void SetDebugText              (bool value) { mData.mDebugText               = value; }   
    void SetMoveCarousel           (bool value) { mData.mMoveCarousel            = value; }   
    void SetOverscan               (bool value) { mData.mOverscan                = value; }   
                                                                         
    void SetThemeHasMenuView       (bool value) { mData.mThemeHasMenuView        = value; }
    void SetThemeChanged           (bool value) { mData.mThemeChanged            = value; }
    void SetThemeHasHelpSystem     (bool value) { mData.mThemeHasHelpSystem      = value; }
                                                                         
    void SetScreenSaverTime        (int  value) { mData.mScreenSaverTime          = value; }
    void SetMusicPopupTime         (int  value) { mData.mMusicPopupTime           = value; }
    void SetScraperResizeWidth     (int  value) { mData.mScraperResizeWidth       = value; }
    void SetScraperResizeHeight    (int  value) { mData.mScraperResizeHeight      = value; }
    void SetSystemVolume           (int  value) { mData.mSystemVolume             = value; }
    void SetHelpPopupTime          (int  value) { mData.mHelpPopupTime            = value; }
    void SetNetplayPopupTime       (int  value) { mData.mNetplayPopupTime         = value; }
    void SetMaxVRAM                (int  value) { mData.mMaxVRAM                  = value; }

    void SetTransitionStyle        (const std::string& value) { mData.mTransitionStyle       = value; }
    void SetPopupPosition          (const std::string& value) { mData.mPopupPosition         = value; }
    void SetThemeSet               (const std::string& value) { mData.mThemeSet              = value; }
    void SetThemeColorSet          (const std::string& value) { mData.mThemeColorSet         = value; }
    void SetThemeIconSet           (const std::string& value) { mData.mThemeIconSet          = value; }
    void SetThemeMenu              (const std::string& value) { mData.mThemeMenu             = value; }
    void SetThemeSystemView        (const std::string& value) { mData.mThemeSystemView       = value; }
    void SetThemeGamelistView      (const std::string& value) { mData.mThemeGamelistView     = value; }
    void SetThemeRegionName        (const std::string& value) { mData.mThemeRegionName       = value; }
    void SetScreenSaverBehavior    (const std::string& value) { mData.mScreenSaverBehavior   = value; }
    void SetScraper                (const std::string& value) { mData.mScraper               = value; }
    void SetLang                   (const std::string& value) { mData.mLang                  = value; }
    void SetOverclock              (const std::string& value) { mData.mOverclock             = value; }
    void SetUpdateCommand          (const std::string& value) { mData.mUpdateCommand         = value; }
    void SetUpdateServer           (const std::string& value) { mData.mUpdateServer          = value; }
    void SetVersionFile            (const std::string& value) { mData.mVersionFile           = value; }
    void SetUpdateVersionFile      (const std::string& value) { mData.mUpdateVersionFile     = value; }
    void SetSharePartition         (const std::string& value) { mData.mSharePartition        = value; }
    void SetRecalboxSettingScript  (const std::string& value) { mData.mRecalboxSettingScript = value; }
    void SetRecalboxUpgradeScript  (const std::string& value) { mData.mRecalboxUpgradeScript = value; }
    void SetLastChangelog          (const std::string& value) { mData.mLastChangelog         = value; }
    void SetUpdateChangelog        (const std::string& value) { mData.mUpdateChangelog       = value; }
    void SetChangelog              (const std::string& value) { mData.mChangelog             = value; }
    void SetMusicDirectory	       (const std::string& value) { mData.mMusicDirectory	       = value; }

    void SetArch                   (const std::string& value) { mData.mArch                  = value; }
    void SetDefaultRomsPath        (const std::string& value) { mData.mDefaultRomsPath       = value; }

    void SetInputName(int index, const std::string& value) { if (index < (int)sizeof(mData.mInputName)) mData.mInputName[index] = value; }
    void SetInputGuid(int index, const std::string& value) { if (index < (int)sizeof(mData.mInputGuid)) mData.mInputGuid[index] = value; }
};
