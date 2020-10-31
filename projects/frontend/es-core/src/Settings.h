#pragma once

#include <string>
#include <utils/cplusplus/StaticLifeCycleControler.h>

//This is a singleton for storing settings.
class Settings : public StaticLifeCycleControler<Settings>
{
  private:
    static constexpr int sMaxJoysticks = 10;

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

    static DataDescriptor const sDescriptors[];

    struct Data
    {
      bool mBackgroundJoystickInput;
      bool mDrawFramerate;
      bool mShowExit;
      bool mWindowed;
      bool mVSync;
      bool mScrapeRatings;
      bool mMixImages;
      bool mIgnoreGamelist;
      bool mHideConsole;
      bool mFavoritesOnly;
      bool mShowHidden;
      bool mDebug;
      bool mDebugGrid;
      bool mDebugText;
      bool mOverscan;

      bool mThemeHasMenuView;
      bool mThemeHasHelpSystem;

      int mScreenSaverTime;
      int mScraperResizeWidth;
      int mScraperResizeHeight;
      int mMaxVRAM;

      std::string mScreenSaverBehavior;
      std::string mScraper;
      std::string mLang;
      std::string mInputName[sMaxJoysticks];
      std::string mInputGuid[sMaxJoysticks];
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

    //Clear everything and load default values.
    void setDefaults();

    void loadFile();

  public:
    /*!
     * @brief Default constructor
     */
    Settings();

    /*!
     * @brief Default destructor
     */
    ~Settings() = default;

    void saveFile();

    /*
     * Accessors
     */
  	
    bool BackgroundJoystickInput() const { return mData.mBackgroundJoystickInput; }   
    bool DrawFramerate          () const { return mData.mDrawFramerate;           }   
    bool ShowExit               () const { return mData.mShowExit;                }   
    bool Windowed               () const { return mData.mWindowed;                }   
    bool VSync                  () const { return mData.mVSync;                   }
    bool ScrapeRatings          () const { return mData.mScrapeRatings;           }
    bool MixImages              () const { return mData.mMixImages;               }   
    bool IgnoreGamelist         () const { return mData.mIgnoreGamelist;          }   
    bool HideConsole            () const { return mData.mHideConsole;             }   
    bool FavoritesOnly          () const { return mData.mFavoritesOnly;           }
    bool ShowHidden             () const { return mData.mShowHidden;              }   
    bool Debug                  () const { return mData.mDebug;                   }   
    bool DebugGrid              () const { return mData.mDebugGrid;               }   
    bool DebugText              () const { return mData.mDebugText;               }   
    bool Overscan               () const { return mData.mOverscan;                }
                                                               
    bool ThemeHasMenuView       () const { return mData.mThemeHasMenuView;        }
    bool ThemeHasHelpSystem     () const { return mData.mThemeHasHelpSystem;      }
                                                                         
    int ScreenSaverTime         () const { return mData.mScreenSaverTime;          }
    int ScraperResizeWidth      () const { return mData.mScraperResizeWidth;       }
    int ScraperResizeHeight     () const { return mData.mScraperResizeHeight;      }
    int MaxVRAM                 () const { return mData.mMaxVRAM;                  }
                                                                         
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

    std::string InputName(int index) const { return (index < sMaxJoysticks) ? mData.mInputName[index] : ""; }
    std::string InputGuid(int index) const { return (index < sMaxJoysticks) ? mData.mInputGuid[index] : ""; }

    void SetBackgroundJoystickInput(bool value) { mData.mBackgroundJoystickInput = value; }   
    void SetDrawFramerate          (bool value) { mData.mDrawFramerate           = value; }   
    void SetShowExit               (bool value) { mData.mShowExit                = value; }   
    void SetWindowed               (bool value) { mData.mWindowed                = value; }   
    void SetVSync                  (bool value) { mData.mVSync                   = value; }
    void SetScrapeRatings          (bool value) { mData.mScrapeRatings           = value; }
    void SetMixImages              (bool value) { mData.mMixImages               = value; }   
    void SetIgnoreGamelist         (bool value) { mData.mIgnoreGamelist          = value; }   
    void SetHideConsole            (bool value) { mData.mHideConsole             = value; }   
    void SetFavoritesOnly          (bool value) { mData.mFavoritesOnly           = value; }
    void SetShowHidden             (bool value) { mData.mShowHidden              = value; }   
    void SetDebug                  (bool value) { mData.mDebug                   = value; }   
    void SetDebugGrid              (bool value) { mData.mDebugGrid               = value; }   
    void SetDebugText              (bool value) { mData.mDebugText               = value; }   
    void SetOverscan               (bool value) { mData.mOverscan                = value; }
                                                                         
    void SetThemeHasMenuView       (bool value) { mData.mThemeHasMenuView        = value; }
    void SetThemeHasHelpSystem     (bool value) { mData.mThemeHasHelpSystem      = value; }
                                                                         
    void SetScreenSaverTime        (int  value) { mData.mScreenSaverTime          = value; }
    void SetScraperResizeWidth     (int  value) { mData.mScraperResizeWidth       = value; }
    void SetScraperResizeHeight    (int  value) { mData.mScraperResizeHeight      = value; }
    void SetMaxVRAM                (int  value) { mData.mMaxVRAM                  = value; }

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
