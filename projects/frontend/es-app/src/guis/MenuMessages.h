//
// Created by matthieu on 28/01/17.
//

#ifndef EMULATIONSTATION_ALL_MENUMESSAGES_H
#define EMULATIONSTATION_ALL_MENUMESSAGES_H

#include <string>

class MenuMessages {
public:
    static constexpr const char *START_KODI_HELP_MSG = "Start kodi media player.";
    static constexpr const char *SYSTEM_HELP_MSG = "Configure your recalbox language, select an external drive to store your games and configurations, check your current version and free space on drive";
    static constexpr const char *VERSION_HELP_MSG = "Shows your current recalboxOS version.";
    static constexpr const char *DISK_USAGE_HELP_MSG = "Show how much space is used on your SHARE partition, located either on the SDCARD or on an external drive. The information shows how much GB are used on how much GB your storage has (example 13GB/26GB).";
    static constexpr const char *STORAGE_DEVICE_HELP_MSG = "Select an external drive to store your roms, saves, configurations etc.\nUse a FAT32 formatted drive. The system do not format the drive. On first boot with this option enabled, recalbox will create a '/recalbox' folder with all system files inside.";
    static constexpr const char *LANGUAGE_HELP_MSG = "Select your language. A reboot is needed to take this configuration in account.";

    static constexpr const char *UPDATE_HELP_MSG = "Manage your recalbox updates. Select the update type. Activate update check.";
    static constexpr const char *START_UPDATE_HELP_MSG = "Check if an update is available, and start the update process.";
    static constexpr const char *UPDATE_TYPE_HELP_MSG = "Stable updates will check for updates on stable recalbox releases. Stable update are tested and approved by the recalbox team and testers.\nUnstable updates allow you to get the last recalbox features by checking our unstable repository. You can test and validate with us the very last version of recalbox.\nIf you choose unstable update, be kind to report issues on recalbox-os issue board (https://github.com/recalbox/recalbox-os/issues)";
    static constexpr const char *UPDATE_CHECK_HELP_MSG = "Automatically check if an update is avaialble. If so, notifies you with a message.";

    static constexpr const char *GAME_SETTINGS_HELP_MSG = "Configure games display, ratio, filters (shaders), auto save and load and retroachievement account.";
    static constexpr const char *GAME_RATIO_HELP_MSG = "The game ratio is the ratio between image width and image height. Use AUTO to let the emulator choose the original game ratio, that will give you the best retrogaming experience.";
    static constexpr const char *GAME_SMOOTH_HELP_MSG = "Smooth the game image. This option make the image smoother, using bilinear filtering.";
    static constexpr const char *GAME_REWIND_HELP_MSG = "This option allows you to rewind the game if you are killed by a monster, or if you make any other mistake. Use HOTKEY + LEFT shortcut in game to rewind.";
    static constexpr const char *GAME_AUTOSAVELOAD_HELP_MSG = "Auto save the state when you quit a game, and auto load last saved state when you load a game.";
    static constexpr const char *GAME_INTEGER_SCALE_HELP_MSG = "Integer scaling is scaling by a factor of a whole number, such as 2x, 3x, 4x, etc. This option scales the image up to the greatest integer scale below the set resolution. So for instance, if you set your fullscreen resolution to 1920x1080 and enable integer scaling, it will only scale a 320x240 image up to 1280x960, and leave black borders all around. This is to maintain a 1:1 pixel ratio with the original source image, so that pixels are not unevenly duplicated.";
    static constexpr const char *GAME_SHADERS_HELP_MSG = "Shaders are like filters for the game rendering. You can select a shader set here, that is a collection of shaders selected for each system. You can also change the shader during the game with HOTKEY + L2 or HOTKEY + R2 shortcut.";

    static constexpr const char *RA_ONOFF_HELP_MSG = "Enable or disable retroachievements support in games.";
    static constexpr const char *RA_HARDCORE_HELP_MSG = "Hardcore mode disables *all* savestate ability in the emulator: you would not be able to save and reload at any time. You would have to complete the game and get the achievements first time, just like it were on the original console. In reward for this, you will earn both the standard and the hardcore achievement, in effect earning double points! A regular game worth 400 points, is now worth 800 if you complete it fully on hardcore! For example: if you complete the game fully for 400 points, you then have the opportunity to earn another 400 on hardcore.";
    static constexpr const char *RA_HELP_MSG = "The website retroachievements.org proposes challenges/achievements/trophies on platforms like NES, SNES, GB, GBC, GBA, Genesis/Megadrive, TurboGrafx16/PCEngine and more! Create your account on retroachievements.org and start the quest of achivements !";

