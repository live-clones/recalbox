################################################################################
#
# GEARCOLECO
#
################################################################################

# Commit: 2021-09-06
LIBRETRO_GEARCOLECO_VERSION = 223b96c6f92fe2ece238df8a463334e2dfadaa7d
LIBRETRO_GEARCOLECO_SITE = $(call gitlab,recalbox,packages/libretro/Gearcoleco,$(LIBRETRO_GEARCOLECO_VERSION))
LIBRETRO_GEARCOLECO_LICENSE = GPL-3.0
LIBRETRO_GEARCOLECO_LICENSE_FILES = LICENSE
LIBRETRO_GEARCOLECO_NON_COMMERCIAL = NO

define LIBRETRO_GEARCOLECO_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/platforms/libretro/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/platforms/libretro -f Makefile platform="unix"
endef

define LIBRETRO_GEARCOLECO_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/platforms/libretro/gearcoleco_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/gearcoleco_libretro.so
endef

$(eval $(generic-package))
