################################################################################
#
# ECWOLF
#
################################################################################

LIBRETRO_ECWOLF_VERSION = 65c7d0fa4ab7bf8601cc9de7b7eba2f73b0273fe
LIBRETRO_ECWOLF_SITE = $(call github,libretro,ecwolf,$(LIBRETRO_ECWOLF_VERSION))
LIBRETRO_ECWOLF_LICENSE = GPL-2.0, ID-Software, MAME
LIBRETRO_ECWOLF_LICENSE_FILES = docs/license-gpl.txt docs/license-id.txt docs/license-mame.txt
LIBRETRO_ECWOLF_NON_COMMERCIAL = y

define LIBRETRO_ECWOLF_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/src/libretro/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/src/libretro/ -f Makefile platform="unix"
endef

define LIBRETRO_ECWOLF_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/src/libretro/ecwolf_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/ecwolf_libretro.so
endef

$(eval $(generic-package))
