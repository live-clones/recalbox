################################################################################
#
# CANNONBALL
#
################################################################################

LIBRETRO_CANNONBALL_VERSION = 5dcef189dc5c408bbd9e4bc08b1f9801b04cd3ca
LIBRETRO_CANNONBALL_SITE = $(call github,libretro,cannonball,$(LIBRETRO_CANNONBALL_VERSION))
LIBRETRO_CANNONBALL_DEPENDENCIES = recalbox-romfs2
LIBRETRO_CANNONBALL_LICENSE = COPYRIGHT
LIBRETRO_CANNONBALL_NON_COMMERCIAL = y

define LIBRETRO_CANNONBALL_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_CANNONBALL_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/cannonball_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/cannonball_libretro.so
	$(INSTALL) -D $(@D)/roms/roms.txt "$(TARGET_DIR)/recalbox/share_init/roms/ports/Out Run/roms_needed.txt"
	mkdir -p "$(TARGET_DIR)/recalbox/share_init/roms/ports/Out Run/res"
	$(INSTALL) -D $(@D)/res/*.bin "$(TARGET_DIR)/recalbox/share_init/roms/ports/Out Run/res/"
endef

$(eval $(generic-package))
