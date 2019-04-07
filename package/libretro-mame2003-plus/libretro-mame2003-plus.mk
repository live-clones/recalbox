################################################################################
#
# MAME2003_PLUS
#
################################################################################
LIBRETRO_MAME2003_PLUS_VERSION = c59c1f7d0f2281f9ed429b3cd0803799ccf3ba61
LIBRETRO_MAME2003_PLUS_SITE = $(call github,libretro,mame2003-plus-libretro,$(LIBRETRO_MAME2003_PLUS_VERSION))

define LIBRETRO_MAME2003_PLUS_BUILD_CMDS
	mkdir -p $(@D)/obj/mame/cpu/ccpu
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" $(MAKE) CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(LIBRETRO_BOARD)"
endef

define LIBRETRO_MAME2003_PLUS_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mame2003_plus_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mame2003_plus_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_init/bios/mame2003-plus/samples
	cp -R $(@D)/metadata/* $(TARGET_DIR)/recalbox/share_init/bios/mame2003-plus
endef

$(eval $(generic-package))