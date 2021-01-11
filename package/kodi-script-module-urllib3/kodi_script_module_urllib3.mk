################################################################################
#
# kodi urllib3 script
#
################################################################################

KODI_SCRIPT_MODULE_URLLIB3_VERSION = 1.25.6
KODI_SCRIPT_MODULE_URLLIB3_SOURCE = script.module.urllib3-$(KODI_SCRIPT_MODULE_URLLIB3_VERSION).zip
KODI_SCRIPT_MODULE_URLLIB3_SITE = https://mirrors.kodi.tv/addons/krypton/script.module.urllib3
KODI_SCRIPT_MODULE_URLLIB3_PLUGIN_NAME = script.module.urllib3
KODI_SCRIPT_MODULE_URLLIB3_LICENSE = MIT

KODI_SCRIPT_MODULE_URLLIB3_TARGET_DIR=$(TARGET_DIR)/usr/share/kodi/addons

define KODI_SCRIPT_MODULE_URLLIB3_EXTRACT_CMDS
	@unzip -q -o $(DL_DIR)/kodi-script-module-urllib3/$(KODI_SCRIPT_MODULE_URLLIB3_SOURCE) -d $(@D)
endef

define KODI_SCRIPT_MODULE_URLLIB3_INSTALL_TARGET_CMDS
	@mkdir -p $(KODI_SCRIPT_MODULE_URLLIB3_TARGET_DIR)
	@cp -r $(@D)/$(KODI_SCRIPT_MODULE_URLLIB3_PLUGIN_NAME) $(KODI_SCRIPT_MODULE_URLLIB3_TARGET_DIR)/
endef

$(eval $(generic-package))
