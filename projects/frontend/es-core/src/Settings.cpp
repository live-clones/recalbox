#include <utils/Strings.h>
#include <cstring>
#include <utils/Xml.h>
#include "Settings.h"
#include "utils/Log.h"
#include "pugixml/pugixml.hpp"
#include "RootFolders.h"

#ifdef _RPI_
  #define __VSYNC__ "0"
#else
  #define __VSYNC__ "1"
#endif

Settings::DataDescriptor Settings::sDescriptors[] =
{
  { Settings::DataType::Bool  , offsetof(Settings::Data, mBackgroundJoystickInput), "BackgroundJoystickInput", "0", false },
  { Settings::DataType::Bool  , offsetof(Settings::Data, mDrawFramerate          ), "DrawFramerate"          , "0", false },
  { Settings::DataType::Bool  , offsetof(Settings::Data, mShowExit               ), "ShowExit"               , "1", true  },
  { Settings::DataType::Bool  , offsetof(Settings::Data, mWindowed               ), "Windowed"               , "0", true  },
  { Settings::DataType::Bool  , offsetof(Settings::Data, mUseOSK                 ), "UseOSK"                 , "1", false },
  { Settings::DataType::Bool  , offsetof(Settings::Data, mShowClock              ), "ShowClock"              , "0", false },
  { Settings::DataType::Bool  , offsetof(Settings::Data, mVSync                  ), "VSync"                  , __VSYNC__, true },
  { Settings::DataType::Bool  , offsetof(Settings::Data, mShowHelpPrompts        ), "ShowHelpPrompts"        , "1", false },
  { Settings::DataType::Bool  , offsetof(Settings::Data, mScrapeRatings          ), "ScrapeRatings"          , "1", false },
  { Settings::DataType::Bool  , offsetof(Settings::Data, mMixImages              ), "MixImages"              , "1", false },
  { Settings::DataType::Bool  , offsetof(Settings::Data, mIgnoreGamelist         ), "IgnoreGamelist"         , "0", true  },
  { Settings::DataType::Bool  , offsetof(Settings::Data, mHideConsole            ), "HideConsole"            , "1", true  },
  { Settings::DataType::Bool  , offsetof(Settings::Data, mQuickSystemSelect      ), "QuickSystemSelect"      , "1", false },
  { Settings::DataType::Bool  , offsetof(Settings::Data, mFavoritesOnly          ), "FavoritesOnly"          , "0", false },
  { Settings::DataType::Bool  , offsetof(Settings::Data, mShowHidden             ), "ShowHidden"             , "0", false },
  { Settings::DataType::Bool  , offsetof(Settings::Data, mDebug                  ), "Debug"                  , "0", true  },
  { Settings::DataType::Bool  , offsetof(Settings::Data, mDebugGrid              ), "DebugGrid"              , "0", true  },
  { Settings::DataType::Bool  , offsetof(Settings::Data, mDebugText              ), "DebugText"              , "0", true  },
  { Settings::DataType::Bool  , offsetof(Settings::Data, mMoveCarousel           ), "MoveCarousel"           , "1", false },
  { Settings::DataType::Bool  , offsetof(Settings::Data, mOverscan               ), "Overscan"               , "0", false },
  { Settings::DataType::Bool  , offsetof(Settings::Data, mThemeHasMenuView       ), "ThemeHasMenuView"       , "0", true  },
  { Settings::DataType::Bool  , offsetof(Settings::Data, mThemeChanged           ), "ThemeChanged"           , "0", true  },
  { Settings::DataType::Bool  , offsetof(Settings::Data, mThemeHasHelpSystem     ), "ThemeHasHelpSystem"     , "0", true  },

  { Settings::DataType::Int   , offsetof(Settings::Data, mScreenSaverTime        ), "ScreenSaverTime"        , "300000", false }, // 5 minutes
  { Settings::DataType::Int   , offsetof(Settings::Data, mMusicPopupTime         ), "MusicPopupTime"         , "3"     , false },
  { Settings::DataType::Int   , offsetof(Settings::Data, mScraperResizeWidth     ), "ScraperResizeWidth"     , "400"   , false },
  { Settings::DataType::Int   , offsetof(Settings::Data, mScraperResizeHeight    ), "ScraperResizeHeight"    , "0"     , false },
  { Settings::DataType::Int   , offsetof(Settings::Data, mSystemVolume           ), "SystemVolume"           , "96"    , false },
  { Settings::DataType::Int   , offsetof(Settings::Data, mHelpPopupTime          ), "HelpPopupTime"          , "4"     , false },
  { Settings::DataType::Int   , offsetof(Settings::Data, mNetplayPopupTime       ), "NetplayPopupTime"       , "4"     , false },
  { Settings::DataType::Int   , offsetof(Settings::Data, mMaxVRAM                ), "MaxVRAM"                , "80"    , false },
                                                          
  { Settings::DataType::String, offsetof(Settings::Data, mTransitionStyle        ), "TransitionStyle"        , "fade"                                                     , false },
  { Settings::DataType::String, offsetof(Settings::Data, mPopupPosition          ), "PopupPosition"          , "Top/Right"                                                , false },
  { Settings::DataType::String, offsetof(Settings::Data, mThemeSet               ), "ThemeSet"               , "recalbox-next"                                            , false },
  { Settings::DataType::String, offsetof(Settings::Data, mThemeColorSet          ), "ThemeColorSet"          , ""                                                         , false },
  { Settings::DataType::String, offsetof(Settings::Data, mThemeIconSet           ), "ThemeIconSet"           , ""                                                         , false },
  { Settings::DataType::String, offsetof(Settings::Data, mThemeMenu              ), "ThemeMenu"              , ""                                                         , false },
  { Settings::DataType::String, offsetof(Settings::Data, mThemeSystemView        ), "ThemeSystemView"        , ""                                                         , false },
  { Settings::DataType::String, offsetof(Settings::Data, mThemeGamelistView      ), "ThemeGamelistView"      , ""                                                         , false },
  { Settings::DataType::String, offsetof(Settings::Data, mThemeRegionName        ), "ThemeRegionName"        , "eu"                                                       , false },
  { Settings::DataType::String, offsetof(Settings::Data, mScreenSaverBehavior    ), "ScreenSaverBehavior"    , "dim"                                                      , false },
  { Settings::DataType::String, offsetof(Settings::Data, mScraper                ), "Scraper"                , "Screenscraper"                                            , false },
  { Settings::DataType::String, offsetof(Settings::Data, mLang                   ), "Lang"                   , "en_US"                                                    , false },
  { Settings::DataType::String, offsetof(Settings::Data, mInputName[0]              ), "INPUT P1NAME"               , "DEFAULT"                                                  , false },
  { Settings::DataType::String, offsetof(Settings::Data, mInputName[1]              ), "INPUT P2NAME"               , "DEFAULT"                                                  , false },
  { Settings::DataType::String, offsetof(Settings::Data, mInputName[2]              ), "INPUT P3NAME"               , "DEFAULT"                                                  , false },
  { Settings::DataType::String, offsetof(Settings::Data, mInputName[3]              ), "INPUT P4NAME"               , "DEFAULT"                                                  , false },
  { Settings::DataType::String, offsetof(Settings::Data, mInputName[4]              ), "INPUT P5NAME"               , "DEFAULT"                                                  , false },
  { Settings::DataType::String, offsetof(Settings::Data, mInputName[5]              ), "INPUT P6NAME"               , "DEFAULT"                                                  , false },
  { Settings::DataType::String, offsetof(Settings::Data, mInputName[6]              ), "INPUT P7NAME"               , "DEFAULT"                                                  , false },
  { Settings::DataType::String, offsetof(Settings::Data, mInputName[7]              ), "INPUT P8NAME"               , "DEFAULT"                                                  , false },
  { Settings::DataType::String, offsetof(Settings::Data, mInputName[8]              ), "INPUT P9NAME"               , "DEFAULT"                                                  , false },
  { Settings::DataType::String, offsetof(Settings::Data, mInputName[9]              ), "INPUT P10NAME"              , "DEFAULT"                                                  , false },
  { Settings::DataType::String, offsetof(Settings::Data, mInputGuid[0]              ), "INPUT P1GUID"               , ""                                                  , false },
  { Settings::DataType::String, offsetof(Settings::Data, mInputGuid[1]              ), "INPUT P2GUID"               , ""                                                  , false },
  { Settings::DataType::String, offsetof(Settings::Data, mInputGuid[2]              ), "INPUT P3GUID"               , ""                                                  , false },
  { Settings::DataType::String, offsetof(Settings::Data, mInputGuid[3]              ), "INPUT P4GUID"               , ""                                                  , false },
  { Settings::DataType::String, offsetof(Settings::Data, mInputGuid[4]              ), "INPUT P5GUID"               , ""                                                  , false },
  { Settings::DataType::String, offsetof(Settings::Data, mInputGuid[5]              ), "INPUT P6GUID"               , ""                                                  , false },
  { Settings::DataType::String, offsetof(Settings::Data, mInputGuid[6]              ), "INPUT P7GUID"               , ""                                                  , false },
  { Settings::DataType::String, offsetof(Settings::Data, mInputGuid[7]              ), "INPUT P8GUID"               , ""                                                  , false },
  { Settings::DataType::String, offsetof(Settings::Data, mInputGuid[8]              ), "INPUT P9GUID"               , ""                                                  , false },
  { Settings::DataType::String, offsetof(Settings::Data, mInputGuid[9]              ), "INPUT P10GUID"              , ""                                                  , false },
  { Settings::DataType::String, offsetof(Settings::Data, mOverclock              ), "Overclock"              , "none"                                                     , false },
  { Settings::DataType::String, offsetof(Settings::Data, mUpdateCommand          ), "UpdateCommand"          , "/recalbox/scripts/recalbox-upgrade.sh"                    , true  },
  { Settings::DataType::String, offsetof(Settings::Data, mUpdateServer           ), "UpdateServer"           , "archive.recalbox.com"                                     , true  },
  { Settings::DataType::String, offsetof(Settings::Data, mVersionFile            ), "VersionFile"            , "/recalbox/recalbox.version"                               , true  },
  { Settings::DataType::String, offsetof(Settings::Data, mUpdateVersionFile      ), "UpdateVersionFile"      , "/recalbox/share/system/upgrade/recalbox.version.update"   , false },
  { Settings::DataType::String, offsetof(Settings::Data, mSharePartition         ), "SharePartition"         , "/recalbox/share/"                                         , true  },
  { Settings::DataType::String, offsetof(Settings::Data, mRecalboxSettingScript  ), "RecalboxSettingScript"  , "/recalbox/scripts/recalbox-config.sh"                     , true  },
  { Settings::DataType::String, offsetof(Settings::Data, mRecalboxUpgradeScript  ), "RecalboxUpgradeScript"  , "/recalbox/scripts/upgrade/recalbox-upgrade.sh"            , true  },
  { Settings::DataType::String, offsetof(Settings::Data, mLastChangelog          ), "LastChangelog"          , "/recalbox/share/system/recalbox.changelog.done"           , false },
  { Settings::DataType::String, offsetof(Settings::Data, mUpdateChangelog        ), "UpdateChangelog"        , "/recalbox/share/system/upgrade/recalbox.changelog.update" , false },
  { Settings::DataType::String, offsetof(Settings::Data, mChangelog              ), "Changelog"              , "/recalbox/recalbox.changelog"                             , false },
  { Settings::DataType::String, offsetof(Settings::Data, mMusicDirectory         ), "MusicDirectory"         , "/recalbox/share/music/"                                   , true },
  { Settings::DataType::String, offsetof(Settings::Data, mArch                   ), "Arch"                   , ""                                                         , true },
  { Settings::DataType::String, offsetof(Settings::Data, mDefaultRomsPath        ), "DefaultRomsPath"        , ""                                                         , true },
};

