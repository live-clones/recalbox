################################################################################
#
# kodi eu_es language resource
#
################################################################################

KODI_RESOURCE_LANGUAGE_EU_ES_VERSION = 9.0.31
KODI_RESOURCE_LANGUAGE_EU_ES_SOURCE = resource.language.eu_es-$(KODI_RESOURCE_LANGUAGE_EU_ES_VERSION).zip
KODI_RESOURCE_LANGUAGE_EU_ES_SITE = http://mirrors.kodi.tv/addons/matrix/resource.language.eu_es
KODI_RESOURCE_LANGUAGE_EU_ES_PLUGINNAME=resource.language.eu_es
KODI_RESOURCE_LANGUAGE_EU_ES_LICENSE = COPYRIGHT

KODI_RESOURCE_LANGUAGE_EU_ES_PLUGIN_TARGET_DIR=$(TARGET_DIR)/usr/share/kodi/addons

define KODI_RESOURCE_LANGUAGE_EU_ES_EXTRACT_CMDS
	@unzip -q -o $(DL_DIR)/kodi-resource-language-eu_es/$(KODI_RESOURCE_LANGUAGE_EU_ES_SOURCE) -d $(@D)
endef

define KODI_RESOURCE_LANGUAGE_EU_ES_INSTALL_TARGET_CMDS
	@mkdir -p $(KODI_RESOURCE_LANGUAGE_EU_ES_PLUGIN_TARGET_DIR)
	@cp -r $(@D)/$(KODI_RESOURCE_LANGUAGE_EU_ES_PLUGINNAME) $(KODI_RESOURCE_LANGUAGE_EU_ES_PLUGIN_TARGET_DIR)
endef

$(eval $(generic-package))
