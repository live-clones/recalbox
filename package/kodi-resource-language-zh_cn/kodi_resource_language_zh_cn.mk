################################################################################
#
# kodi zh_cn language resource
#
################################################################################

KODI_RESOURCE_LANGUAGE_ZH_CN_VERSION = 9.0.39
KODI_RESOURCE_LANGUAGE_ZH_CN_SOURCE = resource.language.zh_cn-$(KODI_RESOURCE_LANGUAGE_ZH_CN_VERSION).zip
KODI_RESOURCE_LANGUAGE_ZH_CN_SITE = http://mirrors.kodi.tv/addons/matrix/resource.language.zh_cn
KODI_RESOURCE_LANGUAGE_ZH_CN_PLUGINNAME=resource.language.zh_cn
KODI_RESOURCE_LANGUAGE_ZH_CN_LICENSE = COPYRIGHT

KODI_RESOURCE_LANGUAGE_ZH_CN_PLUGIN_TARGET_DIR=$(TARGET_DIR)/usr/share/kodi/addons

define KODI_RESOURCE_LANGUAGE_ZH_CN_EXTRACT_CMDS
	@unzip -q -o $(DL_DIR)/kodi-resource-language-zh_cn/$(KODI_RESOURCE_LANGUAGE_ZH_CN_SOURCE) -d $(@D)
endef

define KODI_RESOURCE_LANGUAGE_ZH_CN_INSTALL_TARGET_CMDS
	@mkdir -p $(KODI_RESOURCE_LANGUAGE_ZH_CN_PLUGIN_TARGET_DIR)
	@cp -r $(@D)/$(KODI_RESOURCE_LANGUAGE_ZH_CN_PLUGINNAME) $(KODI_RESOURCE_LANGUAGE_ZH_CN_PLUGIN_TARGET_DIR)
endef

$(eval $(generic-package))
