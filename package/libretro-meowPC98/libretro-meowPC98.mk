################################################################################
#
# MEOWPC98
#
################################################################################
LIBRETRO_MEOWPC98_VERSION = 2b388491037c3f7bdc17869c4846e3c0bd2349a6
LIBRETRO_MEOWPC98_SITE = $(call github,libretro,libretro-meowPC98,$(LIBRETRO_MEOWPC98_VERSION))
LIBRETRO_MEOWPC98_LICENSE = MIT


define LIBRETRO_MEOWPC98_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" $(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/libretro -f Makefile platform="$(LIBRETRO_PLATFORM)"
endef


define LIBRETRO_MEOWPC98_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/libretro/nekop2_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/meowPC98_libretro.so
endef


$(eval $(generic-package))