################################################################################
#
# MUPEN64PLUS NX
#
################################################################################

LIBRETRO_MUPEN64_NX_VERSION = 374f8bb31ce8e54c815cf5b38dc645ff6d8637cb
LIBRETRO_MUPEN64_NX_SITE = $(call github,libretro,mupen64plus-libretro-nx,$(LIBRETRO_MUPEN64_NX_VERSION))
LIBRETRO_MUPEN64_NX_DEPENDENCIES = rpi-userland

ifeq ($(BR2_cortex_a7),y)
LIBRETRO_MUPEN64_NX_PLATFORM=rpi2
else
LIBRETRO_MUPEN64_NX_PLATFORM=rpi3
endif

define LIBRETRO_MUPEN64_NX_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO) -fpermissive" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(LIBRETRO_MUPEN64_NX_PLATFORM)" WITH_DYNAREC=arm
endef

define LIBRETRO_MUPEN64_NX_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mupen64plus_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mupen64plusnx_libretro.so
endef

$(eval $(generic-package))
