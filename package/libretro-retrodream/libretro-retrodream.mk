################################################################################
#
# RETRODREAM
#
################################################################################

LIBRETRO_RETRODREAM_VERSION = 22d84415d884e734961c49b735567eeb2c198a90
LIBRETRO_RETRODREAM_SITE = $(call github,libretro,retrodream,$(LIBRETRO_RETRODREAM_VERSION))
LIBRETRO_RETRODREAM_LICENSE = GPLv3

define LIBRETRO_RETRODREAM_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/deps/libretro/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" AR="$(TARGET_AR)" -C $(@D)/deps/libretro -f Makefile
endef

define LIBRETRO_RETRODREAM_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/retrodream_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/retrodream_libretro.so
endef

$(eval $(generic-package))
