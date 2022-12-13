################################################################################
#
# kodi fr_fr language resource
#
################################################################################

KODI_RESOURCE_LANGUAGE_FR_FR_VERSION = 9.0.52
KODI_RESOURCE_LANGUAGE_FR_FR_SOURCE = resource.language.fr_fr-$(KODI_RESOURCE_LANGUAGE_FR_FR_VERSION).zip
KODI_RESOURCE_LANGUAGE_FR_FR_SITE = http://mirrors.kodi.tv/addons/matrix/resource.language.fr_fr
KODI_RESOURCE_LANGUAGE_FR_FR_PLUGINNAME=resource.language.fr_fr
KODI_RESOURCE_LANGUAGE_FR_FR_LICENSE = COPYRIGHT

KODI_RESOURCE_LANGUAGE_FR_FR_PLUGIN_TARGET_DIR=$(TARGET_DIR)/usr/share/kodi/addons

define KODI_RESOURCE_LANGUAGE_FR_FR_EXTRACT_CMDS
	@unzip -q -o $(DL_DIR)/kodi-resource-language-fr_fr/$(KODI_RESOURCE_LANGUAGE_FR_FR_SOURCE) -d $(@D)
endef

define KODI_RESOURCE_LANGUAGE_FR_FR_INSTALL_TARGET_CMDS
	@mkdir -p $(KODI_RESOURCE_LANGUAGE_FR_FR_PLUGIN_TARGET_DIR)
	@cp -r $(@D)/$(KODI_RESOURCE_LANGUAGE_FR_FR_PLUGINNAME) $(KODI_RESOURCE_LANGUAGE_FR_FR_PLUGIN_TARGET_DIR)
endef

$(eval $(generic-package))
