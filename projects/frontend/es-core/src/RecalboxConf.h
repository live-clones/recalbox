//
// Created by matthieu on 12/09/15.
//
#pragma once

#include <utils/IniFile.h>
#include <utils/cplusplus/StaticLifeCycleControler.h>
#include <games/FileSorts.h>
#include <scraping/ScraperTools.h>
#include <scraping/scrapers/screenscraper/ScreenScraperEnums.h>
#include <scraping/scrapers/screenscraper/Languages.h>
#include <audio/AudioMode.h>
#include "systems/SystemSorting.h"
#include <scraping/ScraperType.h>

// Forward declaration
class SystemData;

class RecalboxConf: public IniFile, public StaticLifeCycleControler<RecalboxConf>
{
  public:
    /*!
     * @brief Constructor
     * @param initialConfigOnly true if only the original file has to be loaded
     */
    RecalboxConf();

    /*!
     * @brief Called when file has been saved
     */
    void OnSave() override;

    /*
     * Enums
     */

    enum class Menu
    {
        Default, //!< All menu available
        Bartop,  //!< Limited menu
        None,    //!< No menu except exit
    };

    enum class Relay
    {
        None,     //!< No relay
        NewYork,  //!< Newyork relay
        Madrid,   //!< Madrid relay
        Montreal, //!< Montreal relay
        Saopaulo, //!< SaoPaulo relay
    };

    /*
     * Shortcuts
     */

    #define DefineGetterSetterGeneric(clazz, name, type, type2, key, defaultValue) \
      type Get##name() const { return As##type2(key, defaultValue); } \
      clazz& Delete##name() { Delete(key); return *this; } \
      bool IsDefined##name() const { return IsDefined(key); } \
      clazz& Set##name(const type& value) { Set##type2(key, value); return *this; } \
      bool Has##name() const { return HasKey(key); }

    #define DefineGetterSetter(name, type, type2, key, defaultValue) \
      DefineGetterSetterGeneric(RecalboxConf, name, type, type2, key, defaultValue)

    #define DefineListGetterSetter(name, key, defaultValue) \
      Strings::Vector Get##name() const { return Strings::Split(AsString(key, defaultValue), ','); } \
      bool IsIn##name(const std::string& value) const { return isInList(key, value); } \
      RecalboxConf& Set##name(const Strings::Vector& value) { SetString(key, Strings::Join(value, ',')); return *this; }

    #define DefineGetterSetterParameterized(name, type, type2, keybefore, keyafter, defaultValue) \
      type Get##name(const std::string& subkey) const { return As##type2(std::string(keybefore).append(subkey).append(keyafter), defaultValue); } \
      RecalboxConf& Set##name(const std::string& subkey, const type& value) { Set##type2(std::string(keybefore).append(subkey).append(keyafter), value); return *this; }

    #define DefineSystemGetterSetterImplementation(name, type, type2, key, defaultValue) \
      type RecalboxConf::GetSystem##name(const SystemData& system) const { return As##type2(std::string(system.Name()).append(1, '.').append(key), defaultValue); } \
      RecalboxConf& RecalboxConf::SetSystem##name(const SystemData& system, const type& value) { Set##type2(std::string(system.Name()).append(1, '.').append(key), value); return *this; } \
      RecalboxConf& RecalboxConf::DeleteSystem##name(const SystemData& system) { Delete(std::string(system.Name()).append(1, '.').append(key)); return *this; } \
      bool RecalboxConf::IsDefinedSystem##name(const SystemData& system) const { return IsDefined(std::string(system.Name()).append(1, '.').append(key)); }

    #define DefineEmulationStationSystemGetterSetterImplementation(name, type, type2, key, defaultValue) \
      type RecalboxConf::GetSystem##name(const SystemData& system) const { return As##type2(std::string("emulationstation.").append(system.Name()).append(1, '.').append(key), defaultValue); } \
      RecalboxConf& RecalboxConf::SetSystem##name(const SystemData& system, const type& value) { Set##type2(std::string("emulationstation.").append(system.Name()).append(1, '.').append(key), value); return *this; }

