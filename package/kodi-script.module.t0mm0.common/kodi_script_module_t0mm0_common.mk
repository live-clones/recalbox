################################################################################
#
# kodi filmon plugin
#
################################################################################

KODI_SCRIPT_MODULE_T0MM0_COMMON_VERSION = 2.1.1
KODI_SCRIPT_MODULE_T0MM0_COMMON_SOURCE = script.module.t0mm0.common-$(KODI_SCRIPT_MODULE_T0MM0_COMMON_VERSION).zip
KODI_SCRIPT_MODULE_T0MM0_COMMON_SITE = "http://mirrors.kodi.tv/addons/leia/script.module.t0mm0.common"
KODI_SCRIPT_MODULE_T0MM0_COMMON_PLUGINNAME=script.module.t0mm0.common
KODI_SCRIPT_MODULE_T0MM0_COMMON_LICENSE = GPL-2.0

KODI_SCRIPT_MODULE_T0MM0_COMMON_PLUGIN_TARGET_DIR=$(TARGET_DIR)/usr/share/kodi/addons

define KODI_SCRIPT_MODULE_T0MM0_COMMON_EXTRACT_CMDS
	@unzip -q -o $(DL_DIR)/kodi-script.module.t0mm0.common/$(KODI_SCRIPT_MODULE_T0MM0_COMMON_SOURCE) -d $(@D)
endef

define KODI_SCRIPT_MODULE_T0MM0_COMMON_INSTALL_TARGET_CMDS
	@mkdir -p $(KODI_SCRIPT_MODULE_T0MM0_COMMON_PLUGIN_TARGET_DIR)
	@cp -r $(@D)/$(KODI_SCRIPT_MODULE_T0MM0_COMMON_PLUGINNAME) $(KODI_SCRIPT_MODULE_T0MM0_COMMON_PLUGIN_TARGET_DIR)
endef

$(eval $(generic-package))
