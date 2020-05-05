################################################################################
#
# ATARI800
#
################################################################################

LIBRETRO_ATARI800_VERSION = 59820eb2b007a9d0e76f0380dfb0580c96bd14e8
LIBRETRO_ATARI800_SITE = git://github.com/libretro/libretro-atari800.git
LIBRETRO_ATARI800_SITE_METHOD=git
LIBRETRO_ATARI800_GIT_SUBMODULES=y
LIBRETRO_ATARI800_LICENSE = GPL

define LIBRETRO_ATARI800_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_NOLTO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_NOLTO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_NOLTO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_ATARI800_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/atari800_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/atari800_libretro.so
endef

$(eval $(generic-package))
