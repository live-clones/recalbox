################################################################################
#
# CHEATS
#
################################################################################

LIBRETRO_CHEATS_VERSION = 69e12b1bec0633a7b63e83506c0b04d5a658e660
LIBRETRO_CHEATS_SITE = $(call github,libretro,libretro-database,$(LIBRETRO_CHEATS_VERSION))

define LIBRETRO_CHEATS_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/recalbox/share_init/cheats
	cp -r $(@D)/cht \
		$(TARGET_DIR)/recalbox/share_init/cheats
endef

$(eval $(generic-package))
