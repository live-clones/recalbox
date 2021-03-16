################################################################################
#
# RETRO8
#
################################################################################

LIBRETRO_RETRO8_VERSION = 31b144f3a9e2c3982e83dbcda7bd61848d0aa6e1
LIBRETRO_RETRO8_SITE = $(call gitlab,recalbox,packages/libretro/libretro-retro8,$(LIBRETRO_RETRO8_VERSION))
LIBRETRO_RETRO8_LICENSE = GPL-3.0
LIBRETRO_RETRO8_LICENSE_FILES = LICENSE

define LIBRETRO_RETRO8_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="unix"
endef

define LIBRETRO_RETRO8_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/retro8_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/retro8_libretro.so
endef

$(eval $(generic-package))
