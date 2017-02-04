//
// Created by matthieu on 28/01/17.
//

#ifndef EMULATIONSTATION_ALL_MENUMESSAGES_H
#define EMULATIONSTATION_ALL_MENUMESSAGES_H

#include <string>

class MenuMessages {
public:
    static const std::string START_KODI_HELP_MSG;
    static const std::string SYSTEM_HELP_MSG;
    static const std::string VERSION_HELP_MSG;
    static const std::string DISK_USAGE_HELP_MSG;
    static const std::string STORAGE_DEVICE_HELP_MSG;
    static const std::string LANGUAGE_HELP_MSG;

    static const std::string UPDATE_HELP_MSG;
    static const std::string START_UPDATE_HELP_MSG;
    static const std::string UPDATE_TYPE_HELP_MSG;
    static const std::string UPDATE_CHECK_HELP_MSG;

    static const std::string GAME_SETTINGS_HELP_MSG;
    static const std::string GAME_RATIO_HELP_MSG;
    static const std::string GAME_SMOOTH_HELP_MSG;
    static const std::string GAME_REWIND_HELP_MSG;
    static const std::string GAME_AUTOSAVELOAD_HELP_MSG;
    static const std::string GAME_INTEGER_SCALE_HELP_MSG;
    static const std::string GAME_SHADERS_HELP_MSG;

    static const std::string RA_ONOFF_HELP_MSG;
    static const std::string RA_HARDCORE_HELP_MSG;
    static const std::string RA_HELP_MSG;

    static const std::string CONTROLLER_HELP_MSG;
    static const std::string CONTROLLER_BT_HELPMSG;
    static const std::string CONTROLLER_FORGET_HELPMSG;

    static const std::string UI_HELP_MSG;
    static const std::string UI_SCREENSAVER_AFTER_HELP_MSG;
    static const std::string UI_SCREENSAVER_BEHAVIOR_HELP_MSG;
    static const std::string UI_ONSCREENHELP_HELP_MSG;
    static const std::string UI_QUICK_HELP_MSG;
    static const std::string UI_KEYBOARD_HELP_MSG;
    static const std::string UI_TRANSITION_HELP_MSG;
    static const std::string UI_THEME_HELP_MSG;
    static const std::string UI_UPDATE_GAMELIST_HELP_MSG;

    static const std::string SOUND_HELP_MSG;
    static const std::string SOUND_VOLUME_HELP_MSG;
    static const std::string SOUND_FRONTEND_MUSIC_HELP_MSG;
    static const std::string SOUND_DEVICE_HELP_MSG;

    static const std::string NETWORK_HELP_MSG;
    static const std::string NETWORK_STATUS_HELP_MSG;
    static const std::string NETWORK_IP_HELP_MSG;
    static const std::string NETWORK_WIFI_HELP_MSG;
    static const std::string NETWORK_HOST_HELP_MSG;
    static const std::string NETWORK_SSID_HELP_MSG;
    static const std::string NETWORK_KEY_HELP_MSG;

    static const std::string SCRAPER_HELP_MSG;
    static const std::string SCRAPER_FROM_HELP_MSG;
    static const std::string SCRAPER_NOW_HELP_MSG;
    static const std::string SCRAPER_RATINGS_HELP_MSG;

    static const std::string ADVANCED_HELP_MSG;
    static const std::string ADVANCED_OVERCLOCK_HELP_MSG;
    static const std::string ADVANCED_BOOT_ON_SYSTEM_HELP_MSG;
    static const std::string ADVANCED_BOOTGAMELIST_HELP_MSG;
    static const std::string ADVANCED_GAMELISTONLY_HELP_MSG;
    static const std::string ADVANCED_HIDESYSTEMVIEW_HELP_MSG;
    static const std::string ADVANCED_EMULATOR_ADVANCED_HELP_MSG;
    static const std::string ADVANCED_BOOT_HELP_MSG;

    static const std::string ADVANCED_KODI_HELP_MSG;
    static const std::string ADVANCED_KODI_ENABLE_HELP_MSG;
    static const std::string ADVANCED_KODI_X_HELP_MSG;
    static const std::string ADVANCED_KODI_AT_START_HELP_MSG;

