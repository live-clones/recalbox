################################################################################
#
# kodi netflix plugin
#
################################################################################

KODI_PLUGIN_VIDEO_NETFLIX_VERSION = 1.17.0
KODI_PLUGIN_VIDEO_NETFLIX_SOURCE = plugin.video.netflix-$(KODI_PLUGIN_VIDEO_NETFLIX_VERSION)+matrix.1.zip
KODI_PLUGIN_VIDEO_NETFLIX_SITE = https://github.com/CastagnaIT/plugin.video.netflix/releases/download/v$(KODI_PLUGIN_VIDEO_NETFLIX_VERSION)
KODI_PLUGIN_VIDEO_NETFLIX_PLUGINNAME = plugin.video.netflix
KODI_PLUGIN_VIDEO_NETFLIX_DEPENDENCIES = python-pycryptodomex
KODI_PLUGIN_VIDEO_NETFLIX_LICENSE = MIT
KODI_PLUGIN_VIDEO_NETFLIX_LICENSE_FILES = plugin.video.netflix/README.md

KODI_PLUGIN_VIDEO_NETFLIX_TARGET_DIR=$(TARGET_DIR)/usr/share/kodi/addons

define KODI_PLUGIN_VIDEO_NETFLIX_EXTRACT_CMDS
	@unzip -q -o $(DL_DIR)/kodi-plugin-video-netflix/$(KODI_PLUGIN_VIDEO_NETFLIX_SOURCE) -d $(@D)
endef

define KODI_PLUGIN_VIDEO_NETFLIX_INSTALL_TARGET_CMDS
	@mkdir -p $(KODI_PLUGIN_VIDEO_NETFLIX_TARGET_DIR)
	@cp -r $(@D)/$(KODI_PLUGIN_VIDEO_NETFLIX_PLUGINNAME) $(KODI_PLUGIN_VIDEO_NETFLIX_TARGET_DIR)
endef

$(eval $(generic-package))
