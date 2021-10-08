################################################################################
#
# kodi myconnpy
#
################################################################################

KODI_SCRIPT_MODULE_MYCONNPY_VERSION = 8.0.18
KODI_SCRIPT_MODULE_MYCONNPY_SOURCE = script.module.myconnpy-$(KODI_SCRIPT_MODULE_MYCONNPY_VERSION)+matrix.1.zip
KODI_SCRIPT_MODULE_MYCONNPY_SITE = https://mirrors.kodi.tv/addons/matrix/script.module.myconnpy
KODI_SCRIPT_MODULE_MYCONNPY_PLUGIN_NAME = script.module.myconnpy
KODI_SCRIPT_MODULE_MYCONNPY_LICENSE = LGPL-2.0
KODI_SCRIPT_MODULE_MYCONNPY_LICENSE_FILES = LICENSE.txt

KODI_SCRIPT_MODULE_MYCONNPY_TARGET_DIR=$(TARGET_DIR)/usr/share/kodi/addons

define KODI_SCRIPT_MODULE_MYCONNPY_EXTRACT_CMDS
	@unzip -q -o $(DL_DIR)/kodi-script-module-myconnpy/$(KODI_SCRIPT_MODULE_MYCONNPY_SOURCE) -d $(@D)
endef

define KODI_SCRIPT_MODULE_MYCONNPY_INSTALL_TARGET_CMDS
	@mkdir -p $(KODI_SCRIPT_MODULE_MYCONNPY_TARGET_DIR)
	@cp -r $(@D)/$(KODI_SCRIPT_MODULE_MYCONNPY_PLUGIN_NAME) $(KODI_SCRIPT_MODULE_MYCONNPY_TARGET_DIR)
endef

$(eval $(generic-package))
