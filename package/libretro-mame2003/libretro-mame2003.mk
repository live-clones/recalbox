################################################################################
#
# MAME2003
#
################################################################################

LIBRETRO_MAME2003_VERSION = 82596014905ad38c80c9eb322ab08c625d1d92cd
LIBRETRO_MAME2003_SITE = $(call github,libretro,mame2003-libretro,$(LIBRETRO_MAME2003_VERSION))

define LIBRETRO_MAME2003_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	mkdir -p $(@D)/obj/mame/cpu/ccpu
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(RETROARCH_LIBRETRO_BOARD)"
endef

define LIBRETRO_MAME2003_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mame2003_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mame2003_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_init/bios/mame2003/samples
	cp -R $(@D)/metadata/* $(TARGET_DIR)/recalbox/share_init/bios/mame2003
endef

$(eval $(generic-package))
