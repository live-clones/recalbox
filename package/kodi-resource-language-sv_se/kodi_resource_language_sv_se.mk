################################################################################
#
# kodi sv_se language resource
#
################################################################################

KODI_RESOURCE_LANGUAGE_SV_SE_VERSION = 9.0.45
KODI_RESOURCE_LANGUAGE_SV_SE_SOURCE = resource.language.sv_se-$(KODI_RESOURCE_LANGUAGE_SV_SE_VERSION).zip
KODI_RESOURCE_LANGUAGE_SV_SE_SITE = http://mirrors.kodi.tv/addons/matrix/resource.language.sv_se
KODI_RESOURCE_LANGUAGE_SV_SE_PLUGINNAME=resource.language.sv_se
KODI_RESOURCE_LANGUAGE_SV_SE_LICENSE = COPYRIGHT

KODI_RESOURCE_LANGUAGE_SV_SE_PLUGIN_TARGET_DIR=$(TARGET_DIR)/usr/share/kodi/addons

define KODI_RESOURCE_LANGUAGE_SV_SE_EXTRACT_CMDS
	@unzip -q -o $(DL_DIR)/kodi-resource-language-sv_se/$(KODI_RESOURCE_LANGUAGE_SV_SE_SOURCE) -d $(@D)
endef

define KODI_RESOURCE_LANGUAGE_SV_SE_INSTALL_TARGET_CMDS
	@mkdir -p $(KODI_RESOURCE_LANGUAGE_SV_SE_PLUGIN_TARGET_DIR)
	@cp -r $(@D)/$(KODI_RESOURCE_LANGUAGE_SV_SE_PLUGINNAME) $(KODI_RESOURCE_LANGUAGE_SV_SE_PLUGIN_TARGET_DIR)
endef

$(eval $(generic-package))
