################################################################################
#
# CHEATS
#
################################################################################

LIBRETRO_CHEATS_VERSION = v1.9.0
LIBRETRO_CHEATS_SITE = $(call github,libretro,libretro-database,$(LIBRETRO_CHEATS_VERSION))
LIBRETRO_CHEATS_LICENSE = MIT
LIBRETRO_CHEATS_LICENSE_FILES = COPYING

define LIBRETRO_CHEATS_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/recalbox/share_init/cheats
	cp -r $(@D)/cht/* \
		$(TARGET_DIR)/recalbox/share_init/cheats
endef

$(eval $(generic-package))
