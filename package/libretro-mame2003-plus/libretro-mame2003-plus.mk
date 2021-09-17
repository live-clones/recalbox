################################################################################
#
# MAME2003_PLUS
#
################################################################################

LIBRETRO_MAME2003_PLUS_VERSION = b31b606e03c3aa3405d1a798add74d57f62bd434
LIBRETRO_MAME2003_PLUS_SITE = $(call github,libretro,mame2003-plus-libretro,$(LIBRETRO_MAME2003_PLUS_VERSION))
LIBRETRO_MAME2003_PLUS_LICENSE = MAME
LIBRETRO_MAME2003_PLUS_NON_COMMERCIAL = y

define LIBRETRO_MAME2003_PLUS_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	mkdir -p $(@D)/obj/mame/cpu/ccpu
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(RETROARCH_LIBRETRO_BOARD)"
endef

define LIBRETRO_MAME2003_PLUS_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mame2003_plus_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mame2003_plus_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_upgrade/bios/mame2003-plus/samples
	cp -R $(@D)/metadata/* $(TARGET_DIR)/recalbox/share_upgrade/bios/mame2003-plus
endef

$(eval $(generic-package))
