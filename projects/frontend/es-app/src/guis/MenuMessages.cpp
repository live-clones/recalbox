#include <Locale.h>
#include "MenuMessages.h"

const char* MenuMessages::START_KODI_HELP_MSG = "Start kodi media player.";
const char* MenuMessages::SYSTEM_HELP_MSG = "Select the language for your recalbox, select an external drive to store your games and configurations, check your current version and the free space on your drive";
const char* MenuMessages::VERSION_HELP_MSG = "Shows your current recalboxOS version.";
const char* MenuMessages::DISK_USAGE_HELP_MSG = "Show how much space is used on your SHARE partition, located either on the SDCARD or on an external drive. The information shows how much GB are used and how much GB your storage has overall (example 13GB/26GB).";
const char* MenuMessages::STORAGE_DEVICE_HELP_MSG = "Select an external drive to store your roms, saves, configurations etc.\nUse a FAT32 formatted drive. The system does not format the drive. On first boot, with this option enabled, recalbox will create a '/recalbox' folder with all system files inside.";
const char* MenuMessages::LANGUAGE_HELP_MSG = "Select your language. A reboot is needed to set this configuration active.";
const char* MenuMessages::KEYBOARD_HELP_MSG = "Select your keyboard layout.";

const char* MenuMessages::UPDATE_HELP_MSG = "Manage your recalbox updates. Select the update type. Activate update check.";
const char* MenuMessages::START_UPDATE_HELP_MSG = "Check if an update is available, and start the update process.";
const char* MenuMessages::UPDATE_TYPE_HELP_MSG = "Stable updates will check for updates on stable recalbox releases. Stable updates are tested and approved by the recalbox team and their testers.\nUnstable updates allows you to get the latest recalbox features by checking our unstable repository. You can test and validate with us the very last version of recalbox.\nIf you choose unstable update, be so kind to report issues on the recalbox-os issue board (https://github.com/recalbox/recalbox-os/issues)";
const char* MenuMessages::UPDATE_CHECK_HELP_MSG = "Automatically check if an update is avaialble. If so, it notifies you with a message.";
const char* MenuMessages::UPDATE_VERSION_HELP_MSG = "Shows the current available update version.";
const char* MenuMessages::UPDATE_CHANGELOG_HELP_MSG = "Shows the current available update changelog.";

const char* MenuMessages::GAME_SETTINGS_HELP_MSG = "Configure games display, ratio, filters (shaders), auto save and load and retroachievement account.";
const char* MenuMessages::GAME_RATIO_HELP_MSG = "The game ratio is the ratio between image width and image height. Use AUTO to let the emulator choose the original game ratio, that will give you the best retrogaming experience.";
const char* MenuMessages::GAME_SMOOTH_HELP_MSG = "Smooth the game image. This option makes the image smoother, using bilinear filtering.";
const char* MenuMessages::GAME_REWIND_HELP_MSG = "This option allows you to rewind the game if you get killed by a monster, or if you make any other mistake. Use the HOTKEY + LEFT command within the game to rewind.";
const char* MenuMessages::GAME_AUTOSAVELOAD_HELP_MSG = "Auto save the state when you quit a game, and auto load last saved state when you start a game.";
const char* MenuMessages::GAME_PRESS_TWICE_QUIT_HELP_MSG = "Press twice the buttons to end the game and go back to main menu.";
const char* MenuMessages::GAME_INTEGER_SCALE_HELP_MSG = "Integer scaling is scaling by a factor of a whole number, such as 2x, 3x, 4x, etc. This option scales the image up to the greatest integer scale below the set resolution. So for instance, if you set your fullscreen resolution to 1920x1080 and enable integer scaling, it will only scale a 320x240 image up to 1280x960, and leave black borders all around. This is to maintain a 1:1 pixel ratio with the original source image, so that pixels are not unevenly duplicated.";
const char* MenuMessages::GAME_SHADERS_HELP_MSG = "Shaders are like filters for the game rendering. You can select a shader set here, which is a collection of shaders selected for each system. You can also change the shader within the game with HOTKEY + L2 or HOTKEY + R2.";