    static const std::string ADVANCED_SECURITY_HELP_MSG;
    static const std::string ADVANCED_ROOT_PWD_HELP_MSG;
    static const std::string ADVANCED_ENFORCE_SECURITY_HELP_MSG;
    static const std::string ADVANCED_OVERSCAN_HELP_MSG;
    static const std::string ADVANCED_FRAMERATE_HELP_MSG;
    static const std::string ADVANCED_MANAGER_HELP_MSG;
    static const std::string ADVANCED_API_HELP_MSG;
    static const std::string ADVANCED_EMU_EMU_HELP_MSG;
    static const std::string ADVANCED_EMU_CORE_HELP_MSG;
};

const std::string MenuMessages::START_KODI_HELP_MSG = _("Start kodi media player.");
const std::string MenuMessages::SYSTEM_HELP_MSG = _("Configure your recalbox language, select an external drive to store your games and configurations, check your current version and free space on drive");
const std::string MenuMessages::VERSION_HELP_MSG = _("Shows your current recalboxOS version.");
const std::string MenuMessages::DISK_USAGE_HELP_MSG = _("Show how much space is used on your SHARE partition, located either on the SDCARD or on an external drive. The information shows how much GB are used on how much GB your storage has (example 13GB/26GB).");
const std::string MenuMessages::STORAGE_DEVICE_HELP_MSG = _("Select an external drive to store your roms, saves, configurations etc.\nUse a FAT32 formatted drive. The system do not format the drive. On first boot with this option enabled, recalbox will create a '/recalbox' folder with all system files inside.");
const std::string MenuMessages::LANGUAGE_HELP_MSG = _("Select your language. A reboot is needed to take this configuration in account.");

const std::string MenuMessages::UPDATE_HELP_MSG = _("Manage your recalbox updates. Select the update type. Activate update check.");
const std::string MenuMessages::START_UPDATE_HELP_MSG = _("Check if an update is available, and start the update process.");
const std::string MenuMessages::UPDATE_TYPE_HELP_MSG = _("Stable updates will check for updates on stable recalbox releases. Stable update are tested and approved by the recalbox team and testers.\nUnstable updates allow you to get the last recalbox features by checking our unstable repository. You can test and validate with us the very last version of recalbox.\nIf you choose unstable update, be kind to report issues on recalbox-os issue board (https://github.com/recalbox/recalbox-os/issues)");
const std::string MenuMessages::UPDATE_CHECK_HELP_MSG = _("Automatically check if an update is avaialble. If so, notifies you with a message.");

const std::string MenuMessages::GAME_SETTINGS_HELP_MSG = _("Configure games display, ratio, filters (shaders), auto save and load and retroachievement account.");
const std::string MenuMessages::GAME_RATIO_HELP_MSG = _("The game ratio is the ratio between image width and image height. Use AUTO to let the emulator choose the original game ratio, that will give you the best retrogaming experience.");
const std::string MenuMessages::GAME_SMOOTH_HELP_MSG = _("Smooth the game image. This option make the image smoother, using bilinear filtering.");
const std::string MenuMessages::GAME_REWIND_HELP_MSG = _("This option allows you to rewind the game if you are killed by a monster, or if you make any other mistake. Use HOTKEY + LEFT shortcut in game to rewind.");
const std::string MenuMessages::GAME_AUTOSAVELOAD_HELP_MSG = _("Auto save the state when you quit a game, and auto load last saved state when you load a game.");
const std::string MenuMessages::GAME_INTEGER_SCALE_HELP_MSG = _("Integer scaling is scaling by a factor of a whole number, such as 2x, 3x, 4x, etc. This option scales the image up to the greatest integer scale below the set resolution. So for instance, if you set your fullscreen resolution to 1920x1080 and enable integer scaling, it will only scale a 320x240 image up to 1280x960, and leave black borders all around. This is to maintain a 1:1 pixel ratio with the original source image, so that pixels are not unevenly duplicated.");
const std::string MenuMessages::GAME_SHADERS_HELP_MSG = _("Shaders are like filters for the game rendering. You can select a shader set here, that is a collection of shaders selected for each system. You can also change the shader during the game with HOTKEY + L2 or HOTKEY + R2 shortcut.");

const std::string MenuMessages::RA_ONOFF_HELP_MSG = _("Enable or disable retroachievements support in games.");
const std::string MenuMessages::RA_HARDCORE_HELP_MSG = _("Hardcore mode disables *all* savestate and rewind abilities in the emulator: you would not be able to save and reload at any time. You would have to complete the game and get the achievements first time, just like it were on the original console. In reward for this, you will earn both the standard and the hardcore achievement, in effect earning double points! A regular game worth 400 points, is now worth 800 if you complete it fully on hardcore! For example: if you complete the game fully for 400 points, you then have the opportunity to earn another 400 on hardcore.");
const std::string MenuMessages::RA_HELP_MSG = _("The website retroachievements.org proposes challenges/achievements/trophies on platforms like NES, SNES, GB, GBC, GBA, Genesis/Megadrive, TurboGrafx16/PCEngine and more! Create your account on retroachievements.org and start the quest of achievements !");

