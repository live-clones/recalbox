################################################################################
#
# MAME2015
#
################################################################################

LIBRETRO_MAME2015_VERSION = 80a2e82cf96ddcc900286c5e1d877bb99700bd6b
LIBRETRO_MAME2015_SITE = $(call github,libretro,mame2015-libretro,$(LIBRETRO_MAME2015_VERSION))
LIBRETRO_MAME2015_LICENSE = MAME

define LIBRETRO_MAME2015_BUILD_CMDS
	$(SED) "s|^CONLYFLAGS =|CONLYFLAGS = $(COMPILER_COMMONS_CFLAGS_SO)|g" $(@D)/Makefile
	$(SED) "s|^CPPONLYFLAGS =|CPPONLYFLAGS = $(COMPILER_COMMONS_CXXFLAGS_SO)|g" $(@D)/Makefile
	$(SED) "s|^LDFLAGS =|LDFLAGS = $(COMPILER_COMMONS_LDFLAGS_SO) -lm|g" $(@D)/Makefile
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	$(SED) "s|-O0|-O3|g" $(@D)/src/lib/lib.mak
	$(MAKE) REALCC="$(TARGET_CC)" CC="$(TARGET_CC)" CXX="$(TARGET_CXX)" LD="$(TARGET_CC)" AR="$(TARGET_CC)-ar" \
		-C $(@D)/ -f Makefile platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_MAME2015_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mame2015_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mame2015_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_init/bios/mame2015/samples
	cp -R $(@D)/metadata/* $(TARGET_DIR)/recalbox/share_init/bios/mame2015
endef

$(eval $(generic-package))
