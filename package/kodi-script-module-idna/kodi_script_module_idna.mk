################################################################################
#
# kodi idna script
#
################################################################################

KODI_SCRIPT_MODULE_IDNA_VERSION = 2.10.0
KODI_SCRIPT_MODULE_IDNA_SOURCE = script.module.idna-$(KODI_SCRIPT_MODULE_IDNA_VERSION)+matrix.1.zip
KODI_SCRIPT_MODULE_IDNA_SITE = https://mirrors.kodi.tv/addons/matrix/script.module.idna
KODI_SCRIPT_MODULE_IDNA_PLUGIN_NAME = script.module.idna
KODI_SCRIPT_MODULE_IDNA_LICENSE = COPYRIGHT

KODI_SCRIPT_MODULE_IDNA_TARGET_DIR=$(TARGET_DIR)/usr/share/kodi/addons

define KODI_SCRIPT_MODULE_IDNA_EXTRACT_CMDS
	@unzip -q -o $(DL_DIR)/kodi-script-module-idna/$(KODI_SCRIPT_MODULE_IDNA_SOURCE) -d $(@D)
endef

define KODI_SCRIPT_MODULE_IDNA_INSTALL_TARGET_CMDS
	@mkdir -p $(KODI_SCRIPT_MODULE_IDNA_TARGET_DIR)
	@cp -r $(@D)/$(KODI_SCRIPT_MODULE_IDNA_PLUGIN_NAME) $(KODI_SCRIPT_MODULE_IDNA_TARGET_DIR)/
endef

$(eval $(generic-package))
