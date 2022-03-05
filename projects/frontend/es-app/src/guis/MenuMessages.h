//
// Created by matthieu on 28/01/17.
//
#pragma once

#define __MESSAGE_DECORATOR "\x9D\x31\x7B\x2C\x54\xFA\x85\x0E\xAD\x65\x1B"

class MenuMessages
{
  public:
    #define MENUMESSAGE_START_KODI_HELP_MSG "Start kodi media player."
    #define MENUMESSAGE_SYSTEM_HELP_MSG "Select the language for your recalbox, select an external drive to store your games and configurations, check your current version and the free space on your drive"
    #define MENUMESSAGE_VERSION_HELP_MSG "Shows your current recalboxOS version."
    #define MENUMESSAGE_DISK_USAGE_HELP_MSG "Show how much space is used on your SHARE partition, located either on the SDCARD or on an external drive. The information shows how much GB are used and how much GB your storage has overall (example 13GB/26GB)."
    #define MENUMESSAGE_STORAGE_DEVICE_HELP_MSG "Select an external drive to store your roms, saves, configurations etc.\nUse a FAT32 formatted drive. The system does not format the drive. On first boot, with this option enabled, recalbox will create a '/recalbox' folder with all system files inside."
    #define MENUMESSAGE_LANGUAGE_HELP_MSG "Select your language. A reboot is needed to set this configuration active."
    #define MENUMESSAGE_KEYBOARD_HELP_MSG "Select your keyboard layout."
    #define MENUMESSAGE_BIOS_HELP_MSG "Scan and check all your BIOS files and report everything in a comprehensive way."

    #define MENUMESSAGE_UPDATE_HELP_MSG "Manage your recalbox updates. Select the update type. Activate update check."
    #define MENUMESSAGE_START_UPDATE_HELP_MSG "Check if an update is available, and start the update process."
    #define MENUMESSAGE_UPDATE_TYPE_HELP_MSG "Stable updates will check for updates on stable recalbox releases. Stable updates are tested and approved by the recalbox team and their testers.\nUnstable updates allows you to get the latest recalbox features by checking our unstable repository. You can test and validate with us the very last version of recalbox.\nIf you choose unstable update, be so kind to report issues on the recalbox-os issue board (https://github.com/recalbox/recalbox-os/issues)"
    #define MENUMESSAGE_UPDATE_CHECK_HELP_MSG "Automatically check if an update is avaialble. If so, it notifies you with a message."
    #define MENUMESSAGE_UPDATE_VERSION_HELP_MSG "Shows the current available update version."
    #define MENUMESSAGE_UPDATE_CHANGELOG_HELP_MSG "Shows the current available update changelog."

    #define MENUMESSAGE_GAME_SETTINGS_HELP_MSG "Configure games display, ratio, filters (shaders), auto save and load and retroachievement account."
    #define MENUMESSAGE_GAME_RATIO_HELP_MSG "The game ratio is the ratio between image width and image height. Use AUTO to let the emulator choose the original game ratio, that will give you the best retrogaming experience."
    #define MENUMESSAGE_GAME_SMOOTH_HELP_MSG "Smooth the game image. This option makes the image smoother, using bilinear filtering."
    #define MENUMESSAGE_GAME_REWIND_HELP_MSG "This option allows you to rewind the game if you get killed by a monster, or if you make any other mistake. Use the HOTKEY + LEFT command within the game to rewind."
    #define MENUMESSAGE_GAME_AUTOSAVELOAD_HELP_MSG "Auto save the state when you quit a game, and auto load last saved state when you start a game."
    #define MENUMESSAGE_GAME_PRESS_TWICE_QUIT_HELP_MSG "Press twice the buttons to end the game and go back to main menu."
    #define MENUMESSAGE_GAME_INTEGER_SCALE_HELP_MSG "Integer scaling is scaling by a factor of a whole number, such as 2x, 3x, 4x, etc. This option scales the image up to the greatest integer scale below the set resolution. So for instance, if you set your fullscreen resolution to 1920x1080 and enable integer scaling, it will only scale a 320x240 image up to 1280x960, and leave black borders all around. This is to maintain a 1:1 pixel ratio with the original source image, so that pixels are not unevenly duplicated."
    #define MENUMESSAGE_GAME_SHADERS_HELP_MSG "Shaders are like filters for the game rendering. You can select a raw shader file here. This setting may be overloaded by shaderset if not definied to 'none'."
    #define MENUMESSAGE_GAME_SHADERSET_HELP_MSG "Shaders are like filters for the game rendering. You can select a shader set here, which is a collection of shaders selected for each system. You can also change the shader within the game with HOTKEY + L2 or HOTKEY + R2."
    #define MENUMESSAGE_GAME_HIDE_PREINSTALLED "Hide all pre-installed games. Changing this option makes EmulationStation to relaunch."

