################################################################################
#
# NESTOPIA
#
################################################################################
LIBRETRO_NESTOPIA_VERSION = 9fa43cb29e0598cf6147e5ca1802f2e3bbd4f40f
LIBRETRO_NESTOPIA_SITE = $(call github,libretro,nestopia,$(LIBRETRO_NESTOPIA_VERSION))

define LIBRETRO_NESTOPIA_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" $(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/libretro/ platform="$(LIBRETRO_BOARD)" GIT_VERSION=" \"9fa43cb\""
endef

define LIBRETRO_NESTOPIA_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/libretro/nestopia_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/nestopia_libretro.so
endef

$(eval $(generic-package))
