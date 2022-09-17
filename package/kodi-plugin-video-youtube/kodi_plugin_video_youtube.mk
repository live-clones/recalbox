################################################################################
#
# kodi youtube plugin
#
################################################################################

KODI_PLUGIN_VIDEO_YOUTUBE_VERSION = 6.8.22
KODI_PLUGIN_VIDEO_YOUTUBE_SOURCE = plugin.video.youtube-$(KODI_PLUGIN_VIDEO_YOUTUBE_VERSION)+matrix.1.zip
KODI_PLUGIN_VIDEO_YOUTUBE_SITE = https://github.com/anxdpanic/plugin.video.youtube/releases/download/v$(KODI_PLUGIN_VIDEO_YOUTUBE_VERSION)
KODI_PLUGIN_VIDEO_YOUTUBE_PLUGINNAME=plugin.video.youtube
KODI_PLUGIN_VIDEO_YOUTUBE_LICENSE = GPL-2.0
KODI_PLUGIN_VIDEO_YOUTUBE_LICENSE_FILES = plugin.video.youtube/LICENSES/GPL-2.0-only

KODI_PLUGIN_VIDEO_YOUTUBE_TARGET_DIR=$(TARGET_DIR)/usr/share/kodi/addons

define KODI_PLUGIN_VIDEO_YOUTUBE_EXTRACT_CMDS
	@unzip -q -o $(DL_DIR)/kodi-plugin-video-youtube/$(KODI_PLUGIN_VIDEO_YOUTUBE_SOURCE) -d $(@D)
endef

define KODI_PLUGIN_VIDEO_YOUTUBE_INSTALL_TARGET_CMDS
	@mkdir -p $(KODI_PLUGIN_VIDEO_YOUTUBE_TARGET_DIR)
	@cp -r $(@D)/$(KODI_PLUGIN_VIDEO_YOUTUBE_PLUGINNAME) $(KODI_PLUGIN_VIDEO_YOUTUBE_TARGET_DIR)
endef

$(eval $(generic-package))
