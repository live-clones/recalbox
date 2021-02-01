################################################################################
#
# kodi chardet script
#
################################################################################

KODI_SCRIPT_MODULE_CHARDET_VERSION = 3.0.4
KODI_SCRIPT_MODULE_CHARDET_SOURCE = script.module.chardet-$(KODI_SCRIPT_MODULE_CHARDET_VERSION).zip
KODI_SCRIPT_MODULE_CHARDET_SITE = https://mirrors.kodi.tv/addons/krypton/script.module.chardet
KODI_SCRIPT_MODULE_CHARDET_PLUGIN_NAME = script.module.chardet
KODI_SCRIPT_MODULE_CHARDET_LICENSE = LGPL-2.0
KODI_SCRIPT_MODULE_CHARDET_LICENSE_FILES = LICENSE.txt

KODI_SCRIPT_MODULE_CHARDET_TARGET_DIR=$(TARGET_DIR)/usr/share/kodi/addons

define KODI_SCRIPT_MODULE_CHARDET_EXTRACT_CMDS
	@unzip -q -o $(DL_DIR)/kodi-script-module-chardet/$(KODI_SCRIPT_MODULE_CHARDET_SOURCE) -d $(@D)
endef

define KODI_SCRIPT_MODULE_CHARDET_INSTALL_TARGET_CMDS
	@mkdir -p $(KODI_SCRIPT_MODULE_CHARDET_TARGET_DIR)
	@cp -r $(@D)/$(KODI_SCRIPT_MODULE_CHARDET_PLUGIN_NAME) $(KODI_SCRIPT_MODULE_CHARDET_TARGET_DIR)/
endef

$(eval $(generic-package))
