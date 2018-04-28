################################################################################
#
# HANDY_LYNX
#
################################################################################
LIBRETRO_HANDY_VERSION = 64e5f90525724032fc7a3f4a4e941e50f169620f
LIBRETRO_HANDY_SITE = $(call github,libretro,libretro-handy,$(LIBRETRO_HANDY_VERSION))


define LIBRETRO_HANDY_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" $(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(LIBRETRO_PLATFORM)"
endef

define LIBRETRO_HANDY_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/handy_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/handy_libretro.so
endef

$(eval $(generic-package))
