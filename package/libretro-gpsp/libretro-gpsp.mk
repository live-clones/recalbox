################################################################################
#
# GPSP
#
################################################################################

LIBRETRO_GPSP_VERSION = d8cf63b9c6b277c347e8e7babc9214d25d17b768
LIBRETRO_GPSP_SITE = $(call github,libretro,gpsp,$(LIBRETRO_GPSP_VERSION))

define LIBRETRO_GPSP_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D) platform="$(RETROARCH_LIBRETRO_BOARD)" OPTIMIZE="$(COMPILER_COMMONS_LDFLAGS_SO)"
endef

define LIBRETRO_GPSP_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/gpsp_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/gpsp_libretro.so
endef

$(eval $(generic-package))
