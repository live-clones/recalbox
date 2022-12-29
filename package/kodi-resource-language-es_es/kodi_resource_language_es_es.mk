################################################################################
#
# kodi spanish language resource
#
################################################################################

KODI_RESOURCE_LANGUAGE_ES_ES_RELEASE = matrix
KODI_RESOURCE_LANGUAGE_ES_ES_VERSION = 9.0.44
KODI_RESOURCE_LANGUAGE_ES_ES_PLUGINNAME = resource.language.es_es
KODI_RESOURCE_LANGUAGE_ES_ES_SOURCE = $(KODI_RESOURCE_LANGUAGE_ES_ES_PLUGINNAME)-$(KODI_RESOURCE_LANGUAGE_ES_ES_VERSION).zip
KODI_RESOURCE_LANGUAGE_ES_ES_SITE = https://mirrors.kodi.tv/addons/$(KODI_RESOURCE_LANGUAGE_ES_ES_RELEASE)/$(KODI_RESOURCE_LANGUAGE_ES_ES_PLUGINNAME)
KODI_RESOURCE_LANGUAGE_ES_ES_LICENSE = COPYRIGHT

KODI_RESOURCE_LANGUAGE_ES_ES_PLUGIN_TARGET_DIR=$(TARGET_DIR)/usr/share/kodi/addons

define KODI_RESOURCE_LANGUAGE_ES_ES_EXTRACT_CMDS
	@unzip -q -o $(DL_DIR)/kodi-resource-language-es_es/$(KODI_RESOURCE_LANGUAGE_ES_ES_SOURCE) -d $(@D)
endef

define KODI_RESOURCE_LANGUAGE_ES_ES_INSTALL_TARGET_CMDS
	@mkdir -p $(KODI_RESOURCE_LANGUAGE_ES_ES_PLUGIN_TARGET_DIR)
	@cp -r $(@D)/$(KODI_RESOURCE_LANGUAGE_ES_ES_PLUGINNAME) $(KODI_RESOURCE_LANGUAGE_ES_ES_PLUGIN_TARGET_DIR)
endef

$(eval $(generic-package))
