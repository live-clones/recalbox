################################################################################
#
# kodi six script
#
################################################################################

KODI_SCRIPT_MODULE_SIX_RELEASE = matrix
KODI_SCRIPT_MODULE_SIX_VERSION = 1.15.0+$(KODI_SCRIPT_MODULE_SIX_RELEASE).1
KODI_SCRIPT_MODULE_SIX_PLUGIN_NAME = script.module.six
KODI_SCRIPT_MODULE_SIX_SOURCE = $(KODI_SCRIPT_MODULE_SIX_PLUGIN_NAME)-$(KODI_SCRIPT_MODULE_SIX_VERSION).zip
KODI_SCRIPT_MODULE_SIX_SITE = https://mirrors.kodi.tv/addons/$(KODI_SCRIPT_MODULE_SIX_RELEASE)/$(KODI_SCRIPT_MODULE_SIX_PLUGIN_NAME)
KODI_SCRIPT_MODULE_SIX_LICENSE = LICENSE

KODI_SCRIPT_MODULE_SIX_TARGET_DIR=$(TARGET_DIR)/usr/share/kodi/addons

define KODI_SCRIPT_MODULE_SIX_EXTRACT_CMDS
	@unzip -q -o $(DL_DIR)/kodi-script-module-six/$(KODI_SCRIPT_MODULE_SIX_SOURCE) -d $(@D)
endef

define KODI_SCRIPT_MODULE_SIX_INSTALL_TARGET_CMDS
	@mkdir -p $(KODI_SCRIPT_MODULE_SIX_TARGET_DIR)
	@cp -r $(@D)/$(KODI_SCRIPT_MODULE_SIX_PLUGIN_NAME) $(KODI_SCRIPT_MODULE_SIX_TARGET_DIR)/
endef

$(eval $(generic-package))
