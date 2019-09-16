################################################################################
#
# MUPEN64PLUS-NX
#
################################################################################

LIBRETRO_MUPEN64PLUS_NX_VERSION = 16d96924d8a31483bfd0b2d1bc39dc6e319d4352
LIBRETRO_MUPEN64PLUS_NX_SITE = $(call github,libretro,mupen64plus-libretro-nx,$(LIBRETRO_MUPEN64PLUS_NX_VERSION))
LIBRETRO_MUPEN64PLUS_NX_DEPENDENCIES = rpi-userland

ifeq ($(BR2_cortex_a7),y)
LIBRETRO_MUPEN64PLUS_NX_PLATFORM=rpi2
else
LIBRETRO_MUPEN64PLUS_NX_PLATFORM=rpi3
endif

define LIBRETRO_MUPEN64PLUS_NX_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO) -fpermissive" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(LIBRETRO_MUPEN64PLUS_NX_PLATFORM)" WITH_DYNAREC=arm
endef

define LIBRETRO_MUPEN64PLUS_NX_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mupen64plus_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mupen64plus_nx_libretro.so
endef

$(eval $(generic-package))