const std::string MenuMessages::CONTROLLER_HELP_MSG = _("Add and configure up to 5 controllers.");
const std::string MenuMessages::CONTROLLER_BT_HELPMSG = _("Pair a bluetooth controller with your recalbox. Your controller must be in pairing mode.");
const std::string MenuMessages::CONTROLLER_FORGET_HELPMSG = _("Forget all paired bluetooth controllers. You will have to pair again your controllers, but this option can help if you have issues to reconnect a controller you already paired.");

const std::string MenuMessages::UI_HELP_MSG = _("Configure your emulationstation experience. Select transition types, helps prompts, screensaver behavior. You can also deactivate the onscreen keyboard if you have a real keyboard plugged on your recalbox.\nIf you added games since last boot, you can also refresh the gamelist from this menu.");
const std::string MenuMessages::UI_SCREENSAVER_AFTER_HELP_MSG = _("Start the screensaver after N minutes.");
const std::string MenuMessages::UI_SCREENSAVER_BEHAVIOR_HELP_MSG = _("Set the screensaver behavior. DIM will reduce the screen light, and BLACK will turn the screen black.");
const std::string MenuMessages::UI_ONSCREENHELP_HELP_MSG = _("Show help prompt showing commands you can use on screen bottom.");
const std::string MenuMessages::UI_QUICK_HELP_MSG = _("When enabled, you can switch between systems while browsing a gamelist by pressing LEFT or RIGHT.");
const std::string MenuMessages::UI_KEYBOARD_HELP_MSG = _("The onscreen keyboard is necessary to type text if you only have controllers plugged to your recalbox. You can disable it if you have a real keyboard.");
const std::string MenuMessages::UI_TRANSITION_HELP_MSG = _("Select the type of transition that occurs when you start a game. FADE will fade to dark, and SLIDE will zoom on the game cover (or name if there is no scrap informations)");
const std::string MenuMessages::UI_THEME_HELP_MSG = _("Select the recalbox theme.");
const std::string MenuMessages::UI_UPDATE_GAMELIST_HELP_MSG = _("Update the gamelists if you added games since last boot.");

const std::string MenuMessages::SOUND_HELP_MSG = _("Configure the sound options of your recalbox.");
const std::string MenuMessages::SOUND_VOLUME_HELP_MSG = _("Set the volume of the sound output for the frontend and games.");
const std::string MenuMessages::SOUND_FRONTEND_MUSIC_HELP_MSG = _("Enable or disable frontend music. You can add your own musics in mp3 or ogg format in the 'musics' directory of your recalbox.");
const std::string MenuMessages::SOUND_DEVICE_HELP_MSG = _("Select your output device. Only HDMI and JACK are supported.");

const std::string MenuMessages::NETWORK_HELP_MSG = _("Configure the network options of your recalbox.\nCheck your network status and IP address, set the hostname and configure the wifi.");
const std::string MenuMessages::NETWORK_STATUS_HELP_MSG = _("Displays CONNECTED if you are connected, by checking if your recalbox can access the recalbox.com update server.");
const std::string MenuMessages::NETWORK_IP_HELP_MSG = _("The recalbox IP on the local network.");
const std::string MenuMessages::NETWORK_WIFI_HELP_MSG = _("Enable or disable the wifi. If you disable the wifi, the SSID and wifi password are saved and can be used when you reenable it.");
const std::string MenuMessages::NETWORK_HOST_HELP_MSG = _("Name of the recalbox on your local network.");
const std::string MenuMessages::NETWORK_SSID_HELP_MSG = _("SSID (Access point name) of your wifi network.");
const std::string MenuMessages::NETWORK_KEY_HELP_MSG = _("Private key of your wifi network.");

