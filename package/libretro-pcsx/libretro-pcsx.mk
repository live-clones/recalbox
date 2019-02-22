################################################################################
#
# PCSX_REARMED
#
################################################################################
LIBRETRO_PCSX_VERSION = 8569f7c3abe705930306d7537cc0f7e53e63bc19
LIBRETRO_PCSX_SITE = $(call github,libretro,pcsx_rearmed,$(LIBRETRO_PCSX_VERSION))

define LIBRETRO_PCSX_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" $(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" AR="$(TARGET_AR)" -C $(@D) -f Makefile.libretro platform="$(LIBRETRO_BOARD)"
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
