################################################################################
#
# RETROARCH ASSETS
#
################################################################################

RETROARCH_ASSETS_VERSION = db5f4f4babb7f6f9c7edd27690c3ffa911cc9b24
RETROARCH_ASSETS_SITE = $(call github,libretro,retroarch-assets,$(RETROARCH_ASSETS_VERSION))
RETROARCH_ASSETS_LICENSE = GPL
RETROARCH_ASSETS_DEPENDENCIES = retroarch

define RETROARCH_ASSETS_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/share/retroarch/assets/xmb
	cp -r $(@D)/ozone $(TARGET_DIR)/usr/share/retroarch/assets
	cp -r $(@D)/rgui $(TARGET_DIR)/usr/share/retroarch/assets
	cp -r $(@D)/glui $(TARGET_DIR)/usr/share/retroarch/assets
	cp -r $(@D)/FlatUX $(TARGET_DIR)/usr/share/retroarch/assets
	cp -r $(@D)/xmb/monochrome $(TARGET_DIR)/usr/share/retroarch/assets/xmb
	cp -r $(@D)/menu_widgets $(TARGET_DIR)/usr/share/retroarch/assets
endef

$(eval $(generic-package))
