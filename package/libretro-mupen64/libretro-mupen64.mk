################################################################################
#
# MUPEN64PLUS
#
################################################################################
LIBRETRO_MUPEN64PLUS_VERSION = 374f8bb31ce8e54c815cf5b38dc645ff6d8637cb
LIBRETRO_MUPEN64PLUS_SITE = $(call github,libretro,mupen64plus-libretro,$(LIBRETRO_MUPEN64PLUS_VERSION))
LIBRETRO_MUPEN64PLUS_DEPENDENCIES = rpi-userland

ifeq ($(BR2_cortex_a7),y)
	LIBRETRO_MUPEN64PLUS_PLATFORM=rpi2
else
	LIBRETRO_MUPEN64PLUS_PLATFORM=rpi
endif

define LIBRETRO_MUPEN64PLUS_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" $(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(LIBRETRO_MUPEN64PLUS_PLATFORM)" WITH_DYNAREC=arm
endef

define LIBRETRO_MUPEN64PLUS_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mupen64plus_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mupen64plus_libretro.so
endef

$(eval $(generic-package))
