################################################################################
#
# MUPEN64PLUS-NX
#
################################################################################

LIBRETRO_MUPEN64PLUS_NX_VERSION = 81a58df0263c90b10b7fc11b6deee04d47e3aa40
LIBRETRO_MUPEN64PLUS_NX_SITE = $(call github,libretro,mupen64plus-libretro-nx,$(LIBRETRO_MUPEN64PLUS_NX_VERSION))
LIBRETRO_MUPEN64PLUS_NX_LICENSE = GPLv3
LIBRETRO_MUPEN64PLUS_NX_DEPENDENCIES += host-nasm

ifeq ($(BR2_PACKAGE_RPI_USERLAND),y)
LIBRETRO_MUPEN64PLUS_NX_DEPENDENCIES += rpi-userland
endif

LIBRETRO_MUPEN64PLUS_NX_SUPP_OPT=

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI2),y)
LIBRETRO_MUPEN64PLUS_NX_PLATFORM=rpi2
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3),y)
LIBRETRO_MUPEN64PLUS_NX_PLATFORM=rpi3
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_XU4),y)
LIBRETRO_MUPEN64PLUS_NX_SUPP_OPT=BOARD=ODROID-XU
LIBRETRO_MUPEN64PLUS_NX_PLATFORM=odroid
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_C2),y)
LIBRETRO_MUPEN64PLUS_NX_SUPP_OPT=BOARD=ODROIDC
LIBRETRO_MUPEN64PLUS_NX_PLATFORM=odroid
else ifeq ($(BR2_i386),y)
LIBRETRO_MUPEN64PLUS_NX_PLATFORM=unix
else ifeq ($(BR2_x86_64),y)
LIBRETRO_MUPEN64PLUS_NX_PLATFORM=unix
else
LIBRETRO_MUPEN64PLUS_NX_PLATFORM=$(LIBRETRO_PLATFORM)
endif

define LIBRETRO_MUPEN64PLUS_NX_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		CPPFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO) \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" LD="$(TARGET_LD)" \
		AR="$(TARGET_AR)" RANLIB="$(TARGET_RANLIB)" -C $(@D)/ -f Makefile platform="$(LIBRETRO_MUPEN64PLUS_NX_PLATFORM)" $(LIBRETRO_MUPEN64PLUS_NX_SUPP_OPT)
endef

define LIBRETRO_MUPEN64PLUS_NX_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mupen64plus_next_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mupen64plus_next_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_init/bios/Mupen64plus/hires_texture
endef

$(eval $(generic-package))
