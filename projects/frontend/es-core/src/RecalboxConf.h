//
// Created by matthieu on 12/09/15.
//
#pragma once

#include <utils/IniFile.h>
#include <utils/cplusplus/StaticLifeCycleControler.h>
#include <audio/alsa/AlsaController.h>

class RecalboxConf: public IniFile, public StaticLifeCycleControler<RecalboxConf>
{
  public:
    /*!
     * @brief Confstructor
     * @param initialConfigOnly true if only the original file has to be loaded
     */
    explicit RecalboxConf();

    /*!
     * @brief Called when file has been saved
     */
    void OnSave() override;

    /*
     * Shortcuts
     */

    #define DefineGetterSetter(name, type, type2, key, defaultValue) \
      type Get##name() const { return As##type2(key, defaultValue); } \
      void Set##name(const type& value) { Set##type2(key, value); }

    #define DefineGetterSetterParameterized(name, type, type2, keybefore, keyafter, defaultValue) \
      type Get##name(const std::string& subkey) const { return As##type2(std::string(keybefore).append(subkey).append(keyafter), defaultValue); } \
      void Set##name(const std::string& subkey, const type& value) { Set##type2(std::string(keybefore).append(subkey).append(keyafter), value); }

    DefineGetterSetter(Hostname, std::string, String, sHostname, "RECALBOX")

    DefineGetterSetter(WifiEnabled, bool, Bool, sWifiEnabled, false)
    DefineGetterSetter(WifiSSID, std::string, String, sWifiSSID, "")
    DefineGetterSetter(WifiKey, std::string, String, sWifiKey, "")

    DefineGetterSetter(AudioVolume, int, Int, sAudioVolume, 90)
    DefineGetterSetter(AudioMusic, bool, Bool, sAudioMusic, true)
    DefineGetterSetter(AudioOuput, std::string, String, sAudioOuput, AlsaController::sDefaultOutput)

    DefineGetterSetter(ScreenSaverTime, int, Int, sScreenSaverTime, 5)
    DefineGetterSetter(ScreenSaverType, std::string, String, sScreenSaverType, "dim")
    DefineGetterSetter(ScreenSaverSystemList, std::string, String, sScreenSaverSystemList, "")

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
    DefineGetterSetterParameterized(ThemeRegion      , std::string, String, sThemeGeneric, ".region", "")

    DefineGetterSetter(Brightness, int, Int, sBrightness, 7)
    DefineGetterSetter(Clock, bool, Bool, sClock, false)
    DefineGetterSetter(ShowHelp, bool, Bool, sShowHelp, true)
    DefineGetterSetter(QuickSystemSelect, bool, Bool, sQuickSystemSelect, true)

    DefineGetterSetter(FirstTimeUse, bool, Bool, sFirstTimeUse, true)

    DefineGetterSetter(SystemLanguage, std::string, String, sSystemLanguage, "en_US")

    DefineGetterSetter(KodiEnabled, bool, Bool, sKodiEnabled, true)
    DefineGetterSetter(KodiAtStartup, bool, Bool, sKodiAtStartup, false)
    DefineGetterSetter(KodiXButton, bool, Bool, sKodiXButton, true)

    #undef DefineGetterSetter
    #undef DefineGetterSetterParameterized

    static constexpr const char* sHostname              = "system.hostname";

    static constexpr const char* sWifiEnabled           = "wifi.enabled";
    static constexpr const char* sWifiSSID              = "wifi.ssid";
    static constexpr const char* sWifiKey               = "wifi.key";

    static constexpr const char* sAudioVolume           = "audio.volume";
    static constexpr const char* sAudioMusic            = "audio.bgmusic";
    static constexpr const char* sAudioOuput            = "audio.device";

    static constexpr const char* sScreenSaverTime       = "emulationstation.screensaver.time";
    static constexpr const char* sScreenSaverType       = "emulationstation.screensaver.type";
    static constexpr const char* sScreenSaverSystemList = "global.demo.systemlist";

    static constexpr const char* sPopupHelp             = "emulationstation.popoup.help";
    static constexpr const char* sPopupMusic            = "emulationstation.popoup.music";
    static constexpr const char* sPopupNetplay          = "emulationstation.popoup.netplay";

    static constexpr const char* sThemeGeneric          = "emulationstation.theme.";
    static constexpr const char* sThemeCarousel         = "emulationstation.theme.carousel";
    static constexpr const char* sThemeTransition       = "emulationstation.theme.transition";
    static constexpr const char* sThemeFolder           = "emulationstation.theme.folder";

    static constexpr const char* sBrightness            = "emulationstation.brightness";
    static constexpr const char* sClock                 = "emulationstation.clock";
    static constexpr const char* sShowHelp              = "emulationstation.showhelp";
    static constexpr const char* sQuickSystemSelect     = "emulationstation.quicksystemselect";

    static constexpr const char* sFirstTimeUse          = "global.firsttimeuse";
    static constexpr const char* sSystemLanguage        = "system.language";

    static constexpr const char* sKodiEnabled        = "kodi.enabled";
    static constexpr const char* sKodiAtStartup      = "kodi.atstartup";
    static constexpr const char* sKodiXButton        = "kodi.xbutton";
};
