################################################################################
#
# kodi inputstreamhelper script
#
################################################################################

KODI_PLUGIN_SCRIPT_INPUTSTREAMHELPER_VERSION = 0.5.0
KODI_PLUGIN_SCRIPT_INPUTSTREAMHELPER_SOURCE = script.module.inputstreamhelper-$(KODI_PLUGIN_SCRIPT_INPUTSTREAMHELPER_VERSION).zip
KODI_PLUGIN_SCRIPT_INPUTSTREAMHELPER_SITE = https://mirrors.kodi.tv/addons/krypton/script.module.inputstreamhelper
KODI_PLUGIN_SCRIPT_INPUTSTREAMHELPER_PLUGIN_NAME = script.module.inputstreamhelper

KODI_PLUGIN_SCRIPT_INPUTSTREAMHELPER_TARGET_DIR=$(TARGET_DIR)/usr/share/kodi/addons

define KODI_PLUGIN_SCRIPT_INPUTSTREAMHELPER_EXTRACT_CMDS
	@unzip -q -o $(DL_DIR)/kodi-plugin-script-inputstreamhelper/$(KODI_PLUGIN_SCRIPT_INPUTSTREAMHELPER_SOURCE) -d $(@D)
endef

define KODI_PLUGIN_SCRIPT_INPUTSTREAMHELPER_INSTALL_TARGET_CMDS
	@mkdir -p $(KODI_PLUGIN_SCRIPT_INPUTSTREAMHELPER_TARGET_DIR)
	@cp -r $(@D)/$(KODI_PLUGIN_SCRIPT_INPUTSTREAMHELPER_PLUGIN_NAME) $(KODI_PLUGIN_SCRIPT_INPUTSTREAMHELPER_TARGET_DIR)/
endef

$(eval $(generic-package))
