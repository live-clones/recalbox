################################################################################
#
# GENESISPLUSGX
#
################################################################################
LIBRETRO_GENESISPLUSGX_VERSION = 365a28c7349b691e6aaa3ad59b055261c42bd130
LIBRETRO_GENESISPLUSGX_SITE = $(call github,libretro,Genesis-Plus-GX,$(LIBRETRO_GENESISPLUSGX_VERSION))

define LIBRETRO_GENESISPLUSGX_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" $(MAKE) GIT_VERSION="\" 365a28c\"" CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D) -f Makefile.libretro platform="$(LIBRETRO_BOARD)"
endef

define LIBRETRO_GENESISPLUSGX_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/genesis_plus_gx_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/genesisplusgx_libretro.so
endef

$(eval $(generic-package))
