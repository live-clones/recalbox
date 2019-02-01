################################################################################
#
# MAME2003_PLUS
#
################################################################################
LIBRETRO_MAME2003_PLUS_VERSION = 56fe71d5906f8ed7fc2af56ca9d8aefbd6476c2c
LIBRETRO_MAME2003_PLUS_SITE = $(call github,libretro,mame2003-plus-libretro,$(LIBRETRO_MAME2003_PLUS_VERSION))

define LIBRETRO_MAME2003_PLUS_BUILD_CMDS
	mkdir -p $(@D)/obj/mame/cpu/ccpu
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" $(MAKE) CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(LIBRETRO_BOARD)"
endef

define LIBRETRO_MAME2003_PLUS_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mame2003_plus_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mame2003_plus_libretro.so
endef

$(eval $(generic-package))