    #define DefineSystemGetterSetterDeclaration(name, type, type2, key) \
      type GetSystem##name(const SystemData& system) const; \
      RecalboxConf& SetSystem##name(const SystemData& system, const type& value); \
      RecalboxConf& DeleteSystem##name(const SystemData& system); \
      bool IsDefinedSystem##name(const SystemData& system) const;

    #define DefineEmulationStationSystemGetterSetterDeclaration(name, type, type2, key) \
      type GetSystem##name(const SystemData& system) const; \
      RecalboxConf& SetSystem##name(const SystemData& system, const type& value);

    #define DefineGetterSetterEnumGeneric(clazz, name, enumType, key, adapterPrefix) \
      enumType Get##name() const { return adapterPrefix##FromString(AsString(key, "")); } \
      clazz& Set##name(enumType value) { SetString(key, adapterPrefix##FromEnum(value)); return *this; }

    #define DefineGetterSetterEnum(name, enumType, key, adapterPrefix) \
       DefineGetterSetterEnumGeneric(RecalboxConf, name, enumType, key, adapterPrefix)

    #define DefineEmulationStationSystemGetterSetterNumericEnumDeclaration(name, enumType) \
      enumType GetSystem##name(const SystemData& system) const; \
      RecalboxConf& SetSystem##name(const SystemData& system, enumType value);

    #define DefineEmulationStationSystemGetterSetterNumericEnumImplementation(name, enumType, key, defaultValue) \
      enumType RecalboxConf::GetSystem##name(const SystemData& system) const { return (enumType)AsInt(std::string("emulationstation.").append(system.Name()).append(1, '.').append(key), (int)(defaultValue)); } \
      RecalboxConf& RecalboxConf::SetSystem##name(const SystemData& system, enumType value) { SetInt(std::string("emulationstation.").append(system.Name()).append(1, '.').append(key), (int)value); return *this; }

    DefineGetterSetterEnum(MenuType, Menu, sMenuType, Menu)
    DefineGetterSetterEnum(ScraperNameOptions, ScraperNameOptions, sScraperGetNameFrom, ScraperTools::ScraperNameOptions)
    DefineGetterSetterEnum(ScreenScraperRegionPriority, ScreenScraperEnums::ScreenScraperRegionPriority, sScreenScraperRegionPriority, ScreenScraperEnums::ScreenScraperRegionPriority)
    DefineGetterSetterEnum(ScreenScraperRegion, Regions::GameRegions, sScreenScraperRegion, Regions::GameRegions)
    DefineGetterSetterEnum(ScreenScraperLanguage, Languages, sScreenScraperLanguage, LanguagesTools::Language)
    DefineGetterSetterEnum(ScreenScraperMainMedia, ScreenScraperEnums::ScreenScraperImageType, sScreenScraperMainMedia, ScreenScraperEnums::ScreenScraperImageType)
    DefineGetterSetterEnum(ScreenScraperThumbnail, ScreenScraperEnums::ScreenScraperImageType, sScreenScraperThumbnail, ScreenScraperEnums::ScreenScraperImageType)
    DefineGetterSetterEnum(ScreenScraperVideo, ScreenScraperEnums::ScreenScraperVideoType, sScreenScraperVideo, ScreenScraperEnums::ScreenScraperVideoType)
    DefineGetterSetterEnum(AudioMode, AudioMode, sAudioOptions, AudioModeTools::AudioMode)
    DefineGetterSetterEnum(SystemSorting, SystemSorting, sSystemSorting, SystemSorting)

    DefineGetterSetter(DebugLogs, bool, Bool, sDebugLogs, false)

    DefineGetterSetter(Hostname, std::string, String, sHostname, "RECALBOX")

    DefineGetterSetter(WifiEnabled, bool, Bool, sWifiEnabled, false)
    DefineGetterSetter(WifiSSID, std::string, String, sWifiSSID, "")
    DefineGetterSetter(WifiKey, std::string, String, sWifiKey, "")

    DefineGetterSetter(SwapValidateAndCancel, bool, Bool, sSwapValidateAndCancel, false)

    DefineGetterSetter(AudioVolume, int, Int, sAudioVolume, 90)
    DefineGetterSetter(AudioOuput, std::string, String, sAudioOuput, "")