const char* MenuMessages::RA_ONOFF_HELP_MSG = "Enable or disable RetroAchievements in games.";
const char* MenuMessages::RA_HARDCORE_HELP_MSG = "Hardcore mode disables *all* savestate and rewind functions within the emulator: you will not be able to save and reload at any time. You will have to complete the game and get the achievements first time, just like on the original console. In reward for this, you will earn both the standard and the hardcore achievement, in effect earning double points! A regular game worth 400 points, is now worth 800 if you complete it on hardcore! For example: if you complete the game for 400 points, you then have the opportunity to earn another 400 on hardcore.";
const char* MenuMessages::RA_HELP_MSG = "The website retroachievements.org proposes challenges/achievements/trophies on platforms like NES, SNES, GB, GBC, GBA, Genesis/Megadrive, TurboGrafx16/PCEngine and more! Create your account on retroachievements.org and start your quest for achievements!";

const char* MenuMessages::NP_ONOFF_HELP_MSG = "Enable or disable Netplay in games.";
const char* MenuMessages::NP_RELAY_HELP_MSG = "Enable or disable connections throught relay servers.";
const char* MenuMessages::NP_HELP_MSG = "Play online on games running through Retroarch like NES, SNES, FBA, Genesis/Megadrive and more!";
const char* MenuMessages::NP_HASH_HELP_MSG = "Add hash of roms in your gamelists to have more accurate results in Netplay.";

const char* MenuMessages::CONTROLLER_HELP_MSG = "Add and configure up to 5 controllers.";
const char* MenuMessages::CONTROLLER_CONF_HELP_MSG = "Configure an associated controller. Your controller has to be associated / plugged before.";
const char* MenuMessages::CONTROLLER_BT_HELP_MSG = "Pair a bluetooth controller with your recalbox. Your controller must be in pairing mode.";
const char* MenuMessages::CONTROLLER_FORGET_HELP_MSG = "Forget all paired bluetooth controllers. You will have to pair your controllers again, but this option can help if you have issues to reconnect a controller, which is already paired.";

const char* MenuMessages::UI_HELP_MSG = "Configure your EmulationStation experience. Select transition types, help prompts, screensaver behavior. You can also deactivate the onscreen keyboard if you have a real keyboard plugged into your recalbox.\nIf you've added games since the last boot, you can also refresh the gamelist from this menu.";
const char* MenuMessages::UI_SCREENSAVER_AFTER_HELP_MSG = "Start the screensaver after N minutes.";
const char* MenuMessages::UI_SCREENSAVER_BEHAVIOR_HELP_MSG = "Set the screensaver behavior. DIM will reduce the screen light, and BLACK will turn the screen black.";
const char* MenuMessages::UI_ONSCREENHELP_HELP_MSG = "Shows a help at the bottom of the screen which displays commands you can use.";
const char* MenuMessages::UI_HELP_POPUP_DURATION_HELP_MSG = "Set duration of help popups, 0 means no popup.";
const char* MenuMessages::UI_POPUP_POSITION_HELP_MSG = "Select the position of popups on screen.";
const char* MenuMessages::UI_POPUP_HELP_MSG = "Set position and duration of popups.";
const char* MenuMessages::UI_MUSIC_POPUP_DURATION_HELP_MSG = "Set duration of music popups, 0 means no popup.";
const char* MenuMessages::UI_NETPLAY_POPUP_DURATION_HELP_MSG = "Set duration of netplay popups, 0 means no popup.";
const char* MenuMessages::UI_CLOCK_HELP_MSG = "Add a clock in the main menu.";
const char* MenuMessages::UI_QUICK_HELP_MSG = "When enabled, you can switch between systems while browsing a gamelist by pressing LEFT or RIGHT.";
const char* MenuMessages::UI_KEYBOARD_HELP_MSG = "The onscreen keyboard is necessary to type text if you only have controllers plugged into your recalbox. You can disable it if you have a real keyboard connected.";
const char* MenuMessages::UI_CAROUSEL_HELP_MSG = "Choose if carousel will be animated or not during transitions";
const char* MenuMessages::UI_TRANSITION_HELP_MSG = "Select the type of transition that occurs when you start a game. INSTANT will do nothing, FADE will fade to dark, and SLIDE will zoom on the game cover (or name if there is no scrape information)";
const char* MenuMessages::UI_THEME_HELP_MSG = "Select a theme for your recalbox.";
const char* MenuMessages::UI_THEME_COLORSET_MSG = "Select exisiting colorset options for this theme.";
const char* MenuMessages::UI_THEME_ICONSET_MSG = "Select exisiting iconset options for this theme.";
const char* MenuMessages::UI_THEME_MENU_MSG = "Select exisiting menu style options for this theme.";
const char* MenuMessages::UI_THEME_SYSTEMVIEW_MSG = "Select exisiting system view options for this theme.";
const char* MenuMessages::UI_THEME_GAMELISTVIEW_MSG = "Select exisiting gamelist view options for this theme.";
const char* MenuMessages::UI_THEME_CONFIGURATION_MSG = "Configure theme options if available.";
const char* MenuMessages::UI_THEME_REGION_MSG = "Select Region of logos, pictures for system that are different for some countries. E.g. Megadrive in EU / Genesis in US";
const char* MenuMessages::UI_UPDATE_GAMELIST_HELP_MSG = "Updates the gamelists, if you added games since the last boot.";

