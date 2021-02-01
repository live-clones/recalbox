################################################################################
#
# MINIVMAC
#
################################################################################

LIBRETRO_MINIVMAC_VERSION = 91e6ee6e7b32e71cb3ecf60a04d187690b3e06f9
LIBRETRO_MINIVMAC_SITE = $(call github,libretro,libretro-minivmac,$(LIBRETRO_MINIVMAC_VERSION))
LIBRETRO_MINIVMAC_LICENSE = GPL-1.0

define LIBRETRO_MINIVMAC_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" LD="$(TARGET_CXX)" AR="$(TARGET_AR)" RANLIB="$(TARGET_RANLIB)" -C $(@D)/ -f Makefile platform="unix"
endef

define LIBRETRO_MINIVMAC_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/minivmac_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/minivmac_libretro.so
endef

$(eval $(generic-package))