    #define MENUMESSAGE_RA_ONOFF_HELP_MSG "Enable or disable RetroAchievements in games."
    #define MENUMESSAGE_RA_HARDCORE_HELP_MSG "Hardcore mode disables *all* savestate and rewind functions within the emulator: you will not be able to save and reload at any time. You will have to complete the game and get the achievements first time, just like on the original console. In reward for this, you will earn both the standard and the hardcore achievement, in effect earning double points! A regular game worth 400 points, is now worth 800 if you complete it on hardcore! For example: if you complete the game for 400 points, you then have the opportunity to earn another 400 on hardcore."
    #define MENUMESSAGE_RA_HELP_MSG "The website retroachievements.org proposes challenges/achievements/trophies on platforms like NES, SNES, GB, GBC, GBA, Genesis/Megadrive, TurboGrafx16/PCEngine and more! Create your account on retroachievements.org and start your quest for achievements!"

    #define MENUMESSAGE_NP_ONOFF_HELP_MSG "Enable or disable Netplay in games."
    #define MENUMESSAGE_NP_RELAY_HELP_MSG "Enable or disable connections throught relay servers."
    #define MENUMESSAGE_NP_HELP_MSG "Play online on games running through Retroarch like NES, SNES, FBA, Genesis/Megadrive and more!"
    #define MENUMESSAGE_NP_HASH_HELP_MSG "Add hash of roms in your gamelists to have more accurate results in Netplay."

    #define MENUMESSAGE_CONTROLLER_HELP_MSG "Add and configure up to 5 controllers."
    #define MENUMESSAGE_CONTROLLER_CONF_HELP_MSG "Configure an associated controller. Your controller has to be associated / plugged before."
    #define MENUMESSAGE_CONTROLLER_BT_HELP_MSG "Pair a bluetooth controller with your recalbox. Your controller must be in pairing mode."
    #define MENUMESSAGE_CONTROLLER_FORGET_HELP_MSG "Forget all paired bluetooth controllers. You will have to pair your controllers again, but this option can help if you have issues to reconnect a controller, which is already paired."
    #define MENUMESSAGE_CONTROLLER_DRIVER_HELP_MSG "Change the driver if your pad is not working at all or not working properly in-game."