    DefineGetterSetter(ScreenSaverTime, int, Int, sScreenSaverTime, 5)
    DefineGetterSetter(ScreenSaverType, std::string, String, sScreenSaverType, "dim")
    DefineListGetterSetter(ScreenSaverSystemList, sScreenSaverSystemList, "")

    DefineGetterSetter(PopupHelp, int, Int, sPopupHelp, 10)
    DefineGetterSetter(PopupMusic, int, Int, sPopupMusic, 5)
    DefineGetterSetter(PopupNetplay, int, Int, sPopupNetplay, 8)

    DefineGetterSetter(ThemeCarousel, bool, Bool, sThemeCarousel, 1)
    DefineGetterSetter(ThemeTransition, std::string, String, sThemeTransition, "slide")
    DefineGetterSetter(ThemeFolder, std::string, String, sThemeFolder, "recalbox-next")

    DefineGetterSetterParameterized(ThemeColorSet    , std::string, String, sThemeGeneric, ".colorset", "")
    DefineGetterSetterParameterized(ThemeIconSet     , std::string, String, sThemeGeneric, ".iconset", "")
    DefineGetterSetterParameterized(ThemeMenuSet     , std::string, String, sThemeGeneric, ".menuset", "")
    DefineGetterSetterParameterized(ThemeSystemView  , std::string, String, sThemeGeneric, ".systemview", "")
    DefineGetterSetterParameterized(ThemeGamelistView, std::string, String, sThemeGeneric, ".gamelistview", "")
    DefineGetterSetterParameterized(ThemeGameClipView, std::string, String, sThemeGeneric, ".gameclipview", "")
    DefineGetterSetterParameterized(ThemeRegion      , std::string, String, sThemeGeneric, ".region", "")

    DefineGetterSetter(Brightness, int, Int, sBrightness, 7)
    DefineGetterSetter(Clock, bool, Bool, sClock, false)
    DefineGetterSetter(ShowHelp, bool, Bool, sShowHelp, true)
    DefineGetterSetter(ShowGameClipHelpItems, bool, Bool, sShowGameClipHelpItems, true)
    DefineGetterSetter(ShowGameClipClippingItem, bool, Bool, sShowGameClipClippingItem, true)
    DefineGetterSetter(QuickSystemSelect, bool, Bool, sQuickSystemSelect, true)
    DefineGetterSetter(FilterAdultGames, bool, Bool, sFilterAdultGames, true)
    DefineGetterSetter(FavoritesOnly, bool, Bool, sFavoritesOnly, false)
    DefineGetterSetter(ShowHidden, bool, Bool, sShowHidden, false)
    DefineGetterSetter(DisplayByFileName, bool, Bool, sDisplayByFileName, false)
    DefineGetterSetter(ShowOnlyLatestVersion, bool, Bool, sShowOnlyLatestVersion, false)
    DefineGetterSetter(HideNoGames, bool, Bool, sHideNoGames, false)

    DefineGetterSetter(FirstTimeUse, bool, Bool, sFirstTimeUse, true)

    DefineGetterSetter(SystemLanguage, std::string, String, sSystemLanguage, "en_US")
    DefineGetterSetter(SystemKbLayout, std::string, String, sSystemKbLayout, "us")
    DefineGetterSetter(SystemManagerEnabled, bool, Bool, sSystemManagerEnabled, true)

    DefineGetterSetter(Overclocking, std::string, String, sOverclocking, "none")
    DefineGetterSetter(Overscan, bool, Bool, sOverscan, false)

    DefineGetterSetter(KodiEnabled, bool, Bool, sKodiEnabled, true)
    DefineGetterSetter(KodiAtStartup, bool, Bool, sKodiAtStartup, false)
    DefineGetterSetter(KodiXButton, bool, Bool, sKodiXButton, false)

    DefineGetterSetterEnum(ScraperSource, ScraperType, sScraperSource, ScraperType)
    DefineGetterSetter(ScraperAuto, bool, Bool, sScraperAuto, true)

    DefineGetterSetter(RecalboxPrivateKey, std::string, String, sRecalboxPrivateKey, "")

