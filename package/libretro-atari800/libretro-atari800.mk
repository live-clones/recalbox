################################################################################
#
# ATARI800
#
################################################################################

LIBRETRO_ATARI800_VERSION = 59820eb2b007a9d0e76f0380dfb0580c96bd14e8
LIBRETRO_ATARI800_LICENSE = GPL
LIBRETRO_ATARI800_SITE = $(call github,libretro,libretro-atari800,$(LIBRETRO_ATARI800_VERSION))

define LIBRETRO_ATARI800_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_ATARI800_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/atari800_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/atari800_libretro.so
endef

$(eval $(generic-package))
