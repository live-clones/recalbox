################################################################################
#
# kodi confluence 480 skin
#
################################################################################

KODI_SKIN_CONFLUENCE_480_VERSION = 4.7.15
KODI_SKIN_CONFLUENCE_480_SOURCE = skin.confluence.480_v$(KODI_SKIN_CONFLUENCE_480_VERSION).zip
KODI_SKIN_CONFLUENCE_480_SITE = https://github.com/YggdrasiI/skin.confluence.480/releases/download/v$(KODI_SKIN_CONFLUENCE_480_VERSION)
KODI_SKIN_CONFLUENCE_480_PLUGINNAME = skin.confluence.480
KODI_SKIN_CONFLUENCE_480_LICENSE = GPL-2.0
KODI_SKIN_CONFLUENCE_480_LICENSE_FILES = skin.confluence.480/LICENSE.txt

KODI_SKIN_CONFLUENCE_480_TARGET_DIR=$(TARGET_DIR)/usr/share/kodi/addons

define KODI_SKIN_CONFLUENCE_480_EXTRACT_CMDS
	@unzip -q -o $(DL_DIR)/kodi-skin-confluence-480/$(KODI_SKIN_CONFLUENCE_480_SOURCE) -d $(@D)
endef

define KODI_SKIN_CONFLUENCE_480_INSTALL_TARGET_CMDS
	@mkdir -p $(KODI_SKIN_CONFLUENCE_480_TARGET_DIR)
	@cp -r $(@D)/$(KODI_SKIN_CONFLUENCE_480_PLUGINNAME) $(KODI_SKIN_CONFLUENCE_480_TARGET_DIR)
endef

$(eval $(generic-package))
