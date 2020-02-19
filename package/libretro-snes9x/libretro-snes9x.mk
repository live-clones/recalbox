################################################################################
#
# SNES9X
#
################################################################################

LIBRETRO_SNES9X_VERSION = 6db918cfe32b157239da44096091c212fdfb3b60
LIBRETRO_SNES9X_SITE = $(call github,libretro,snes9x,$(LIBRETRO_SNES9X_VERSION))

define LIBRETRO_SNES9X_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/libretro/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO) -std=c++11" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/libretro -f Makefile platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_SNES9X_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/libretro/snes9x_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/snes9x_libretro.so
endef

$(eval $(generic-package))
