################################################################################
#
# MAME2000 / IMAME
#
################################################################################

LIBRETRO_MAME2000_VERSION = 0a8a174f5e755cdd476895207003c5d07cfa6af2
LIBRETRO_MAME2000_SITE = $(call github,libretro,mame2000-libretro,$(LIBRETRO_MAME2000_VERSION))

define LIBRETRO_MAME2000_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	mkdir -p $(@D)/obj_libretro_libretro/cpu
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CC="$(TARGET_CC)" -C $(@D) -f Makefile ARM=1
endef

define LIBRETRO_MAME2000_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mame2000_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mame2000_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_init/bios/mame2000/samples
	cp -R $(@D)/metadata/* $(TARGET_DIR)/recalbox/share_init/bios/mame2000
endef

$(eval $(generic-package))
