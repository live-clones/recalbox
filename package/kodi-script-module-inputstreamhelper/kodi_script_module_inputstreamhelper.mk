################################################################################
#
# kodi inputstreamhelper script
#
################################################################################

KODI_SCRIPT_MODULE_INPUTSTREAMHELPER_VERSION = 0.5.5
KODI_SCRIPT_MODULE_INPUTSTREAMHELPER_SOURCE = script.module.inputstreamhelper-$(KODI_SCRIPT_MODULE_INPUTSTREAMHELPER_VERSION).zip
KODI_SCRIPT_MODULE_INPUTSTREAMHELPER_SITE = https://mirrors.kodi.tv/addons/krypton/script.module.inputstreamhelper
KODI_SCRIPT_MODULE_INPUTSTREAMHELPER_PLUGIN_NAME = script.module.inputstreamhelper
KODI_SCRIPT_MODULE_INPUTSTREAMHELPER_LICENSE = MIT

KODI_SCRIPT_MODULE_INPUTSTREAMHELPER_TARGET_DIR=$(TARGET_DIR)/usr/share/kodi/addons

define KODI_SCRIPT_MODULE_INPUTSTREAMHELPER_EXTRACT_CMDS
	@unzip -q -o $(DL_DIR)/kodi-script-module-inputstreamhelper/$(KODI_SCRIPT_MODULE_INPUTSTREAMHELPER_SOURCE) -d $(@D)
endef

define KODI_SCRIPT_MODULE_INPUTSTREAMHELPER_INSTALL_TARGET_CMDS
	@mkdir -p $(KODI_SCRIPT_MODULE_INPUTSTREAMHELPER_TARGET_DIR)
	@cp -r $(@D)/$(KODI_SCRIPT_MODULE_INPUTSTREAMHELPER_PLUGIN_NAME) $(KODI_SCRIPT_MODULE_INPUTSTREAMHELPER_TARGET_DIR)/
endef

$(eval $(generic-package))