Settings::Settings()
  : StaticLifeCycleControler<Settings>("Settings")
{
  setDefaults();
  loadFile();
}

void Settings::setDefaults()
{
  for(DataDescriptor& desc : sDescriptors)
  {
    void* targetPointer = (char*)&mData + desc.mTarget;
    switch(desc.mType)
    {
      case DataType::Bool: if (!Strings::ToBool(desc.mDefaultValue, *((bool*)targetPointer))) *((bool*)targetPointer) = false; break;
      case DataType::Int: if (!Strings::ToInt(desc.mDefaultValue, *((int*)targetPointer))) *((int*)targetPointer) = 0; break;
      case DataType::Float: if (!Strings::ToFloat(desc.mDefaultValue, *((float*)targetPointer))) *((float*)targetPointer) = 0.0f; break;
      case DataType::String: *((std::string*)targetPointer) = desc.mDefaultValue; break;
    }
  }
}

void Settings::saveFile()
{
  const Path path = RootFolders::DataRootFolder / "/system/.emulationstation/es_settings.cfg";

  pugi::xml_document doc;
  pugi::xml_node config = doc.append_child("config");

  for(DataDescriptor& desc : sDescriptors)
    if (!desc.mPrivate)
    {
      pugi::xml_node node = config.append_child(DataTypeToString(desc.mType));
      node.append_attribute("name").set_value(desc.mName);
      std::string value;
      void* targetPointer = (char*)&mData + desc.mTarget;
      switch(desc.mType)
      {
        case DataType::Bool: value = *((bool*)targetPointer) ? "1" : "0"; break;
        case DataType::Int: value = Strings::ToString(*((int*)targetPointer)); break;
        case DataType::Float: value = Strings::ToString(*((float*)targetPointer), 4); break;
        case DataType::String: value = *((std::string*)targetPointer); break;
      }
      node.append_attribute("value").set_value(value.c_str());
    }

  doc.save_file(path.ToChars());
}

