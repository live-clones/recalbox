################################################################################
#
# SNES9X_NEXT / SNES9X2010
#
################################################################################
LIBRETRO_SNES9X_NEXT_VERSION = b2a69de0df1eb39ed362806f9c9633f4544272af
LIBRETRO_SNES9X_NEXT_SITE = $(call github,libretro,snes9x2010,$(LIBRETRO_SNES9X_NEXT_VERSION))

define LIBRETRO_SNES9X_NEXT_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" $(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile.libretro platform="$(LIBRETRO_BOARD)" GIT_VERSION=" \"b2a69de\""
endef

define LIBRETRO_SNES9X_NEXT_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/snes9x2010_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/snes9x_next_libretro.so
endef

$(eval $(generic-package))
