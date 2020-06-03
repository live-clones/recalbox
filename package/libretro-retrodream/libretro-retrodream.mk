################################################################################
#
# RETRODREAM
#
################################################################################

LIBRETRO_RETRODREAM_VERSION = 15b5f54f08b1b26685c73a18824d2aca3b4792b7
LIBRETRO_RETRODREAM_SITE = $(call github,libretro,retrodream,$(LIBRETRO_RETRODREAM_VERSION))
LIBRETRO_RETRODREAM_LICENSE = GPLv3

define LIBRETRO_RETRODREAM_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/deps/libretro/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" AR="$(TARGET_AR)" -C $(@D)/deps/libretro -f Makefile platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_RETRODREAM_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/deps/libretro/retrodream_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/retrodream_libretro.so
endef

$(eval $(generic-package))
