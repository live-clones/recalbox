//
// Created by matthieu on 28/01/17.
//

#ifndef EMULATIONSTATION_ALL_MENUMESSAGES_H
#define EMULATIONSTATION_ALL_MENUMESSAGES_H

#include <string>

class MenuMessages {
public:
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

    static constexpr const char *OVERCLOCK_HELP_MSG = "Overclock your board to increase performance.\nOverclock settings are tested and validated by the community. Keep in mind that overclocking your board can void the warranty.";
    static constexpr const char *BOOT_ON_SYSTEM_HELP_MSG = "Select the system to show when the recalbox frontend starts. The default value is 'favorites'.";
    static constexpr const char *BOOTGAMELIST_HELP_MSG = "On boot, recalbox will show the list of the games of the selected system rather than the system view.";
    static constexpr const char *GAMELISTONLY_HELP_MSG = "Only show games contained in the gamelist.xml file (located in your roms directories).\nThis option highly speeds up boot time, but new games are not detected.";
    static constexpr const char *HIDESYSTEMVIEW_HELP_MSG = "This option allows you to fix the selected system. The user cannot move to other systems.";
    static constexpr const char *EMULATOR_ADVANCED_HELP_MSG = "Override global options like emulator, core, ratio and more for each available system in your recalbox.";
    static constexpr const char *BOOT_HELP_MSG = "Configure boot options that makes your recalbox boot straight on a system or in kodi, lock user to a single system, or show the gamelist.";
    static constexpr const char *KODI_HELP_MSG = "Enable or disable Kodi, customize Kodi startup, allow X button to start Kodi";
    static constexpr const char *KODI_AT_START_HELP_MSG = "Automatically start Kodi when the recalbox boots.";
    static constexpr const char *SECURITY_HELP_MSG = "Manage your recalbox security.";
    static constexpr const char *ROOT_PSW_HELP_MSG = "Change the ssh root password.";
    static constexpr const char *ENFORCE_SECURITY_HELP_MSG = "Enforce recalbox security";
    static constexpr const char *OVERSCAN_HELP_MSG = "Enable or disable overscan.\nOverscan can help you if you have black boarder or if the image is bigger than your screen. Before setting the overscan, try to configure your TV to have a 1:1 pixel display.\nMore overscan settings can be defined in the boot.txt file, available when you plug your sd card on your computer.";
    static constexpr const char *MANAGER_HELP_MSG = "Enable or disable the Recalbox Manager.\nThe Recalbox Manager is a web application available on http://recalbox if you are on windows, http://recalbox.local if you are on Linux or mac, or directly with your recalbox IP : http://192.168.1.XX.\nYou can configure many options from the manager, and even manage games, saves, and scrap !";
    static constexpr const char *API_HELP_MSG = "Enable or disable the Recalbox Api.\nThe Recalbox Api is a REST api exposing endpoints to control your recalbox via http requests.";
};

#endif //EMULATIONSTATION_ALL_MENUMESSAGES_H
