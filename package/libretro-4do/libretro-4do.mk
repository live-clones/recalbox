################################################################################
#
# 4DO
#
################################################################################
LIBRETRO_4DO_VERSION = 2a06cd1f6e4987e021e86c9652f0a7eecc2b1130
LIBRETRO_4DO_SITE = $(call github,libretro,4do-libretro,$(LIBRETRO_4DO_VERSION))

define LIBRETRO_4DO_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" $(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D) -f Makefile platform="unix $(LIBRETRO_PLATFORM)"
endef

define LIBRETRO_4DO_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/4do_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/4do_libretro.so
endef

$(eval $(generic-package))
