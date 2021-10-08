################################################################################
#
# kodi addon signals
#
################################################################################

KODI_SCRIPT_MODULE_ADDON_SIGNALS_VERSION = 0.0.6
KODI_SCRIPT_MODULE_ADDON_SIGNALS_SOURCE = script.module.addon.signals-$(KODI_SCRIPT_MODULE_ADDON_SIGNALS_VERSION)+matrix.1.zip
KODI_SCRIPT_MODULE_ADDON_SIGNALS_SITE = https://mirrors.kodi.tv/addons/matrix/script.module.addon.signals
KODI_SCRIPT_MODULE_ADDON_SIGNALS_PLUGIN_NAME = script.module.addon.signals
KODI_SCRIPT_MODULE_ADDON_SIGNALS_LICENSE = LGPL-2.0
KODI_SCRIPT_MODULE_ADDON_SIGNALS_LICENSE_FILES = LICENSE.txt

KODI_SCRIPT_MODULE_ADDON_SIGNALS_TARGET_DIR=$(TARGET_DIR)/usr/share/kodi/addons

define KODI_SCRIPT_MODULE_ADDON_SIGNALS_EXTRACT_CMDS
	@unzip -q -o $(DL_DIR)/kodi-script-module-addon-signals/$(KODI_SCRIPT_MODULE_ADDON_SIGNALS_SOURCE) -d $(@D)
endef

define KODI_SCRIPT_MODULE_ADDON_SIGNALS_INSTALL_TARGET_CMDS
	@mkdir -p $(KODI_SCRIPT_MODULE_ADDON_SIGNALS_TARGET_DIR)
	@cp -r $(@D)/* $(KODI_SCRIPT_MODULE_ADDON_SIGNALS_TARGET_DIR)/$(KODI_SCRIPT_MODULE_ADDON_SIGNALS_PLUGIN_NAME)/
endef

$(eval $(generic-package))