    DefineGetterSetter(ScreenScraperLogin, std::string, String, sScreenScraperLogin, "")
    DefineGetterSetter(ScreenScraperPassword, std::string, String, sScreenScraperPassword, "")
    DefineGetterSetter(ScreenScraperWantMarquee, bool, Bool, sScreenScraperWantMarquee, false)
    DefineGetterSetter(ScreenScraperWantWheel, bool, Bool, sScreenScraperWantWheel, false)
    DefineGetterSetter(ScreenScraperWantManual, bool, Bool, sScreenScraperWantManual, false)
    DefineGetterSetter(ScreenScraperWantMaps, bool, Bool, sScreenScraperWantMaps, false)
    DefineGetterSetter(ScreenScraperWantP2K, bool, Bool, sScreenScraperWantP2K, false)

    DefineGetterSetter(NetplayEnabled, bool, Bool, sNetplayEnabled, false)
    DefineGetterSetter(NetplayLogin, std::string, String, sNetplayLogin, "")
    DefineGetterSetter(NetplayLobby, std::string, String, sNetplayLobby, "http://lobby.libretro.com/list/")
    DefineGetterSetter(NetplayPort, int, Int, sNetplayPort, sNetplayDefaultPort)
    DefineGetterSetterEnum(NetplayRelay, Relay, sNetplayRelay, Relay)

    DefineGetterSetter(RetroAchievementOnOff, bool, Bool, sRetroAchievementOnOff, false)
    DefineGetterSetter(RetroAchievementHardcore, bool, Bool, sRetroAchievementHardcore, false)
    DefineGetterSetter(RetroAchievementLogin, std::string, String, sRetroAchievementLogin, "")
    DefineGetterSetter(RetroAchievementPassword, std::string, String, sRetroAchievementPassword, "")

    DefineGetterSetter(StartupGamelistOnly, bool, Bool, sStartupGamelistOnly, false)
    DefineGetterSetter(StartupSelectedSystem, std::string, String, sStartupSelectedSystem, "")
    DefineGetterSetter(StartupStartOnGamelist, bool, Bool, sStartupStartOnGamelist, false)
    DefineGetterSetter(StartupHideSystemView, bool, Bool, sStartupHideSystemView, false)

    DefineGetterSetter(GlobalRatio, std::string, String, sGlobalRatio, "auto")
    DefineGetterSetter(GlobalSmooth, bool, Bool, sGlobalSmooth, true)
    DefineGetterSetter(GlobalRecalboxOverlays, bool, Bool, sGlobalRecalboxOverlays, true)
    DefineGetterSetter(GlobalRewind, bool, Bool, sGlobalRewind, true)
    DefineGetterSetter(GlobalAutoSave, bool, Bool, sGlobalAutoSave, false)
    DefineGetterSetter(GlobalQuitTwice, bool, Bool, sGlobalQuitTwice, false)
    DefineGetterSetter(GlobalHidePreinstalled, bool, Bool, sGlobalHidePreinstalled, false)
    DefineGetterSetter(GlobalIntegerScale, bool, Bool, sGlobalIntegerScale, false)
    DefineGetterSetter(GlobalSoftpatching, std::string, String, sGlobalSoftpatching, "auto")
    DefineGetterSetter(GlobalShaders, std::string, String, sGlobalShaders, "")
    DefineGetterSetter(GlobalShaderSet, std::string, String, sGlobalShaderSet, "none")
    DefineGetterSetter(GlobalShowFPS, bool, Bool, sGlobalShowFPS, false)
    DefineGetterSetter(GlobalInputDriver, std::string, String, sGlobalInputDriver, "auto")
    DefineGetterSetter(GlobalDemoDuration, int, Int, sGlobalDemoDuration, 90)
    DefineGetterSetter(GlobalDemoInfoScreen, int, Int, sGlobalDemoInfoScreen, 6)
    DefineGetterSetter(GlobalZeroLag, bool, Bool, sGlobalZeroLag, false)