    static constexpr const char *CONTROLLER_HELP_MSG = "Add and configure up to 5 controllers.";
    static constexpr const char *CONTROLLER_BT_HELPMSG = "Pair a bluetooth controller with your recalbox. Your controller must be in pairing mode.";
    static constexpr const char *CONTROLLER_FORGET_HELPMSG = "Forget all paired bluetooth controllers. You will have to pair again your controllers, but this option can help if you have issues reconnected a controller you already paired.";

    static constexpr const char *UI_HELP_MSG = "Configure your emulationstation experience. Select transition types, helps prompts, screensaver behavior. You can also deactivate the onscreen keyboard if you have a real keyboard plugged on your recalbox.\nIf you added games since last boot, you can also refresh the gamelist from this menu.";
    static constexpr const char *UI_SCREENSAVER_AFTER_HELP_MSG = "Start the screensaver after N minutes.";
    static constexpr const char *UI_SCREENSAVER_BEHAVIOR_HELP_MSG = "Set the screensaver behavior. DIM will reduce the screen light, and BLACK will turn the screen black.";
    static constexpr const char *UI_ONSCREENHELP_HELP_MSG = "Show help prompt showing commands you can use on screen bottom.";
    static constexpr const char *UI_QUICK_HELP_MSG = "When enabled, you can switch between systems while browsing a gamelist by pressing LEFT or RIGHT.";
    static constexpr const char *UI_KEYBOARD_HELP_MSG = "The onscreen keyboard is necessary to type text if you only have controllers plugged to your recalbox. You can disable it if you have a real keyboard.";
    static constexpr const char *UI_TRANSITION_HELP_MSG = "Select the type of transition that occurs when you start a game. FADE will fade to dark, and SLIDE will zoom on the game cover (or name if there is no scrap informations)";
    static constexpr const char *UI_THEME_HELP_MSG = "Select the recalbox theme.";
    static constexpr const char *UI_UPDATE_GAMELIST_HELP_MSG = "Update the gamelists if you added games since last boot.";

    static constexpr const char *SOUND_HELP_MSG = "Configure the sound options of your recalbox.";
    static constexpr const char *SOUND_VOLUME_HELP_MSG = "Set the volume of the sound output for the frontend and games.";
    static constexpr const char *SOUND_FRONTEND_MUSIC_HELP_MSG = "Enable or disable frontend music. You can add your own musics in mp3 or ogg format in the 'musics' directory of your recalbox.";
    static constexpr const char *SOUND_DEVICE_HELP_MSG = "Select your output device. Only HDMI and JACK are supported.";

    static constexpr const char *NETWORK_HELP_MSG = "Configure the network options of your recalbox.\nCheck your network status and IP address, set the hostname and configure the wifi.";
    static constexpr const char *NETWORK_STATUS_HELP_MSG = "Displays CONNECTED if you are connected, by checking if your recalbox can access the recalbox.com update server.";
    static constexpr const char *NETWORK_IP_HELP_MSG = "The recalbox IP on the local network.";
    static constexpr const char *NETWORK_WIFI_HELP_MSG = "Enable or disable the wifi. If you disable the wifi, the SSID and wifi password are saved and can be used when you reenable it.";
    static constexpr const char *NETWORK_HOST_HELP_MSG = "Name of the recalbox on your local network.";
    static constexpr const char *NETWORK_SSID_HELP_MSG = "SSID (Access point name) of your wifi network.";
    static constexpr const char *NETWORK_KEY_HELP_MSG = "Private key of your wifi network.";

    static constexpr const char *SCRAPER_HELP_MSG = "Get informations and visual for your games. The scraper download for different servers the metadatas and visuals for your games, and enhance completly the user experience in emulationstation.";
    static constexpr const char *SCRAPER_FROM_HELP_MSG = "Select the server to scrape from. The SCREENSCRAPER server is recommanded and is based on www.screenscraper.fr and scraps game data in your language if available.";
    static constexpr const char *SCRAPER_NOW_HELP_MSG = "Begin the scrape process, taking in account the configuration below.";
    static constexpr const char *SCRAPER_RATINGS_HELP_MSG = "Scrape and display game ratings.";

