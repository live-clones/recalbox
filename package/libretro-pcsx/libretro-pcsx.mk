################################################################################
#
# PCSX_REARMED
#
################################################################################

LIBRETRO_PCSX_VERSION = e1d8eb0f131f9e7e838ec5658345e51ebcc3179c
LIBRETRO_PCSX_SITE = $(call github,libretro,pcsx_rearmed,$(LIBRETRO_PCSX_VERSION))

define LIBRETRO_PCSX_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile.libretro
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" AR="$(TARGET_AR)" -C $(@D) -f Makefile.libretro platform="$(RETROARCH_LIBRETRO_BOARD)"
endef

define LIBRETRO_PCSX_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/pcsx_rearmed_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/pcsx_rearmed_libretro.so
endef

define LIBRETRO_PCSX_PRE_PATCH_FIXUP
	$(SED) 's/\r//g' $(@D)/libpcsxcore/plugins.c
endef

LIBRETRO_PCSX_PRE_PATCH_HOOKS += LIBRETRO_PCSX_PRE_PATCH_FIXUP

$(eval $(generic-package))
