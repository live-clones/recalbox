################################################################################
#
# kodi pt_br language resource
#
################################################################################

KODI_RESOURCE_LANGUAGE_PT_BR_VERSION = 9.0.48
KODI_RESOURCE_LANGUAGE_PT_BR_SOURCE = resource.language.pt_br-$(KODI_RESOURCE_LANGUAGE_PT_BR_VERSION).zip
KODI_RESOURCE_LANGUAGE_PT_BR_SITE = http://mirrors.kodi.tv/addons/matrix/resource.language.pt_br
KODI_RESOURCE_LANGUAGE_PT_BR_PLUGINNAME=resource.language.pt_br
KODI_RESOURCE_LANGUAGE_PT_BR_LICENSE = COPYRIGHT

KODI_RESOURCE_LANGUAGE_PT_BR_PLUGIN_TARGET_DIR=$(TARGET_DIR)/usr/share/kodi/addons

define KODI_RESOURCE_LANGUAGE_PT_BR_EXTRACT_CMDS
	@unzip -q -o $(DL_DIR)/kodi-resource-language-pt_br/$(KODI_RESOURCE_LANGUAGE_PT_BR_SOURCE) -d $(@D)
endef

define KODI_RESOURCE_LANGUAGE_PT_BR_INSTALL_TARGET_CMDS
	@mkdir -p $(KODI_RESOURCE_LANGUAGE_PT_BR_PLUGIN_TARGET_DIR)
	@cp -r $(@D)/$(KODI_RESOURCE_LANGUAGE_PT_BR_PLUGINNAME) $(KODI_RESOURCE_LANGUAGE_PT_BR_PLUGIN_TARGET_DIR)
endef

$(eval $(generic-package))
