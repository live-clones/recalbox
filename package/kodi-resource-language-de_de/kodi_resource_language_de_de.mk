################################################################################
#
# kodi german language resource
#
################################################################################

KODI_RESOURCE_LANGUAGE_DE_DE_RELEASE = matrix
KODI_RESOURCE_LANGUAGE_DE_DE_VERSION = 9.0.54
KODI_RESOURCE_LANGUAGE_DE_DE_PLUGINNAME = resource.language.de_de
KODI_RESOURCE_LANGUAGE_DE_DE_SOURCE = $(KODI_RESOURCE_LANGUAGE_DE_DE_PLUGINNAME)-$(KODI_RESOURCE_LANGUAGE_DE_DE_VERSION).zip
KODI_RESOURCE_LANGUAGE_DE_DE_SITE = https://mirrors.kodi.tv/addons/$(KODI_RESOURCE_LANGUAGE_DE_DE_RELEASE)/$(KODI_RESOURCE_LANGUAGE_DE_DE_PLUGINNAME)
KODI_RESOURCE_LANGUAGE_DE_DE_LICENSE = COPYRIGHT

KODI_RESOURCE_LANGUAGE_DE_DE_PLUGIN_TARGET_DIR=$(TARGET_DIR)/usr/share/kodi/addons

define KODI_RESOURCE_LANGUAGE_DE_DE_EXTRACT_CMDS
	@unzip -q -o $(DL_DIR)/kodi-resource-language-de_de/$(KODI_RESOURCE_LANGUAGE_DE_DE_SOURCE) -d $(@D)
endef

define KODI_RESOURCE_LANGUAGE_DE_DE_INSTALL_TARGET_CMDS
	@mkdir -p $(KODI_RESOURCE_LANGUAGE_DE_DE_PLUGIN_TARGET_DIR)
	@cp -r $(@D)/$(KODI_RESOURCE_LANGUAGE_DE_DE_PLUGINNAME) $(KODI_RESOURCE_LANGUAGE_DE_DE_PLUGIN_TARGET_DIR)
endef

$(eval $(generic-package))