const char* MenuMessages::SOUND_HELP_MSG = "Configure the sound options of your recalbox.";
const char* MenuMessages::SOUND_VOLUME_HELP_MSG = "Set the volume of the sound output for the frontend and the games.";
const char* MenuMessages::SOUND_FRONTEND_MUSIC_HELP_MSG = "Enable or disable the frontend music. You can add your own music as mp3, or ogg format in the 'musics' directory of your recalbox.";
const char* MenuMessages::SOUND_DEVICE_HELP_MSG = "Select your output device. Only HDMI and JACK are supported.";

const char* MenuMessages::NETWORK_HELP_MSG = "Configure the network options of your recalbox.\nCheck your network status and IP address, set the hostname and configure the WIFI.";
const char* MenuMessages::NETWORK_STATUS_HELP_MSG = "Displays CONNECTED, if you are connected, by checking if your recalbox can access the recalbox.com update server.";
const char* MenuMessages::NETWORK_IP_HELP_MSG = "The IP address of your recalbox within your local network.";
const char* MenuMessages::NETWORK_WIFI_HELP_MSG = "Enable or disable WIFI.\nIf you disable WIFI, the SSID and the WIFI passwords are saved and can be used when you reactivate it";
const char* MenuMessages::NETWORK_HOST_HELP_MSG = "The name of your recalbox in your local network";
const char* MenuMessages::NETWORK_SSID_HELP_MSG = "SSID (WIFI Name) of your network.";
const char* MenuMessages::NETWORK_MANUAL_INPUT_HELP_MSG = "Type the name of your SSID if it is hidden or not listed";
const char* MenuMessages::NETWORK_KEY_HELP_MSG = "Private key of your WIFI network.";

const char* MenuMessages::SCRAPER_HELP_MSG = "Get informations and visual for your games. The scraper downloads metadata and visuals for your games from different servers and enhances the user experience in EmulationStation completely.";
const char* MenuMessages::SCRAPER_FROM_HELP_MSG = "Select a server to scrape from. The SCREENSCRAPER server is recommended and is based on www.screenscraper.fr and scrapes game data in your language, if available.";
const char* MenuMessages::SCRAPER_NOW_HELP_MSG = "Begin the scrape process with the configuration shown below.";
const char* MenuMessages::SCRAPER_RATINGS_HELP_MSG = "Scrape and display game ratings.";

const char* MenuMessages::ADVANCED_HELP_MSG = "Advanced settings. Please make sure you really know what you're doing, before changing any values in this menu.";
const char* MenuMessages::ADVANCED_OVERCLOCK_HELP_MSG = "Overclock your board to increase the performance.\nOverclock settings are tested and validated by the community. Keep in mind that overclocking your board can void your warranty.";
const char* MenuMessages::ADVANCED_BOOT_ON_SYSTEM_HELP_MSG = "Select which system to show when the recalbox frontend starts. The default value is 'favorites'.";
const char* MenuMessages::ADVANCED_BOOTGAMELIST_HELP_MSG = "On boot, recalbox will show the list of games of the selected system rather than the system view.";
const char* MenuMessages::ADVANCED_GAMELISTONLY_HELP_MSG = "Only show games contained in the gamelist.xml file (located in your roms directories).\nThis option highly speeds up boot time, but new games will not be detected.";
const char* MenuMessages::ADVANCED_HIDESYSTEMVIEW_HELP_MSG = "This option allows you to set the selected system to fixed mode. With this option activated, the user cannot access other systems.";
const char* MenuMessages::ADVANCED_BASICGAMELISTVIEW_HELP_MSG = "Always display the basic gamelist view, even if you have scraped your games.";
const char* MenuMessages::ADVANCED_EMULATOR_ADVANCED_HELP_MSG = "Override global options like emulator, core, ratio and more for each available system in your recalbox.";
const char* MenuMessages::ADVANCED_BOOT_HELP_MSG = "Configure boot options that make your recalbox boot straight into a system or into Kodi, lock a user to a single system, or directly show the gamelist.";

