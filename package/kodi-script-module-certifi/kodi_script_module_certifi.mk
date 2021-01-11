################################################################################
#
# kodi certifi script
#
################################################################################

KODI_SCRIPT_MODULE_CERTIFI_VERSION = 2019.9.11
KODI_SCRIPT_MODULE_CERTIFI_SOURCE = script.module.certifi-$(KODI_SCRIPT_MODULE_CERTIFI_VERSION).zip
KODI_SCRIPT_MODULE_CERTIFI_SITE = https://mirrors.kodi.tv/addons/krypton/script.module.certifi
KODI_SCRIPT_MODULE_CERTIFI_PLUGIN_NAME = script.module.certifi
KODI_SCRIPT_MODULE_CERTIFI_LICENSE = MPL-2.0

KODI_SCRIPT_MODULE_CERTIFI_TARGET_DIR=$(TARGET_DIR)/usr/share/kodi/addons

define KODI_SCRIPT_MODULE_CERTIFI_EXTRACT_CMDS
	@unzip -q -o $(DL_DIR)/kodi-script-module-certifi/$(KODI_SCRIPT_MODULE_CERTIFI_SOURCE) -d $(@D)
endef

define KODI_SCRIPT_MODULE_CERTIFI_INSTALL_TARGET_CMDS
	@mkdir -p $(KODI_SCRIPT_MODULE_CERTIFI_TARGET_DIR)
	@cp -r $(@D)/$(KODI_SCRIPT_MODULE_CERTIFI_PLUGIN_NAME) $(KODI_SCRIPT_MODULE_CERTIFI_TARGET_DIR)/
endef

$(eval $(generic-package))
