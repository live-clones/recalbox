################################################################################
#
# kodi embycon plugin
#
################################################################################

KODI_PLUGIN_VIDEO_EMBYCON_VERSION = 1.8.16
KODI_PLUGIN_VIDEO_EMBYCON_SITE = $(call github,faush01,plugin.video.embycon,$(KODI_PLUGIN_VIDEO_EMBYCON_VERSION))
KODI_PLUGIN_VIDEO_EMBYCON_PLUGINNAME = plugin.video.embycon
KODI_PLUGIN_VIDEO_EMBYCON_LICENSE = GPL-2.0+
KODI_PLUGIN_VIDEO_EMBYCON_LICENSE_FILES = LICENSE.txt

KODI_PLUGIN_TARGET_DIR=$(TARGET_DIR)/usr/share/kodi/addons/

define KODI_PLUGIN_VIDEO_EMBYCON_INSTALL_TARGET_CMDS
	@mkdir -p $(KODI_PLUGIN_TARGET_DIR)
	@cp -r $(@D) $(KODI_PLUGIN_TARGET_DIR)/$(KODI_PLUGIN_VIDEO_EMBYCON_PLUGINNAME)
endef

$(eval $(generic-package))
