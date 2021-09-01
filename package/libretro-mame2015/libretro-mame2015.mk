################################################################################
#
# MAME2015
#
################################################################################

LIBRETRO_MAME2015_VERSION = 969bf4a4376866f046c885237ee6aee37fe04bf4
LIBRETRO_MAME2015_SITE = $(call github,libretro,mame2015-libretro,$(LIBRETRO_MAME2015_VERSION))
LIBRETRO_MAME2015_LICENSE = MAME
LIBRETRO_MAME2015_NON_COMMERCIAL = y

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_ODROIDXU4),y)
LIBRETRO_MAME2015_CFLAGSO = $(COMPILER_COMMONS_CFLAGS_NOLTO)
LIBRETRO_MAME2015_CXXFLAGSO = $(COMPILER_COMMONS_CXXFLAGS_NOLTO)
LIBRETRO_MAME2015_LDFLAGS = $(COMPILER_COMMONS_LDFLAGS_NOLTO)
else
LIBRETRO_MAME2015_CFLAGSO = $(COMPILER_COMMONS_CFLAGS_SO)
LIBRETRO_MAME2015_CXXFLAGSO = $(COMPILER_COMMONS_CXXFLAGS_SO)
LIBRETRO_MAME2015_LDFLAGS = $(COMPILER_COMMONS_LDFLAGS_SO)
endif

define LIBRETRO_MAME2015_BUILD_CMDS
	$(SED) "s|^CONLYFLAGS =|CONLYFLAGS = $(LIBRETRO_MAME2015_CFLAGSO)|g" $(@D)/Makefile
	$(SED) "s|^CPPONLYFLAGS =|CPPONLYFLAGS = $(LIBRETRO_MAME2015_CXXFLAGSO)|g" $(@D)/Makefile
	$(SED) "s|^LDFLAGS =|LDFLAGS = $(LIBRETRO_MAME2015_LDFLAGS) -lm|g" $(@D)/Makefile
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	$(SED) "s|-O0|-O3|g" $(@D)/src/lib/lib.mak
	$(MAKE) REALCC="$(TARGET_CC)" CC="$(TARGET_CC)" CXX="$(TARGET_CXX)" LD="$(TARGET_CC)" AR="$(TARGET_CC)-ar" \
		-C $(@D)/ -f Makefile platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_MAME2015_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mame2015_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mame2015_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_upgrade/bios/mame2015/samples
	cp -R $(@D)/metadata/* $(TARGET_DIR)/recalbox/share_upgrade/bios/mame2015
endef

$(eval $(generic-package))
