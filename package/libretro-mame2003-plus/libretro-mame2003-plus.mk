################################################################################
#
# MAME2003_PLUS
#
################################################################################

# Version of 2022-03-01
LIBRETRO_MAME2003_PLUS_VERSION = 29ac9a30929f82f68424e10175e6b64e06136b27
LIBRETRO_MAME2003_PLUS_SITE = $(call github,libretro,mame2003-plus-libretro,$(LIBRETRO_MAME2003_PLUS_VERSION))
LIBRETRO_MAME2003_PLUS_LICENSE = MAME
LIBRETRO_MAME2003_PLUS_NON_COMMERCIAL = y

LIBRETRO_MAME2003_PLUS_DEPENDENCIES = arcade-dats

define LIBRETRO_MAME2003_PLUS_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	mkdir -p $(@D)/obj/mame/cpu/ccpu
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(RETROARCH_LIBRETRO_BOARD)"
endef

define LIBRETRO_MAME2003_PLUS_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/recalbox/system/arcade/dats/libretro-mame2003-plus
	cp $(@D)/metadata/mame2003-plus.xml $(TARGET_DIR)/recalbox/system/arcade/dats/libretro-mame2003-plus
	mkdir -p $(TARGET_DIR)/recalbox/system/arcade/flats
	xsltproc --stringparam lastmamexml $(ARCADE_DATS_FULLARCADE_DAT) $(ARCADE_DATS_DIR)/arcade.xslt \
		$(TARGET_DIR)/recalbox/system/arcade/dats/libretro-mame2003-plus/mame2003-plus.xml > $(TARGET_DIR)/recalbox/system/arcade/flats/mame2003-plus.lst
	$(INSTALL) -D $(@D)/mame2003_plus_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mame2003_plus_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_upgrade/bios/mame2003-plus/samples
	cp -R $(@D)/metadata/* $(TARGET_DIR)/recalbox/share_upgrade/bios/mame2003-plus
	rm $(TARGET_DIR)/recalbox/share_upgrade/bios/mame2003-plus/mame2003-plus.xml
endef

$(eval $(generic-package))