    DefineGetterSetter(CollectionLastPlayed, bool, Bool, sCollectionLastPlayed, false)
    DefineGetterSetter(CollectionMultiplayer, bool, Bool, sCollectionMultiplayer, false)
    DefineGetterSetter(CollectionAllGames, bool, Bool, sCollectionAllGames, false)
    DefineGetterSetter(CollectionLightGun, bool, Bool, sCollectionLightGun, false)
    DefineGetterSetter(CollectionPorts, bool, Bool, sCollectionPorts, false)

    DefineGetterSetter(CollectionArcade, bool, Bool, sCollectionArcade, false)
    DefineGetterSetter(CollectionArcadeNeogeo, bool, Bool, sCollectionArcadeNeogeo, true)
    DefineGetterSetter(CollectionArcadeHide, bool, Bool, sCollectionArcadeHide, true)
    DefineGetterSetter(CollectionArcadePosition, int, Int, sCollectionArcadePosition, 0)

    DefineGetterSetter(UpdatesEnabled, bool, Bool, sUpdatesEnabled, true)
    DefineGetterSetter(UpdatesType, std::string, String, sUpdatesType, "stable")

    DefineGetterSetter(EmulationstationVideoMode, std::string, String, sEsVideoMode, "")
    DefineGetterSetter(GlobalVideoMode, std::string, String, sGlobalVideoMode, "")

    DefineGetterSetter(BatteryHidden, bool, Bool, sBatteryHidden, false)

    DefineGetterSetter(Experimental, bool, Bool, sExperimental, GetUpdatesType() != "stable")

    /*
     * System
     */

    DefineSystemGetterSetterDeclaration(Emulator, std::string, String, sSystemEmulator)
    DefineSystemGetterSetterDeclaration(Core, std::string, String, sSystemCore)
    DefineSystemGetterSetterDeclaration(Ratio, std::string, String, sSystemRatio)
    DefineSystemGetterSetterDeclaration(Smooth, bool, Bool, sSystemSmooth)
    DefineSystemGetterSetterDeclaration(Rewind, bool, Bool, sSystemRewind)
    DefineSystemGetterSetterDeclaration(AutoSave, bool, Bool, sSystemAutoSave)
    DefineSystemGetterSetterDeclaration(Shaders, std::string, String, sSystemShaders)
    DefineSystemGetterSetterDeclaration(ShaderSet, std::string, String, sSystemShaderSet)
    DefineSystemGetterSetterDeclaration(Ignore, bool, Bool, sSystemIgnore)
    DefineSystemGetterSetterDeclaration(DemoInclude, bool, Bool, sSystemDemoInclude)
    DefineSystemGetterSetterDeclaration(DemoDuration, int, Int, sSystemDemoDuration)
    DefineSystemGetterSetterDeclaration(VideoMode, std::string, String, sSystemVideoMode)

    DefineEmulationStationSystemGetterSetterDeclaration(FilterAdult, bool, Bool, sSystemFilterAdult)
    DefineEmulationStationSystemGetterSetterDeclaration(FlatFolders, bool, Bool, sSystemFlatFolders)
    DefineEmulationStationSystemGetterSetterNumericEnumDeclaration(Sort, FileSorts::Sorts)
    DefineEmulationStationSystemGetterSetterNumericEnumDeclaration(RegionFilter, Regions::GameRegions)

    #undef DefineGetterSetter
    #undef DefineListGetterSetter
    #undef DefineGetterSetterParameterized

    /*
     * Direct Implementations
     */

    bool GetCollection(const std::string& name) const { return AsBool(std::string(sCollectionHeader).append(1, '.').append(name), false); }
    RecalboxConf& SetCollection(const std::string& name, bool on) { SetBool(std::string(sCollectionHeader).append(1, '.').append(name), on); return *this; }

    std::string GetCollectionTheme(const std::string& name) const { return AsString(std::string(sCollectionHeader).append(1, '.').append(name).append(1, '.').append(sCollectionTheme), std::string("auto-").append(name)); }
    RecalboxConf& SetCollectionTheme(const std::string& name, const std::string& value) { SetString(std::string(sCollectionHeader).append(1, '.').append(name).append(1, '.').append(sCollectionTheme), value); return *this; }

