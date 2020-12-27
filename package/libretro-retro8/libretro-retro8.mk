################################################################################
#
# RETRO8
#
################################################################################

LIBRETRO_RETRO8_VERSION = b82bc9b08b757fb4a00a0d8f90605b33e998195d
LIBRETRO_RETRO8_SITE = $(call github,libretro,retro8,$(LIBRETRO_RETRO8_VERSION))

define LIBRETRO_RETRO8_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_RETRO8_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/retro8_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/retro8_libretro.so
endef

$(eval $(generic-package))