const Settings::DataDescriptor& Settings::LookupDescriptor(const char* name)
{
  for(const DataDescriptor& desc : sDescriptors)
    if (strcmp(name, desc.mName) == 0)
      return desc;

  static DataDescriptor sNull = { Settings::DataType::String, 0, nullptr,
                                  nullptr, true };
  return sNull;
}

void Settings::loadFile()
{
  const Path path = RootFolders::DataRootFolder / "/system/.emulationstation/es_settings.cfg";
  if (!path.Exists()) return;

  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(path.ToChars());
  if (!result)
  {
    LOG(LogError) << "Could not parse Settings file!\n   " << result.description();
    return;
  }

  pugi::xml_node config = doc.child("config");
  for(XmlNode node : config.children())
  {
    // Ignore matching of the xml node name and the real data type
    // Just lookup the name
    const DataDescriptor& desc = LookupDescriptor(node.attribute("name").value());
    if (desc.mName == nullptr)
    {
      LOG(LogError) << "Unknown field " << node.attribute("name").value();
      continue;
    }
    std::string value = node.attribute("value").value();
    void* targetPointer = (char*)&mData + desc.mTarget;
    switch(desc.mType)
    {
      case DataType::Bool: if (!Strings::ToBool(value, *((bool*)targetPointer))) *((bool*)targetPointer) = false; break;
      case DataType::Int: if (!Strings::ToInt(value, *((int*)targetPointer))) *((int*)targetPointer) = 0; break;
      case DataType::Float: if (!Strings::ToFloat(value, *((float*)targetPointer))) *((float*)targetPointer) = 0.0f; break;
      case DataType::String: *((std::string*)targetPointer) = value; break;
    }
  }
}