    static constexpr const char *ADVANCED_HELP_MSG = "Advanced settings. Please be sure to know what you do before changing any value in this menu.";
    static constexpr const char *ADVANCED_OVERCLOCK_HELP_MSG = "Overclock your board to increase performance.\nOverclock settings are tested and validated by the community. Keep in mind that overclocking your board can void the warranty.";
    static constexpr const char *ADVANCED_BOOT_ON_SYSTEM_HELP_MSG = "Select the system to show when the recalbox frontend starts. The default value is 'favorites'.";
    static constexpr const char *ADVANCED_BOOTGAMELIST_HELP_MSG = "On boot, recalbox will show the list of the games of the selected system rather than the system view.";
    static constexpr const char *ADVANCED_GAMELISTONLY_HELP_MSG = "Only show games contained in the gamelist.xml file (located in your roms directories).\nThis option highly speeds up boot time, but new games are not detected.";
    static constexpr const char *ADVANCED_HIDESYSTEMVIEW_HELP_MSG = "This option allows you to fix the selected system. The user cannot move to other systems.";
    static constexpr const char *ADVANCED_EMULATOR_ADVANCED_HELP_MSG = "Override global options like emulator, core, ratio and more for each available system in your recalbox.";
    static constexpr const char *ADVANCED_BOOT_HELP_MSG = "Configure boot options that makes your recalbox boot straight on a system or in kodi, lock user to a single system, or show the gamelist.";

    static constexpr const char *ADVANCED_KODI_HELP_MSG = "Enable or disable Kodi, customize Kodi startup, allow X button to start Kodi";
    static constexpr const char *ADVANCED_KODI_ENABLE_HELP_MSG = "Enable or disable Kodi. If kodi is disabled, you won't be able to start it with X buttonor at boot time and the menu entry will be removed.";
    static constexpr const char *ADVANCED_KODI_X_HELP_MSG = "Use X button to start Kodi.";
    static constexpr const char *ADVANCED_KODI_AT_START_HELP_MSG = "Automatically start Kodi when the recalbox boots.";

    static constexpr const char *ADVANCED_SECURITY_HELP_MSG = "Manage your recalbox security.";
    static constexpr const char *ADVANCED_ROOT_PWD_HELP_MSG = "Change the ssh root password.";
    static constexpr const char *ADVANCED_ENFORCE_SECURITY_HELP_MSG = "Enforce recalbox security.";
    static constexpr const char *ADVANCED_OVERSCAN_HELP_MSG = "Enable or disable overscan.\nOverscan can help you if you have black boarder or if the image is bigger than your screen. Before setting the overscan, try to configure your TV to have a 1:1 pixel display.\nMore overscan settings can be defined in the boot.txt file, available when you plug your sd card on your computer.";
    static constexpr const char *ADVANCED_FRAMERATE_HELP_MSG = "Show framerate in emulationstation in ingame.";
    static constexpr const char *ADVANCED_MANAGER_HELP_MSG = "Enable or disable the Recalbox Manager.\nThe Recalbox Manager is a web application available on http://recalbox if you are on windows, http://recalbox.local if you are on Linux or mac, or directly with your recalbox IP : http://192.168.1.XX.\nYou can configure many options from the manager, and even manage games, saves, and scrap !";
    static constexpr const char *ADVANCED_API_HELP_MSG = "Enable or disable the Recalbox Api.\nThe Recalbox Api is a REST api exposing endpoints to control your recalbox via http requests.";
    static constexpr const char *ADVANCED_EMU_EMU_HELP_MSG = "Select the emulator to use when you start a game in this system.";
    static constexpr const char *ADVANCED_EMU_CORE_HELP_MSG = "Select the core to use for the selected emulator. For example, the LIBRETRO emulator ha many core to run Super Nintendo games. The default core you choose here can also be overriden in game specific settings.";
};

#endif //EMULATIONSTATION_ALL_MENUMESSAGES_H