    int GetCollectionLimit(const std::string& name) const { return AsInt(std::string(sCollectionHeader).append(1, '.').append(name).append(1, '.').append(sCollectionLimit), 0); }
    RecalboxConf& SetCollectionLimit(const std::string& name, int limit) { SetInt(std::string(sCollectionHeader).append(1, '.').append(name).append(1, '.').append(sCollectionLimit), limit); return *this; }

    int GetCollectionPosition(const std::string& name) const { return AsInt(std::string(sCollectionHeader).append(1, '.').append(name).append(1, '.').append(sCollectionPosition), 0); }
    RecalboxConf& SetCollectionPosition(const std::string& name, int position) { SetInt(std::string(sCollectionHeader).append(1, '.').append(name).append(1, '.').append(sCollectionPosition), position); return *this; }

    bool GetCollectionHide(const std::string& name) const { return AsBool(std::string(sCollectionHeader).append(1, '.').append(name).append(1, '.').append(sCollectionHide), false); }
    RecalboxConf& SetCollectionHide(const std::string& name, bool hide) { SetBool(std::string(sCollectionHeader).append(1, '.').append(name).append(1, '.').append(sCollectionHide), hide); return *this; }

    std::string GetPad(int index) const { return AsString(std::string(sPadHeader).append(Strings::ToString(index)), ""); }
    RecalboxConf& SetPad(int index, const std::string& padid) { SetString(std::string(sPadHeader).append(Strings::ToString(index)), padid); return *this; }

    /*
     * System keys
     */

    static constexpr const char* sSystemEmulator             = "emulator";
    static constexpr const char* sSystemCore                 = "core";
    static constexpr const char* sSystemRatio                = "ratio";
    static constexpr const char* sSystemSmooth               = "smooth";
    static constexpr const char* sSystemRewind               = "rewind";
    static constexpr const char* sSystemAutoSave             = "autosave";
    static constexpr const char* sSystemIgnore               = "ignore";
    static constexpr const char* sSystemShaders              = "shaders";
    static constexpr const char* sSystemShaderSet            = "shaderset";
    static constexpr const char* sSystemFilterAdult          = "filteradultgames";
    static constexpr const char* sDisplayByFileName          = "displaybyfilename";
    static constexpr const char* sSystemRegionFilter         = "regionfilter";
    static constexpr const char* sSystemFlatFolders          = "flatfolders";
    static constexpr const char* sSystemSort                 = "sort";
    static constexpr const char* sSystemDemoInclude          = "demo.include";
    static constexpr const char* sSystemDemoDuration         = "demo.duration";
    static constexpr const char* sSystemVideoMode            = "videomode";

    /*
     * Collection Keys
     */

    static constexpr const char* sCollectionHide             = "hide";
    static constexpr const char* sCollectionTheme            = "theme";
    static constexpr const char* sCollectionLimit            = "limit";
    static constexpr const char* sCollectionPosition         = "position";

    /*
     * Key headers
     */

    static constexpr const char* sCollectionHeader           = "emulationstation.collection";

    /*
     * Keys
     */

    static constexpr const char* sExperimental               = "global.experimental";

    static constexpr const char* sGlobalRatio                = "global.ratio";
    static constexpr const char* sGlobalSmooth               = "global.smooth";
    static constexpr const char* sGlobalRecalboxOverlays     = "global.recalboxoverlays";
    static constexpr const char* sGlobalRewind               = "global.rewind";
    static constexpr const char* sGlobalAutoSave             = "global.autosave";
    static constexpr const char* sGlobalSoftpatching         = "global.softpatching";
    static constexpr const char* sGlobalShaders              = "global.shaders";
    static constexpr const char* sGlobalShaderSet            = "global.shaderset";
    static constexpr const char* sGlobalQuitTwice            = "global.quitpresstwice";
    static constexpr const char* sGlobalHidePreinstalled     = "global.hidepreinstalledgames";
    static constexpr const char* sGlobalIntegerScale         = "global.integerscale";
    static constexpr const char* sGlobalShowFPS              = "global.showfps";
    static constexpr const char* sGlobalDemoDuration         = "global.demo.duration";
    static constexpr const char* sGlobalDemoInfoScreen       = "global.demo.infoscreenduration";
    static constexpr const char* sGlobalZeroLag              = "global.zerolag";

