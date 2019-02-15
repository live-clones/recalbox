################################################################################
#
# SNES9X2005 / CATSFC
#
################################################################################
LIBRETRO_SNES9X2005_VERSION = 09e85e3e49feb9c0f7b95c5f53ca1a72989a94f2
LIBRETRO_SNES9X2005_SITE = $(call github,libretro,snes9x2005,$(LIBRETRO_SNES9X2005_VERSION))

define LIBRETRO_SNES9X2005_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" $(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D) platform="$(LIBRETRO_PLATFORM)"
endef

define LIBRETRO_SNES9X2005_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/snes9x2005_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/snes9x2005_libretro.so
endef

$(eval $(generic-package))
