################################################################################
#
# kodi future script
#
################################################################################

KODI_PLUGIN_SCRIPT_FUTURE_VERSION = 0.17.1
KODI_PLUGIN_SCRIPT_FUTURE_SOURCE = script.module.future-$(KODI_PLUGIN_SCRIPT_FUTURE_VERSION).zip
KODI_PLUGIN_SCRIPT_FUTURE_SITE = https://mirrors.kodi.tv/addons/krypton/script.module.future
KODI_PLUGIN_SCRIPT_FUTURE_PLUGIN_NAME = script.module.future

KODI_PLUGIN_SCRIPT_FUTURE_TARGET_DIR=$(TARGET_DIR)/usr/share/kodi/addons

define KODI_PLUGIN_SCRIPT_FUTURE_EXTRACT_CMDS
	@unzip -q -o $(DL_DIR)/kodi-plugin-script-future/$(KODI_PLUGIN_SCRIPT_FUTURE_SOURCE) -d $(@D)
endef

define KODI_PLUGIN_SCRIPT_FUTURE_INSTALL_TARGET_CMDS
	@mkdir -p $(KODI_PLUGIN_SCRIPT_FUTURE_TARGET_DIR)
	@cp -r $(@D)/$(KODI_PLUGIN_SCRIPT_FUTURE_PLUGIN_NAME) $(KODI_PLUGIN_SCRIPT_FUTURE_TARGET_DIR)/
endef

$(eval $(generic-package))