    static constexpr const char* sGlobalInputDriver          = "global.inputdriver";

    static constexpr const char* sHostname                   = "system.hostname";

    static constexpr const char* sWifiEnabled                = "wifi.enabled";
    static constexpr const char* sWifiSSID                   = "wifi.ssid";
    static constexpr const char* sWifiKey                    = "wifi.key";

    static constexpr const char* sSwapValidateAndCancel      = "controllers.swapvalidateandcancel";

    static constexpr const char* sAudioVolume                = "audio.volume";
    static constexpr const char* sAudioOptions               = "audio.mode";
    static constexpr const char* sAudioOuput                 = "audio.device";

    static constexpr const char* sScreenSaverTime            = "emulationstation.screensaver.time";
    static constexpr const char* sScreenSaverType            = "emulationstation.screensaver.type";
    static constexpr const char* sScreenSaverSystemList      = "global.demo.systemlist";

    static constexpr const char* sPopupHelp                  = "emulationstation.popoup.help";
    static constexpr const char* sPopupMusic                 = "emulationstation.popoup.music";
    static constexpr const char* sPopupNetplay               = "emulationstation.popoup.netplay";

    static constexpr const char* sThemeGeneric               = "emulationstation.theme.";
    static constexpr const char* sThemeCarousel              = "emulationstation.theme.carousel";
    static constexpr const char* sThemeTransition            = "emulationstation.theme.transition";
    static constexpr const char* sThemeFolder                = "emulationstation.theme.folder";

    static constexpr const char* sBrightness                 = "emulationstation.brightness";
    static constexpr const char* sClock                      = "emulationstation.clock";
    static constexpr const char* sShowHelp                   = "emulationstation.showhelp";
    static constexpr const char* sShowGameClipHelpItems      = "emulationstation.showgamecliphelpitems";
    static constexpr const char* sShowGameClipClippingItem   = "emulationstation.showgameclipclippingitem";
    static constexpr const char* sQuickSystemSelect          = "emulationstation.quicksystemselect";
    static constexpr const char* sFilterAdultGames           = "emulationstation.filteradultgames";
    static constexpr const char* sFavoritesOnly              = "emulationstation.favoritesonly";
    static constexpr const char* sShowHidden                 = "emulationstation.showhidden";
    static constexpr const char* sShowOnlyLatestVersion      = "emulationstation.showonlylatestversion";
    static constexpr const char* sHideNoGames                = "emulationstation.hidenogames";

    static constexpr const char* sSystemSorting              = "emulationstation.systemsorting";

    static constexpr const char* sBatteryHidden              = "emulationstation.battery.hidden";

    static constexpr const char* sEsVideoMode                = "system.es.videomode";
    static constexpr const char* sGlobalVideoMode            = "global.videomode";

    static constexpr const char* sFirstTimeUse               = "system.firsttimeuse";
    static constexpr const char* sSystemLanguage             = "system.language";
    static constexpr const char* sSystemKbLayout             = "system.kblayout";
    static constexpr const char* sSystemManagerEnabled       = "system.manager.enabled";

    static constexpr const char* sOverclocking               = "system.overclocking";
    static constexpr const char* sOverscan                   = "system.overscan";

    static constexpr const char* sKodiEnabled                = "kodi.enabled";
    static constexpr const char* sKodiAtStartup              = "kodi.atstartup";
    static constexpr const char* sKodiXButton                = "kodi.xbutton";

    static constexpr const char* sScraperSource              = "scraper.source";
    static constexpr const char* sScraperAuto              = "scraper.auto";
    static constexpr const char* sScraperGetNameFrom         = "scraper.getnamefrom";

    static constexpr const char* sRecalboxPrivateKey         = "patron.privatekey";