    #define MENUMESSAGE_UI_HELP_MSG "Configure your EmulationStation experience. Select transition types, help prompts, screensaver behavior. You can also deactivate the onscreen keyboard if you have a real keyboard plugged into your recalbox.\nIf you've added games since the last boot, you can also refresh the gamelist from this menu."
    #define MENUMESSAGE_UI_SCREENSAVER_AFTER_HELP_MSG "Configure screensaver"
    #define MENUMESSAGE_UI_SCREENSAVER_HELP_MSG "Start the screensaver after N minutes."
    #define MENUMESSAGE_GAME_OVERLAYS_HELP_MSG "On wide screens, display system images that wrap around emulated screen."
    #define MENUMESSAGE_UI_SCREENSAVER_BEHAVIOR_HELP_MSG "Set the screensaver behavior. DIM will reduce the screen light, BLACK will turn the screen black, DEMO will launch demo mode."
    #define MENUMESSAGE_UI_ONSCREENHELP_HELP_MSG "Shows a help at the bottom of the screen which displays commands you can use."
    #define MENUMESSAGE_UI_HELP_POPUP_DURATION_HELP_MSG "Set duration of help popups, 0 means no popup."
    #define MENUMESSAGE_UI_POPUP_POSITION_HELP_MSG "Select the position of popups on screen."
    #define MENUMESSAGE_UI_POPUP_HELP_MSG "Set position and duration of popups."
    #define MENUMESSAGE_UI_MUSIC_POPUP_DURATION_HELP_MSG "Set duration of music popups, 0 means no popup."
    #define MENUMESSAGE_UI_NETPLAY_POPUP_DURATION_HELP_MSG "Set duration of netplay popups, 0 means no popup."
    #define MENUMESSAGE_UI_CLOCK_HELP_MSG "Add a clock in the main menu."
    #define MENUMESSAGE_UI_QUICK_HELP_MSG "When enabled, you can switch between systems while browsing a gamelist by pressing LEFT or RIGHT."
    #define MENUMESSAGE_UI_KEYBOARD_HELP_MSG "The onscreen keyboard is necessary to type text if you only have controllers plugged into your recalbox. You can disable it if you have a real keyboard connected."
    #define MENUMESSAGE_UI_CAROUSEL_HELP_MSG "Choose if carousel will be animated or not during transitions"
    #define MENUMESSAGE_UI_TRANSITION_HELP_MSG "Select the type of transition that occurs when you start a game. INSTANT will do nothing, FADE will fade to dark, and SLIDE will zoom on the game cover (or name if there is no scrape information)"
    #define MENUMESSAGE_UI_THEME_HELP_MSG "Select a theme for your recalbox."
    #define MENUMESSAGE_UI_THEME_COLORSET_MSG "Select exisiting colorset options for this theme."
    #define MENUMESSAGE_UI_THEME_ICONSET_MSG "Select exisiting iconset options for this theme."
    #define MENUMESSAGE_UI_THEME_MENU_MSG "Select exisiting menu style options for this theme."
    #define MENUMESSAGE_UI_THEME_SYSTEMVIEW_MSG "Select exisiting system view options for this theme."
    #define MENUMESSAGE_UI_THEME_GAMELISTVIEW_MSG "Select exisiting gamelist view options for this theme."
    #define MENUMESSAGE_UI_THEME_GAMECLIPVIEW_MSG "Select exisiting game clip view options for this theme."
    #define MENUMESSAGE_UI_THEME_CONFIGURATION_MSG "Configure theme options if available."
    #define MENUMESSAGE_UI_THEME_REGION_MSG "Select Region of logos, pictures for system that are different for some countries. E.g. Megadrive in EU / Genesis in US"
    #define MENUMESSAGE_UI_UPDATE_GAMELIST_HELP_MSG "Updates the gamelists, if you added games since the last boot."
    #define MENUMESSAGE_UI_SWAP_VALIDATE_CANCEL_BUTTONS_HELP_MSG "Allow to swap validate button B/X and cancel button B/O"

    #define MENUMESSAGE_SOUND_HELP_MSG "Configure the sound options of your recalbox."
    #define MENUMESSAGE_SOUND_VOLUME_HELP_MSG "Set the volume of the sound output for the frontend and the games."
    #define MENUMESSAGE_SOUND_MODE_HELP_MSG "Select sound to play. Musics, videos sound, both or none"
    #define MENUMESSAGE_SOUND_DEVICE_HELP_MSG "Select your output device. Only HDMI and JACK are supported."

    #define MENUMESSAGE_NETWORK_HELP_MSG "Configure the network options of your recalbox.\nCheck your network status and IP address, set the hostname and configure the WIFI."
    #define MENUMESSAGE_NETWORK_STATUS_HELP_MSG "Displays CONNECTED, if you are connected, by checking if your recalbox can access the recalbox.com update server."
    #define MENUMESSAGE_NETWORK_IP_HELP_MSG "The IP address of your recalbox within your local network."
    #define MENUMESSAGE_NETWORK_WIFI_HELP_MSG "Enable or disable WIFI.\nIf you disable WIFI, the SSID and the WIFI passwords are saved and can be used when you reactivate it"
    #define MENUMESSAGE_NETWORK_HOST_HELP_MSG "The name of your recalbox in your local network"
    #define MENUMESSAGE_NETWORK_SSID_HELP_MSG "SSID (WIFI Name) of your network."
    #define MENUMESSAGE_NETWORK_MANUAL_INPUT_HELP_MSG "Type the name of your SSID if it is hidden or not listed"
    #define MENUMESSAGE_NETWORK_KEY_HELP_MSG "Private key of your WIFI network."

