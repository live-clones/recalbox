################################################################################
#
# PCSX_REARMED
#
################################################################################

LIBRETRO_PCSX_REARMED_VERSION = dfcd7153048c2f6d479dfcd328ee2c0e1e0784ed
LIBRETRO_PCSX_REARMED_SITE = $(call github,libretro,pcsx_rearmed,$(LIBRETRO_PCSX_REARMED_VERSION))

define LIBRETRO_PCSX_REARMED_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile.libretro
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" AR="$(TARGET_AR)" -C $(@D) -f Makefile.libretro platform="$(RETROARCH_LIBRETRO_BOARD)"
endef

define LIBRETRO_PCSX_REARMED_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/pcsx_rearmed_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/pcsx_rearmed_libretro.so
endef

define LIBRETRO_PCSX_REARMED_PRE_PATCH_FIXUP
	$(SED) 's/\r//g' $(@D)/libpcsxcore/plugins.c
endef

LIBRETRO_PCSX_REARMED_PRE_PATCH_HOOKS += LIBRETRO_PCSX_REARMED_PRE_PATCH_FIXUP

$(eval $(generic-package))
