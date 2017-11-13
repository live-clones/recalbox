################################################################################
#
# LUTRO
#
################################################################################
LIBRETRO_LUTRO_VERSION = d9cacd3d66d01d2af5e5e4a1f959f4082eca1e40
LIBRETRO_LUTRO_SITE = $(call github,libretro,libretro-lutro,$(LIBRETRO_LUTRO_VERSION))

define LIBRETRO_LUTRO_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" $(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(LIBRETRO_PLATFORM)"
endef

define LIBRETRO_LUTRO_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/lutro_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/lutro_libretro.so
endef

$(eval $(generic-package))
