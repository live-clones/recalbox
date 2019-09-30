################################################################################
#
# SNES9X2005 / CATSFC
#
################################################################################

LIBRETRO_SNES9X2005_VERSION = e5cadd2f21fb64e8c7194ad006b39e6f555c4a5b
LIBRETRO_SNES9X2005_SITE = $(call github,libretro,snes9x2005,$(LIBRETRO_SNES9X2005_VERSION))

define LIBRETRO_SNES9X2005_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D) platform="$(RETROARCH_LIBRETRO_PLATFORM)" USE_BLARGG_APU=1
endef

define LIBRETRO_SNES9X2005_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/snes9x2005_plus_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/snes9x2005_libretro.so
endef

$(eval $(generic-package))
