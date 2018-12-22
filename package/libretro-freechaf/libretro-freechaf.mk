################################################################################
#
# FREECHAF
#
################################################################################
LIBRETRO_FREECHAF_VERSION = 0b3e90406e43f6bc78cec3fbd2bb4cad40ad06d5
LIBRETRO_FREECHAF_SITE = $(call github,libretro,FreeChaF,$(LIBRETRO_FREECHAF_VERSION))

define LIBRETRO_FREECHAF_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" $(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(LIBRETRO_PLATFORM)"
endef

define LIBRETRO_FREECHAF_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/freechaf_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/freechaf_libretro.so
endef

$(eval $(generic-package))