    #define MENUMESSAGE_SCRAPER_HELP_MSG "Get informations and visual for your games. The scraper downloads metadata and visuals for your games from different servers and enhances the user experience in EmulationStation completely."
    #define MENUMESSAGE_SCRAPER_FROM_HELP_MSG "Select a server to scrape from. The SCREENSCRAPER server is recommended and is based on www.screenscraper.fr and scrapes game data in your language, if available."
    #define MENUMESSAGE_SCRAPER_GET_NAME_FROM_HELP_MSG "Select the source of your game name. Trust the scraping database or get them from filename, raw or undecorated (without decoration in () or [] )."
    #define MENUMESSAGE_SCRAPER_EXTRACT_REGION_FROM_FILENAME_HELP_MSG "Try to extract game region from its filename when possible. Support long and short region game (JP or Japan, EU or Europe, ...)"
    #define MENUMESSAGE_SCRAPER_NOW_HELP_MSG "Begin the scrape process with the configuration shown below."
    #define MENUMESSAGE_SCRAPER_RATINGS_HELP_MSG "Scrape and display game ratings."
    #define MENUMESSAGE_SCRAPER_FINAL_POPUP "Scraping complete! {PROCESSED} games processed.\n\n{SUCCESS} game(s) scraped or updated\n{NOTFOUND} game(s) not found...\n{ERRORS} request/download errors\n\n{TEXTINFO} Text information updated\n{IMAGES} images and {VIDEOS} videos downloaded\n{MEDIASIZE} of media saved\nNow, EmulationStation is going to relaunch to update all gamelists."
    #define MENUMESSAGE_SCRAPER_FINAL_QUOTA "You reached your daily quota of scraping request.\nAll your today's scrapes have been saved anyway.\n\nStart scraping again tomorrow.\nDont forget to select 'update' and not 'scrape all'"
    #define MENUMESSAGE_SCRAPER_FINAL_DISKFULL "Your share partition is almost full.\nThe scraper stopped automatically.\n\nRemove unused games, media, files to make room before running the scraper again!"

    #define MENUMESSAGE_ADVANCED_HELP_MSG "Advanced settings. Please make sure you really know what you're doing, before changing any values in this menu."
    #define MENUMESSAGE_ADVANCED_OVERCLOCK_HELP_MSG "Overclock your board to increase the performance.\nOverclock settings are tested and validated by the community. Keep in mind that overclocking your board can void your warranty."
    #define MENUMESSAGE_ADVANCED_BOOT_ON_SYSTEM_HELP_MSG "Select which system to show when the recalbox frontend starts. The default value is 'favorites'."
    #define MENUMESSAGE_ADVANCED_BOOTGAMELIST_HELP_MSG "On boot, recalbox will show the list of games of the selected system rather than the system view."
    #define MENUMESSAGE_ADVANCED_GAMELISTONLY_HELP_MSG "Only show games contained in the gamelist.xml file (located in your roms directories).\nThis option highly speeds up boot time, but new games will not be detected."
    #define MENUMESSAGE_ADVANCED_HIDESYSTEMVIEW_HELP_MSG "This option allows you to set the selected system to fixed mode. With this option activated, the user cannot access other systems."
    #define MENUMESSAGE_ADVANCED_BASICGAMELISTVIEW_HELP_MSG "Always display the basic gamelist view, even if you have scraped your games."
    #define MENUMESSAGE_ADVANCED_EMULATOR_ADVANCED_HELP_MSG "Override global options like emulator, core, ratio and more for each available system in your recalbox."
    #define MENUMESSAGE_ADVANCED_BOOT_HELP_MSG "Configure boot options that make your recalbox boot straight into a system or into Kodi, lock a user to a single system, or directly show the gamelist."
    #define MENUMESSAGE_ADVANCED_ALLGAMES_HELP_MSG "Show a 'all-games' system with all games from all systems."
    #define MENUMESSAGE_ADVANCED_SORTINGOPTION_HELP_MSG "Default: use original or custom systemlist.xml order\nSystem Type: order by Console/Handheld/Computer/Arcade/Other\nRelease Date: order by release date asc\nManufacturer: order by manufacturer (e.g. Sega)\nSpecial Blend: Sort by type then Manufacturer then Release Date"
    #define MENUMESSAGE_ADVANCED_MULTIPLAYERS_HELP_MSG "Show multiplayer games (all games playable by two or more players)."
    #define MENUMESSAGE_ADVANCED_LASTPLAYED_HELP_MSG "Show last played games."
    #define MENUMESSAGE_ADVANCED_LIGHTGUN_HELP_MSG "Show all available games playable with a lightgun."
    #define MENUMESSAGE_ADVANCED_PORTS_HELP_MSG "Show a 'ports' system with all ports in the same place."
    #define MENUMESSAGE_ADVANCED_VIRTUALSYSTEMS_HELP_MSG "Select virtual systems to show."
    #define MENUMESSAGE_ADVANCED_VIRTUALGENRESYSTEMS_HELP_MSG "Select vitual systems per genre to show."
    #define MENUMESSAGE_ADVANCED_ARCADEVIRTUALSYSTEM_HELP_MSG "Enable/Disable Arcade virtual system and set its options"

