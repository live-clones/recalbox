################################################################################
#
# BK-0010/0011/Terak 8510a emulator
#
################################################################################

LIBRETRO_BK_EMULATOR_VERSION = 70f4a2d812379b8e3e3c7ed3025d08583dee98b1
LIBRETRO_BK_EMULATOR_SITE = $(call github,libretro,bk-emulator,$(LIBRETRO_BK_EMULATOR_VERSION))
LIBRETRO_BK_EMULATOR_LICENSE = Custom

define LIBRETRO_BK_EMULATOR_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CPPFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D) -f Makefile.libretro platform="unix"
endef

define LIBRETRO_BK_EMULATOR_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/bk_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/bk_libretro.so
endef

$(eval $(generic-package))
