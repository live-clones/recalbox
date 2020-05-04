################################################################################
#
# DOSBOX_CORE
#
################################################################################

LIBRETRO_DOSBOX_CORE_VERSION = 5521e6f216f1084613f5c15c8b44c06541cc85c2
LIBRETRO_DOSBOX_CORE_SITE = $(call github,libretro,dosbox-core,$(LIBRETRO_DOSBOX_CORE_VERSION))
LIBRETRO_DOSBOX_CORE_DEPENDENCIES = sdl sdl_net
LIBRETRO_DOSBOX_CORE_GIT_SUBMODULES = YES

define LIBRETRO_DOSBOX_CORE_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile.libretro
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(RETROARCH_LIBRETRO_BOARD)"
endef

define LIBRETRO_DOSBOX_CORE_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/libretro/dosbox_core_libretro.so \
		$(TARGET_DIR)/usr/bin/dosbox_core_libretro.so
endef

$(eval $(generic-package))
