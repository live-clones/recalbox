################################################################################
#
# CHEATS
#
################################################################################

LIBRETRO_CHEATS_VERSION = 53ab2a9bdd5376d33ae1e732eb3cdc70611bc987
LIBRETRO_CHEATS_SITE = $(call github,libretro,libretro-database,$(LIBRETRO_CHEATS_VERSION))

define LIBRETRO_CHEATS_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/recalbox/share_init/cheats
	cp -r $(@D)/cht \
		$(TARGET_DIR)/recalbox/share_init/cheats
endef

$(eval $(generic-package))
