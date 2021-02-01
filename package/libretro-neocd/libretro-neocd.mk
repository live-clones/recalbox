################################################################################
#
# NEOCD
#
################################################################################

LIBRETRO_NEOCD_VERSION = aedbf12d4df2d2ee4dbc584ecb5102bab29aff59
LIBRETRO_NEOCD_SITE = git://github.com/libretro/neocd_libretro.git
LIBRETRO_NEOCD_LICENSE = LGPLv3

LIBRETRO_NEOCD_GIT_SUBMODULES=y

define LIBRETRO_NEOCD_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="unix"
endef

define LIBRETRO_NEOCD_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/neocd_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/neocd_libretro.so
endef

$(eval $(generic-package))
