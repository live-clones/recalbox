//
// Created by matthieu on 28/01/17.
//
#pragma once

#define __MESSAGE_DECORATOR "\x9D\x31\x7B\x2C\x54\xFA\x85\x0E\xAD\x65\x1B"

class MenuMessages {
public:
	static const char* START_KODI_HELP_MSG;
	static const char* SYSTEM_HELP_MSG;
	static const char* VERSION_HELP_MSG;
	static const char* DISK_USAGE_HELP_MSG;
	static const char* STORAGE_DEVICE_HELP_MSG;
	static const char* LANGUAGE_HELP_MSG;
	static const char* KEYBOARD_HELP_MSG;

	static const char* UPDATE_HELP_MSG;
	static const char* START_UPDATE_HELP_MSG;
	static const char* UPDATE_TYPE_HELP_MSG;
	static const char* UPDATE_CHECK_HELP_MSG;
	static const char* UPDATE_VERSION_HELP_MSG;
	static const char* UPDATE_CHANGELOG_HELP_MSG;

	static const char* GAME_SETTINGS_HELP_MSG;
	static const char* GAME_RATIO_HELP_MSG;
	static const char* GAME_SMOOTH_HELP_MSG;
	static const char* GAME_REWIND_HELP_MSG;
	static const char* GAME_AUTOSAVELOAD_HELP_MSG;
	static const char* GAME_PRESS_TWICE_QUIT_HELP_MSG;
	static const char* GAME_INTEGER_SCALE_HELP_MSG;
	static const char* GAME_SHADERS_HELP_MSG;

	static const char* RA_ONOFF_HELP_MSG;
	static const char* RA_HARDCORE_HELP_MSG;
	static const char* RA_HELP_MSG;

	static const char* NP_ONOFF_HELP_MSG;
	static const char* NP_RELAY_HELP_MSG;
	static const char* NP_HELP_MSG;
	static const char* NP_HASH_HELP_MSG;

	static const char* CONTROLLER_HELP_MSG;
	static const char* CONTROLLER_CONF_HELP_MSG;
	static const char* CONTROLLER_BT_HELP_MSG;
	static const char* CONTROLLER_FORGET_HELP_MSG;

	static const char* UI_HELP_MSG;
	static const char* UI_SCREENSAVER_HELP_MSG;
	static const char* UI_SCREENSAVER_AFTER_HELP_MSG;
	static const char* UI_SCREENSAVER_BEHAVIOR_HELP_MSG;
	static const char* UI_ONSCREENHELP_HELP_MSG;
	static const char* UI_HELP_POPUP_DURATION_HELP_MSG;
	static const char* UI_POPUP_POSITION_HELP_MSG;
	static const char* UI_POPUP_HELP_MSG;
	static const char* UI_MUSIC_POPUP_DURATION_HELP_MSG;
	static const char* UI_NETPLAY_POPUP_DURATION_HELP_MSG;
	static const char* UI_CLOCK_HELP_MSG;
	static const char* UI_QUICK_HELP_MSG;
	static const char* UI_KEYBOARD_HELP_MSG;
	static const char* UI_CAROUSEL_HELP_MSG;
	static const char* UI_TRANSITION_HELP_MSG;
	static const char* UI_THEME_HELP_MSG;
	static const char* UI_THEME_COLORSET_MSG;
	static const char* UI_THEME_ICONSET_MSG;
	static const char* UI_THEME_MENU_MSG;
	static const char* UI_THEME_SYSTEMVIEW_MSG;
	static const char* UI_THEME_GAMELISTVIEW_MSG;
	static const char* UI_THEME_CONFIGURATION_MSG;
	static const char* UI_THEME_REGION_MSG;
	static const char* UI_UPDATE_GAMELIST_HELP_MSG;

	static const char* SOUND_HELP_MSG;
	static const char* SOUND_VOLUME_HELP_MSG;
	static const char* SOUND_FRONTEND_MUSIC_HELP_MSG;
	static const char* SOUND_DEVICE_HELP_MSG;

	static const char* NETWORK_HELP_MSG;
	static const char* NETWORK_STATUS_HELP_MSG;
	static const char* NETWORK_IP_HELP_MSG;
	static const char* NETWORK_WIFI_HELP_MSG;
	static const char* NETWORK_HOST_HELP_MSG;
	static const char* NETWORK_SSID_HELP_MSG;
	static const char* NETWORK_MANUAL_INPUT_HELP_MSG;
	static const char* NETWORK_KEY_HELP_MSG;

	static const char* SCRAPER_HELP_MSG;
	static const char* SCRAPER_FROM_HELP_MSG;
	static const char* SCRAPER_NOW_HELP_MSG;
	static const char* SCRAPER_RATINGS_HELP_MSG;

	static const char* ADVANCED_HELP_MSG;
	static const char* ADVANCED_OVERCLOCK_HELP_MSG;
	static const char* ADVANCED_BOOT_ON_SYSTEM_HELP_MSG;
	static const char* ADVANCED_BOOTGAMELIST_HELP_MSG;
	static const char* ADVANCED_GAMELISTONLY_HELP_MSG;
	static const char* ADVANCED_HIDESYSTEMVIEW_HELP_MSG;
	static const char* ADVANCED_BASICGAMELISTVIEW_HELP_MSG;
	static const char* ADVANCED_EMULATOR_ADVANCED_HELP_MSG;
	static const char* ADVANCED_BOOT_HELP_MSG;

	static const char* ADVANCED_KODI_HELP_MSG;
	static const char* ADVANCED_KODI_ENABLE_HELP_MSG;
	static const char* ADVANCED_KODI_X_HELP_MSG;
	static const char* ADVANCED_KODI_AT_START_HELP_MSG;

	static const char* ADVANCED_SECURITY_HELP_MSG;
	static const char* ADVANCED_ROOT_PWD_HELP_MSG;
	static const char* ADVANCED_ENFORCE_SECURITY_HELP_MSG;
	static const char* ADVANCED_OVERSCAN_HELP_MSG;
	static const char* ADVANCED_FRAMERATE_HELP_MSG;
	static const char* ADVANCED_MANAGER_HELP_MSG;
	static const char* ADVANCED_API_HELP_MSG;
	static const char* ADVANCED_EMU_EMU_HELP_MSG;
	static const char* ADVANCED_EMU_CORE_HELP_MSG;

	static const char* GAMELISTOPTION_JUMP_LETTER_MSG;
	static const char* GAMELISTOPTION_SORT_GAMES_MSG;
	static const char* GAMELISTOPTION_FAVORITES_ONLY_MSG;
	static const char* GAMELISTOPTION_SHOW_HIDDEN_MSG;
	static const char* GAMELISTOPTION_SHOW_FOLDER_CONTENT_MSG;
	static const char* GAMELISTOPTION_EDIT_METADATA_MSG;

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
