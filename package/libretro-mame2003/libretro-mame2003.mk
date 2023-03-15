################################################################################
#
# MAME2003
#
################################################################################

# MAME 2003 - 24/10/2021
LIBRETRO_MAME2003_VERSION = 1da9b28a07dc8bfd896ae97c11696318d86de10b
LIBRETRO_MAME2003_SITE = $(call github,libretro,mame2003-libretro,$(LIBRETRO_MAME2003_VERSION))
LIBRETRO_MAME2003_LICENSE = MAME
LIBRETRO_MAME2003_NON_COMMERCIAL = y

LIBRETRO_MAME2003_DEPENDENCIES = arcade-dats

define LIBRETRO_MAME2003_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	mkdir -p $(@D)/obj/mame/cpu/ccpu
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(RETROARCH_LIBRETRO_BOARD)"
endef

define LIBRETRO_MAME2003_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/recalbox/system/arcade/dats/libretro-mame2003
	cp $(@D)/metadata/mame2003.xml $(TARGET_DIR)/recalbox/system/arcade/dats/libretro-mame2003
	mkdir -p $(TARGET_DIR)/recalbox/system/arcade/flats
	xsltproc --stringparam lastmamexml $(ARCADE_DATS_FULLARCADE_DAT) $(ARCADE_DATS_DIR)/arcade.xslt \
		$(TARGET_DIR)/recalbox/system/arcade/dats/libretro-mame2003/mame2003.xml > $(TARGET_DIR)/recalbox/system/arcade/flats/mame2003.lst
	$(INSTALL) -D $(@D)/mame2003_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mame2003_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_upgrade/bios/mame2003/samples
	cp -R $(@D)/metadata/* $(TARGET_DIR)/recalbox/share_upgrade/bios/mame2003
	rm $(TARGET_DIR)/recalbox/share_upgrade/bios/mame2003/mame2003.xml
endef

$(eval $(generic-package))
