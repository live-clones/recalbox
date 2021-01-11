################################################################################
#
# kodi requests script
#
################################################################################

KODI_SCRIPT_MODULE_REQUESTS_VERSION = 2.22.0
KODI_SCRIPT_MODULE_REQUESTS_SOURCE = script.module.requests-$(KODI_SCRIPT_MODULE_REQUESTS_VERSION).zip
KODI_SCRIPT_MODULE_REQUESTS_SITE = https://mirrors.kodi.tv/addons/krypton/script.module.requests
KODI_SCRIPT_MODULE_REQUESTS_PLUGIN_NAME = script.module.requests
KODI_SCRIPT_MODULE_REQUESTS_LICENSE = Apache-2.0

KODI_SCRIPT_MODULE_REQUESTS_TARGET_DIR=$(TARGET_DIR)/usr/share/kodi/addons

define KODI_SCRIPT_MODULE_REQUESTS_EXTRACT_CMDS
	@unzip -q -o $(DL_DIR)/kodi-script-module-requests/$(KODI_SCRIPT_MODULE_REQUESTS_SOURCE) -d $(@D)
endef

define KODI_SCRIPT_MODULE_REQUESTS_INSTALL_TARGET_CMDS
	@mkdir -p $(KODI_SCRIPT_MODULE_REQUESTS_TARGET_DIR)
	@cp -r $(@D)/$(KODI_SCRIPT_MODULE_REQUESTS_PLUGIN_NAME) $(KODI_SCRIPT_MODULE_REQUESTS_TARGET_DIR)/
endef

$(eval $(generic-package))
