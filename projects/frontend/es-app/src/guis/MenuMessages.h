//
// Created by matthieu on 28/01/17.
//

#ifndef EMULATIONSTATION_ALL_MENUMESSAGES_H
#define EMULATIONSTATION_ALL_MENUMESSAGES_H


#include <string>

class MenuMessages {
public:
    static constexpr const char *SYSTEM_HELP_MSG = "Configure your recalbox language, select an external drive to store your games and configurations, check your current version and free space on drive";
    static constexpr const char *UPDATE_HELP_MSG = "Manage your recalbox updates. Select the update type. Activate update check.";
    static constexpr const char *UPDATE_TYPE_HELP_MSG = "Stable updates will check for updates on stable recalbox releases. Stable update are tested and approved by the recalbox team and testers.\nUnstable updates allow you to get the last recalbox features by checking our unstable repository. You can test and validate with us the very last version of recalbox.\nIf you choose unstable update, be kind to report issues on recalbox-os issue board (https://github.com/recalbox/recalbox-os/issues)";
    static constexpr const char *UPDATE_CHECK_HELP_MSG = "Automatically check if an update is avaialble. If so, notifies you with a message.";
    static constexpr const char *OVERCLOCK_HELP_MSG = "Overclock your board to increase performance.\nOverclock settings are tested and validated by the community. Keep in mind that overclocking your board can void the warranty.";
    static constexpr const char *BOOT_ON_SYSTEM_HELP_MSG = "Select the system to show when the recalbox frontend starts. The default value is 'favorites'.";
    static constexpr const char *GAMELISTONLY_HELP_MSG = "Only show games contained in the gamelist.xml file (located in your roms directories).\nThis option highly speeds up boot time, but new games are not detected.";
};

#endif //EMULATIONSTATION_ALL_MENUMESSAGES_H
