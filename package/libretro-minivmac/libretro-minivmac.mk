################################################################################
#
# MINIVMAC
#
################################################################################

LIBRETRO_MINIVMAC_VERSION = 91e6ee6e7b32e71cb3ecf60a04d187690b3e06f9
LIBRETRO_MINIVMAC_SITE = $(call github,libretro,libretro-minivmac,$(LIBRETRO_MINIVMAC_VERSION))

define LIBRETRO_MINIVMAC_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" LD="$(TARGET_CXX)" AR="$(TARGET_AR)" RANLIB="$(TARGET_RANLIB)" -C $(@D)/ -f Makefile platform="unix"
endef

define LIBRETRO_MINIVMAC_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/minivmac_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/minivmac_libretro.so
endef

$(eval $(generic-package))