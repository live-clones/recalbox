################################################################################
#
# MAME2015
#
################################################################################

#Commit version of 19/01/2022
LIBRETRO_MAME2015_VERSION = e6a7aa4d53726e61498f68d6b8e2c092a2169fa2
LIBRETRO_MAME2015_SITE = $(call github,libretro,mame2015-libretro,$(LIBRETRO_MAME2015_VERSION))
LIBRETRO_MAME2015_LICENSE = MAME
LIBRETRO_MAME2015_NON_COMMERCIAL = y

LIBRETRO_MAME2015_DEPENDENCIES = arcade-dats 

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_ODROIDXU4),y)
LIBRETRO_MAME2015_CFLAGSO = $(COMPILER_COMMONS_CFLAGS_NOLTO)
LIBRETRO_MAME2015_CXXFLAGSO = $(COMPILER_COMMONS_CXXFLAGS_NOLTO)
LIBRETRO_MAME2015_LDFLAGS = $(COMPILER_COMMONS_LDFLAGS_NOLTO)
else
LIBRETRO_MAME2015_CFLAGSO = $(COMPILER_COMMONS_CFLAGS_SO)
LIBRETRO_MAME2015_CXXFLAGSO = $(COMPILER_COMMONS_CXXFLAGS_SO)
LIBRETRO_MAME2015_LDFLAGS = $(COMPILER_COMMONS_LDFLAGS_SO)
endif

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_ODROIDGO2)$(BR2_PACKAGE_RECALBOX_TARGET_RPI4_64),y)
LIBRETRO_MAME2015_OPTS = PTR64=1
LIBRETRO_MAME2015_AARCH64OPTS = -DSDLMAME_NO64BITIO -DIOS -DSDLMAME_ARM -DHAVE_POSIX_MEMALIGN
else
LIBRETRO_MAME2015_AARCH64OPTS =
endif

define LIBRETRO_MAME2015_BUILD_CMDS
	$(SED) "s|^CONLYFLAGS =|CONLYFLAGS = $(LIBRETRO_MAME2015_CFLAGSO) $(LIBRETRO_MAME2015_AARCH64OPTS)|g" $(@D)/Makefile
	$(SED) "s|^CPPONLYFLAGS =|CPPONLYFLAGS = $(LIBRETRO_MAME2015_CXXFLAGSO) $(LIBRETRO_MAME2015_AARCH64OPTS)|g" $(@D)/Makefile
	$(SED) "s|^LDFLAGS =|LDFLAGS = $(LIBRETRO_MAME2015_LDFLAGS) -lm|g" $(@D)/Makefile
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	$(SED) "s|-O0|-O3|g" $(@D)/src/lib/lib.mak
	$(MAKE) REALCC="$(TARGET_CC)" CC="$(TARGET_CC)" CXX="$(TARGET_CXX)" LD="$(TARGET_CC)" AR="$(TARGET_CC)-ar" \
		-C $(@D)/ -f Makefile platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_MAME2015_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/recalbox/system/arcade/dats/libretro-mame2015
	$(UNZIP) -o -d $(TARGET_DIR)/recalbox/system/arcade/dats/libretro-mame2015 $(@D)/metadata/mame2015-xml.zip
	mkdir -p $(TARGET_DIR)/recalbox/system/arcade/flats
	xsltproc --stringparam lastmamexml $(ARCADE_DATS_FULLARCADE_DAT) $(ARCADE_DATS_DIR)/arcade.xslt \
		$(TARGET_DIR)/recalbox/system/arcade/dats/libretro-mame2015/mame2014.xml > $(TARGET_DIR)/recalbox/system/arcade/flats/mame2015.lst
	$(INSTALL) -D $(@D)/mame2015_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mame2015_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_upgrade/bios/mame2015/samples
	cp -R $(@D)/metadata/* $(TARGET_DIR)/recalbox/share_upgrade/bios/mame2015
	rm $(TARGET_DIR)/recalbox/share_upgrade/bios/mame2015/mame2015-xml.zip
endef

$(eval $(generic-package))