    #define MENUMESSAGE_ADVANCED_KODI_HELP_MSG "Enable or disable Kodi, customize the Kodi startup, enable the X button to start Kodi"
    #define MENUMESSAGE_ADVANCED_KODI_ENABLE_HELP_MSG "Enable or disable Kodi. If kodi is disabled, you won't be able to start it with the X button, or start it automatically at boot. The menu entry will be removed as well."
    #define MENUMESSAGE_ADVANCED_KODI_X_HELP_MSG "Use the X button to start Kodi."
    #define MENUMESSAGE_ADVANCED_KODI_AT_START_HELP_MSG "Automatically start into Kodi on boot."

    #define MENUMESSAGE_ADVANCED_OVERSCAN_HELP_MSG "Enable or disable overscan.\nOverscan can help you, if you have a black border, or if the image is bigger than your screen. Before setting the overscan, try to configure your TV to have a 1:1 pixel output.\nMore overscan settings can be defined in the boot.txt file, available when you plug your SD card into your computer."
    #define MENUMESSAGE_ADVANCED_FRAMERATE_HELP_MSG "Show the framerate in EmulationStation and in game."
    #define MENUMESSAGE_ADVANCED_MANAGER_HELP_MSG "Enable or disable the Recalbox Manager.\nThe Recalbox Manager is a web application available on http://recalbox , if you are on windows, http://recalbox.local , if you are on Linux or Mac, or directly with your recalbox IP : http://192.168.1.XX.\nYou can configure many options from within the manager, and even manage games, saves, and scrapes!"
    #define MENUMESSAGE_ADVANCED_API_HELP_MSG "Enable or disable the recalbox API.\nThe Recalbox API is a REST API exposing endpoints to control your recalbox via http requests."
    #define MENUMESSAGE_ADVANCED_EMU_EMU_HELP_MSG "Select which emulator to use when you start a game for this system."
    #define MENUMESSAGE_ADVANCED_EMU_CORE_HELP_MSG "Select which core to use for the selected emulator. For example, the LIBRETRO emulator has many cores to run Super Nintendo games. The default core you choose here can also be overridden in game specific settings."
    #define MENUMESSAGE_ADVANCED_FACTORY_RESET "WARNING! This option erase all recalbox and emulator configurations! Use it carefully!"

