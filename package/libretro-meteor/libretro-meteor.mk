################################################################################
#
# METEOR
#
################################################################################

LIBRETRO_METEOR_VERSION = 3d21e3b5a7596918bee0fcaca5752ae76624c05e
LIBRETRO_METEOR_SITE = $(call github,libretro,meteor-libretro,$(LIBRETRO_METEOR_VERSION))
LIBRETRO_METEOR_LICENSE = GPL-3.0
LIBRETRO_METEOR_LICENSE_FILES = COPYING

define LIBRETRO_METEOR_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/libretro/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" LD="$(TARGET_LD)" AR="$(TARGET_AR)" RANLIB="$(TARGET_RANLIB)" -C $(@D)/libretro platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_METEOR_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/libretro/meteor_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/meteor_libretro.so
endef

$(eval $(generic-package))
