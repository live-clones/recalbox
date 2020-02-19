################################################################################
#
# SNES9X2005 / CATSFC
#
################################################################################

LIBRETRO_SNES9X2005_VERSION = c216559b9e0dc3d7f059dcf31b813402ad47fea5
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