const std::string MenuMessages::SCRAPER_HELP_MSG = _("Get informations and visual for your games. The scraper download for different servers the metadatas and visuals for your games, and enhance completely the user experience in emulationstation.");
const std::string MenuMessages::SCRAPER_FROM_HELP_MSG = _("Select the server to scrape from. The SCREENSCRAPER server is recommended and is based on www.screenscraper.fr and scraps game data in your language if available.");
const std::string MenuMessages::SCRAPER_NOW_HELP_MSG = _("Begin the scrape process, taking in account the configuration below.");
const std::string MenuMessages::SCRAPER_RATINGS_HELP_MSG = _("Scrape and display game ratings.");

const std::string MenuMessages::ADVANCED_HELP_MSG = _("Advanced settings. Please be sure to know what you do before changing any value in this menu.");
const std::string MenuMessages::ADVANCED_OVERCLOCK_HELP_MSG = _("Overclock your board to increase performance.\nOverclock settings are tested and validated by the community. Keep in mind that overclocking your board can void the warranty.");
const std::string MenuMessages::ADVANCED_BOOT_ON_SYSTEM_HELP_MSG = _("Select the system to show when the recalbox frontend starts. The default value is 'favorites'.");
const std::string MenuMessages::ADVANCED_BOOTGAMELIST_HELP_MSG = _("On boot, recalbox will show the list of the games of the selected system rather than the system view.");
const std::string MenuMessages::ADVANCED_GAMELISTONLY_HELP_MSG = _("Only show games contained in the gamelist.xml file (located in your roms directories).\nThis option highly speeds up boot time, but new games are not detected.");
const std::string MenuMessages::ADVANCED_HIDESYSTEMVIEW_HELP_MSG = _("This option allows you to fix the selected system. The user cannot move to other systems.");
const std::string MenuMessages::ADVANCED_EMULATOR_ADVANCED_HELP_MSG = _("Override global options like emulator, core, ratio and more for each available system in your recalbox.");
const std::string MenuMessages::ADVANCED_BOOT_HELP_MSG = _("Configure boot options that makes your recalbox boot straight on a system or in kodi, lock user to a single system, or show the gamelist.");

const std::string MenuMessages::ADVANCED_KODI_HELP_MSG = _("Enable or disable Kodi, customize Kodi startup, allow X button to start Kodi");
const std::string MenuMessages::ADVANCED_KODI_ENABLE_HELP_MSG = _("Enable or disable Kodi. If kodi is disabled, you won't be able to start it with X button or at boot time and the menu entry will be removed.");
const std::string MenuMessages::ADVANCED_KODI_X_HELP_MSG = _("Use X button to start Kodi.");
const std::string MenuMessages::ADVANCED_KODI_AT_START_HELP_MSG = _("Automatically start Kodi when the recalbox boots.");

const std::string MenuMessages::ADVANCED_SECURITY_HELP_MSG = _("Manage your recalbox security.");
const std::string MenuMessages::ADVANCED_ROOT_PWD_HELP_MSG = _("Change the ssh root password.");
const std::string MenuMessages::ADVANCED_ENFORCE_SECURITY_HELP_MSG = _("Enforce recalbox security.");
const std::string MenuMessages::ADVANCED_OVERSCAN_HELP_MSG = _("Enable or disable overscan.\nOverscan can help you if you have black boarder or if the image is bigger than your screen. Before setting the overscan, try to configure your TV to have a 1:1 pixel display.\nMore overscan settings can be defined in the boot.txt file, available when you plug your sd card on your computer.");
const std::string MenuMessages::ADVANCED_FRAMERATE_HELP_MSG = _("Show framerate in emulationstation in ingame.");
const std::string MenuMessages::ADVANCED_MANAGER_HELP_MSG = _("Enable or disable the Recalbox Manager.\nThe Recalbox Manager is a web application available on http://recalbox if you are on windows, http://recalbox.local if you are on Linux or mac, or directly with your recalbox IP : http://192.168.1.XX.\nYou can configure many options from the manager, and even manage games, saves, and scrap !");
const std::string MenuMessages::ADVANCED_API_HELP_MSG = _("Enable or disable the Recalbox Api.\nThe Recalbox Api is a REST api exposing endpoints to control your recalbox via http requests.");
const std::string MenuMessages::ADVANCED_EMU_EMU_HELP_MSG = _("Select the emulator to use when you start a game in this system.");
const std::string MenuMessages::ADVANCED_EMU_CORE_HELP_MSG = _("Select the core to use for the selected emulator. For example, the LIBRETRO emulator has many core to run Super Nintendo games. The default core you choose here can also be overridden in game specific settings.");


#endif //EMULATIONSTATION_ALL_MENUMESSAGES_H