    static constexpr const char* sScreenScraperLogin         = "scraper.screenscraper.user";
    static constexpr const char* sScreenScraperPassword      = "scraper.screenscraper.password";
    static constexpr const char* sScreenScraperRegionPriority= "scraper.screenscraper.regionPriority";
    static constexpr const char* sScreenScraperRegion        = "scraper.screenscraper.region";
    static constexpr const char* sScreenScraperLanguage      = "scraper.screenscraper.language";
    static constexpr const char* sScreenScraperMainMedia     = "scraper.screenscraper.media";
    static constexpr const char* sScreenScraperThumbnail     = "scraper.screenscraper.thumbnail";
    static constexpr const char* sScreenScraperVideo         = "scraper.screenscraper.video";
    static constexpr const char* sScreenScraperWantMarquee   = "scraper.screenscraper.marquee";
    static constexpr const char* sScreenScraperWantWheel     = "scraper.screenscraper.wheel";
    static constexpr const char* sScreenScraperWantManual    = "scraper.screenscraper.manual";
    static constexpr const char* sScreenScraperWantMaps      = "scraper.screenscraper.maps";
    static constexpr const char* sScreenScraperWantP2K       = "scraper.screenscraper.p2k";

    static constexpr const char* sNetplayEnabled             = "global.netplay.active";
    static constexpr const char* sNetplayLogin               = "global.netplay.nickname";
    static constexpr const char* sNetplayLobby               = "global.netplay.lobby";
    static constexpr const char* sNetplayPort                = "global.netplay.port";
    static constexpr const char* sNetplayRelay               = "global.netplay.relay";

    static constexpr const char* sRetroAchievementOnOff      = "global.retroachievements";
    static constexpr const char* sRetroAchievementHardcore   = "global.retroachievements.hardcore";
    static constexpr const char* sRetroAchievementLogin      = "global.retroachievements.username";
    static constexpr const char* sRetroAchievementPassword   = "global.retroachievements.password";

    static constexpr const char* sStartupGamelistOnly        = "emulationstation.gamelistonly";
    static constexpr const char* sStartupSelectedSystem      = "emulationstation.selectedsystem";
    static constexpr const char* sStartupStartOnGamelist     = "emulationstation.bootongamelist";
    static constexpr const char* sStartupHideSystemView      = "emulationstation.hidesystemview";

    static constexpr const char* sMenuType                   = "emulationstation.menu";
    static constexpr const char* sHideSystemView             = "emulationstation.hidesystemview";
    static constexpr const char* sBootOnGamelist             = "emulationstation.bootongamelist";
    static constexpr const char* sForceBasicGamelistView     = "emulationstation.forcebasicgamelistview";

    static constexpr const char* sCollectionLastPlayed       = "emulationstation.collection.lastplayed";
    static constexpr const char* sCollectionMultiplayer      = "emulationstation.collection.multiplayer";
    static constexpr const char* sCollectionAllGames         = "emulationstation.collection.allgames";
    static constexpr const char* sCollectionLightGun         = "emulationstation.collection.lightgun";
    static constexpr const char* sCollectionPorts            = "emulationstation.collection.ports";

    static constexpr const char* sCollectionArcade           = "emulationstation.arcade";
    static constexpr const char* sCollectionArcadeNeogeo     = "emulationstation.arcade.includeneogeo";
    static constexpr const char* sCollectionArcadeHide       = "emulationstation.arcade.hideoriginals";
    static constexpr const char* sCollectionArcadePosition   = "emulationstation.arcade.position";

    static constexpr const char* sUpdatesEnabled             = "updates.enabled";
    static constexpr const char* sUpdatesType                = "updates.type";

    static constexpr const char* sPadHeader                  = "emulationstation.pad";

    static constexpr const char* sDebugLogs                  = "emulationstation.debuglogs";

    static constexpr const int sNetplayDefaultPort           = 55435;

  private:
    /*
     * Culture
     */

    static std::string GetLanguage();
    static std::string GetCountry();

    /*
     * Enumeration getter/setter
     */

    static Menu MenuFromString(const std::string& menu);
    static const std::string& MenuFromEnum(Menu menu);
    static Relay RelayFromString(const std::string& relay);
    static const std::string& RelayFromEnum(Relay relay);
    static SystemSorting SystemSortingFromString(const std::string& systemSorting);
    static const std::string& SystemSortingFromEnum(SystemSorting systemSorting);
    static ScraperType ScraperTypeFromString(const std::string& menu);
    static const std::string& ScraperTypeFromEnum(ScraperType type);
};
