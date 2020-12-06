################################################################################
#
# GPSP
#
################################################################################

LIBRETRO_GPSP_VERSION = 856af16ba64951d99a6a9aa84b1a10dc1540a868
LIBRETRO_GPSP_SITE = $(call github,libretro,gpsp,$(LIBRETRO_GPSP_VERSION))

define LIBRETRO_GPSP_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS) -freorder-blocks-and-partition" CXXFLAGS="$(TARGET_CXXFLAGS) -freorder-blocks-and-partition" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D) platform="$(RETROARCH_LIBRETRO_BOARD)" OPTIMIZE="$(COMPILER_COMMONS_LDFLAGS_SO)"
endef

define LIBRETRO_GPSP_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/gpsp_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/gpsp_libretro.so
endef

$(eval $(generic-package))
