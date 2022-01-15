################################################################################
#
# kodi python twitch script
#
################################################################################

KODI_SCRIPT_MODULE_PYTHON_TWITCH_VERSION = 2.0.21
KODI_SCRIPT_MODULE_PYTHON_TWITCH_SOURCE = script.module.python.twitch-$(KODI_SCRIPT_MODULE_PYTHON_TWITCH_VERSION)+matrix.1.zip
KODI_SCRIPT_MODULE_PYTHON_TWITCH_SITE = https://github.com/anxdpanic/script.module.python.twitch/releases/download/v$(KODI_SCRIPT_MODULE_PYTHON_TWITCH_VERSION)
KODI_SCRIPT_MODULE_PYTHON_TWITCH_PLUGIN_NAME = script.module.python.twitch
KODI_SCRIPT_MODULE_PYTHON_TWITCH_LICENSE = GPL-3.0-only
KODI_SCRIPT_MODULE_PYTHON_TWITCH_LICENSE_FILES = script.module.python.twitch/LICENSES/GPL-3.0-only

KODI_SCRIPT_MODULE_PYTHON_TWITCH_TARGET_DIR=$(TARGET_DIR)/usr/share/kodi/addons

define KODI_SCRIPT_MODULE_PYTHON_TWITCH_EXTRACT_CMDS
	@unzip -q -o $(DL_DIR)/kodi-script-module-python-twitch/$(KODI_SCRIPT_MODULE_PYTHON_TWITCH_SOURCE) -d $(@D)
endef

define KODI_SCRIPT_MODULE_PYTHON_TWITCH_INSTALL_TARGET_CMDS
	@mkdir -p $(KODI_SCRIPT_MODULE_PYTHON_TWITCH_TARGET_DIR)
	@cp -r $(@D)/$(KODI_SCRIPT_MODULE_PYTHON_TWITCH_PLUGIN_NAME) $(KODI_SCRIPT_MODULE_PYTHON_TWITCH_TARGET_DIR)/
endef

$(eval $(generic-package))