    #define MENUMESSAGE_GAMELISTOPTION_JUMP_LETTER_MSG "Select a letter and the listing will go directly on the first game starting with this letter."
    #define MENUMESSAGE_GAMELISTOPTION_SORT_GAMES_MSG "Select the way the game list is sortered (alphabetically, by notation...)."
    #define MENUMESSAGE_GAMELISTOPTION_FILTER_REGION_MSG "Select a region to filter out games not matching the selected region."
    #define MENUMESSAGE_GAMELISTOPTION_HIDE_ADULT_MSG "Hide games flagged as adult games."
    #define MENUMESSAGE_GAMELISTOPTION_DIPLAY_By_FILENAME_MSG "Display games by file names."
    #define MENUMESSAGE_GAMELISTOPTION_ONLY_LASt_VERSION "Hide non final versions of a same game. Works with NO-INTRO set."
    #define MENUMESSAGE_GAMELISTOPTION_FAVORITES_ONLY_MSG "Switch between seing or not only the favorites games. To add a game in the favorite list, select the game and toggle its state using 'Y'."
    #define MENUMESSAGE_GAMELISTOPTION_SHOW_HIDDEN_MSG "Switch between seing or not the hidden games. To hide a game, edit its data and select 'Hide'."
    #define MENUMESSAGE_GAMELISTOPTION_SHOW_FOLDER_CONTENT_MSG "Switch between seeing the folders structure and seeing all games in a flatten top level."
    #define MENUMESSAGE_GAMELISTOPTION_EDIT_METADATA_MSG "This option display a menu which allows to change game data and many others options."
    #define MENUMESSAGE_GAMELISTOPTION_DELETE_GAME_MSG "This option display a menu which allows to DELETE game data."

    #define MENUMESSAGE_ADVANCED_CRT_HELP_MSG "RGB output for VGA666, PI2SCART, RGBPI."
    #define MENUMESSAGE_ADVANCED_CRT_DAC_HELP_MSG "Enable RGB output for VGA666, PI2SCART, RGBPI."
    #define MENUMESSAGE_ADVANCED_CRT_ES_RESOLUTION_HELP_MSG "Select emulationstation resolution. 480i is recommended for better details."
    #define MENUMESSAGE_ADVANCED_CRT_GAME_REGION_HELP_MSG "Let you choice between 50Hz and 60Hz at launch, for compatible systems."
    #define MENUMESSAGE_ADVANCED_CRT_GAME_RESOLUTION_HELP_MSG "Let you choice between 240p, 480i, or 480p at launch, for compatible systems."
    #define MENUMESSAGE_ADVANCED_CRT_ZERO_LAG_HELP_MSG "Configure emulators to approach a zero lag experience."
    #define MENUMESSAGE_ADVANCED_CRT_DEMO_RESOLUTION_ON_31KHZ_HELP_MSG "Run the demos in 240p resolution on you 31kHz monitor."
    #define MENUMESSAGE_ADVANCED_CRT_SCANLINES_ON_31KHZ_HELP_MSG "Add scanlines when running games in 480p on 31kHz screen."
    #define MENUMESSAGE_ADVANCED_CRT_FORCE_JACK_HELP_MSG "Force sound on jack. Auto-enabled when 31kHz switch is ON"
    #define MENUMESSAGE_RECALBOXRGBDUAL_HELP_MSG "Recalbox RGB Dual options and configuration."
    #define MENUMESSAGE_ADVANCED_CRT_HORIZONTAL_PAL_OFFSET_HELP_MSG "If you PAL images are not centered, you can override the default horizontal offset here."
    #define MENUMESSAGE_ADVANCED_CRT_VERTICAL_PAL_OFFSET_HELP_MSG "If you PAL images are not centered, you can override the default vertical offset here."

    #define MENUMESSAGE_ADVANCED_RESOLUTION_HELP_MSG "Select system, frontend and emulator resolutions."
    #define MENUMESSAGE_ADVANCED_RESOLUTION_GLOBAL_HELP_MSG "Global resolution is the resolution used by default when specific resolutions are undefined."
    #define MENUMESSAGE_ADVANCED_RESOLUTION_FRONTEND_HELP_MSG "Select the resolution EmulationStation will use."
    #define MENUMESSAGE_ADVANCED_RESOLUTION_EMULATORS_HELP_MSG "Select the resolution used by specific systems."
    #define MENUMESSAGE_ADVANCED_RESOLUTION_SYSTEM_HELP_MSG "Select resolution to use with this system."

    static const char* LICENCE_MSG;
  static const int   LICENCE_MSG_SIZE;

#define LICENSE_ORIGINAL_TEXT "Redistribution and use of the RECALBOX code or any derivative works are permitted provided that the following conditions are met:\n" \
                              "\n" \
                              "- Redistributions may not be sold without authorization, nor may they be used in a commercial product or activity.\n" \
                              "- Redistributions that are modified from the original source must include the complete source code, including the source code for all components used by a binary built from the modified sources.\n" \
                              "- Redistributions must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.\n" \
                              "\n" \
                              "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
};
