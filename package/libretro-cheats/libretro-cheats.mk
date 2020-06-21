################################################################################
#
# CHEATS
#
################################################################################

LIBRETRO_CHEATS_VERSION = f04e06c18945cf24957a7b44fafef35b40de0d53
LIBRETRO_CHEATS_SITE = $(call github,libretro,libretro-database,$(LIBRETRO_CHEATS_VERSION))

define LIBRETRO_CHEATS_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/recalbox/share_init/cheats
	cp -r $(@D)/cht \
		$(TARGET_DIR)/recalbox/share_init/cheats
endef

$(eval $(generic-package))