const char* MenuMessages::ADVANCED_KODI_HELP_MSG = "Enable or disable Kodi, customize the Kodi startup, enable the X button to start Kodi";
const char* MenuMessages::ADVANCED_KODI_ENABLE_HELP_MSG = "Enable or disable Kodi. If kodi is disabled, you won't be able to start it with the X button, or start it automatically at boot. The menu entry will be removed as well.";
const char* MenuMessages::ADVANCED_KODI_X_HELP_MSG = "Use the X button to start Kodi.";
const char* MenuMessages::ADVANCED_KODI_AT_START_HELP_MSG = "Automatically start into Kodi on boot.";

const char* MenuMessages::ADVANCED_SECURITY_HELP_MSG = "Manage your recalbox security.";
const char* MenuMessages::ADVANCED_ROOT_PWD_HELP_MSG = "Change the SSH root password.";
const char* MenuMessages::ADVANCED_ENFORCE_SECURITY_HELP_MSG = "Enforce recalbox security.";
const char* MenuMessages::ADVANCED_OVERSCAN_HELP_MSG = "Enable or disable overscan.\nOverscan can help you, if you have a black border, or if the image is bigger than your screen. Before setting the overscan, try to configure your TV to have a 1:1 pixel output.\nMore overscan settings can be defined in the boot.txt file, available when you plug your SD card into your computer.";
const char* MenuMessages::ADVANCED_FRAMERATE_HELP_MSG = "Show the framerate in EmulationStation and in game.";
const char* MenuMessages::ADVANCED_MANAGER_HELP_MSG = "Enable or disable the Recalbox Manager.\nThe Recalbox Manager is a web application available on http://recalbox , if you are on windows, http://recalbox.local , if you are on Linux or Mac, or directly with your recalbox IP : http://192.168.1.XX.\nYou can configure many options from within the manager, and even manage games, saves, and scrapes!";
const char* MenuMessages::ADVANCED_API_HELP_MSG = "Enable or disable the recalbox API.\nThe Recalbox API is a REST API exposing endpoints to control your recalbox via http requests.";
const char* MenuMessages::ADVANCED_EMU_EMU_HELP_MSG = "Select which emulator to use when you start a game for this system.";
const char* MenuMessages::ADVANCED_EMU_CORE_HELP_MSG = "Select which core to use for the selected emulator. For example, the LIBRETRO emulator has many cores to run Super Nintendo games. The default core you choose here can also be overridden in game specific settings.";

const char* MenuMessages::GAMELISTOPTION_JUMP_LETTER_MSG = "Select a letter and the listing will go directly on the first game starting with this letter.";
const char* MenuMessages::GAMELISTOPTION_SORT_GAMES_MSG = "Select the way the game list is sortered (alphabetically, by notation...).";
const char* MenuMessages::GAMELISTOPTION_FAVORITES_ONLY_MSG = "Switch between seing or not only the favorites games. To add a game in the favorite list, select the game and toggle its state using 'Y'.";
const char* MenuMessages::GAMELISTOPTION_SHOW_HIDDEN_MSG = "Switch between seing or not the hidden games. To hide a game, edit its data and select 'Hide'.";
const char* MenuMessages::GAMELISTOPTION_SHOW_FOLDER_CONTENT_MSG = "Switch between seeing the folders structure and seeing all games in a flatten top level.";
const char* MenuMessages::GAMELISTOPTION_EDIT_METADATA_MSG = "This option display a menu which allows to change game data and many others options.";
