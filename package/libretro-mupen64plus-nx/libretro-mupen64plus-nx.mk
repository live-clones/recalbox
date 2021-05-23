################################################################################
#
# MUPEN64PLUS-NX
#
################################################################################

LIBRETRO_MUPEN64PLUS_NX_VERSION = 8fb474275aaca2e92e025c6a60af3e47cfdd185c
LIBRETRO_MUPEN64PLUS_NX_SITE = $(call github,libretro,mupen64plus-libretro-nx,$(LIBRETRO_MUPEN64PLUS_NX_VERSION))
LIBRETRO_MUPEN64PLUS_NX_LICENSE = GPLv3
LIBRETRO_MUPEN64PLUS_NX_DEPENDENCIES += host-nasm

ifeq ($(BR2_PACKAGE_HAS_LIBEGL),y)
LIBRETRO_MUPEN64PLUS_NX_DEPENDENCIES += libegl
endif

ifeq ($(BR2_PACKAGE_HAS_LIBGL),y)
LIBRETRO_MUPEN64PLUS_NX_DEPENDENCIES += libgl
endif

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI2),y)
LIBRETRO_MUPEN64PLUS_NX_PLATFORM=rpi2
LIBRETRO_MUPEN64PLUS_NX_DYNAREC=arm

else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3),y)
LIBRETRO_MUPEN64PLUS_NX_PLATFORM=rpi3
LIBRETRO_MUPEN64PLUS_NX_DYNAREC=arm

else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI4),y)
LIBRETRO_MUPEN64PLUS_NX_PLATFORM=rpi4
LIBRETRO_MUPEN64PLUS_NX_SUPP_OPT=FORCE_GLES3=1
LIBRETRO_MUPEN64PLUS_NX_DYNAREC=arm

else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_ODROIDXU4),y)
LIBRETRO_MUPEN64PLUS_NX_PLATFORM=odroid
LIBRETRO_MUPEN64PLUS_NX_SUPP_OPT=BOARD="ODROID-XU" FORCE_GLES=1
LIBRETRO_MUPEN64PLUS_NX_DYNAREC=arm

else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_ODROIDGO2),y)
LIBRETRO_MUPEN64PLUS_NX_PLATFORM=odroid64
LIBRETRO_MUPEN64PLUS_NX_SUPP_OPT=BOARD="ODROID-GO2"
LIBRETRO_MUPEN64PLUS_NX_DYNAREC=aarch64

else ifeq ($(BR2_i386),y)
LIBRETRO_MUPEN64PLUS_NX_PLATFORM=unix
LIBRETRO_MUPEN64PLUS_NX_SUPP_OPT=ARCH="i686" HAVE_PARALLEL_RSP=1 HAVE_THR_AL=1
LIBRETRO_MUPEN64PLUS_NX_DYNAREC=x86

else ifeq ($(BR2_x86_64),y)
LIBRETRO_MUPEN64PLUS_NX_PLATFORM=unix
LIBRETRO_MUPEN64PLUS_NX_SUPP_OPT=ARCH="x86_64" HAVE_PARALLEL_RSP=1 HAVE_THR_AL=1 LLE=1
LIBRETRO_MUPEN64PLUS_NX_DYNAREC=x86_64

else
LIBRETRO_MUPEN64PLUS_NX_PLATFORM=$(RETROARCH_LIBRETRO_BOARD)
LIBRETRO_MUPEN64PLUS_NX_SUPP_OPT=
endif

define LIBRETRO_MUPEN64PLUS_NX_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		CPPFLAGS="$(TARGET_CPPFLAGS) $(COMPILER_COMMONS_CPPFLAGS_SO)" \
		$(MAKE) NASM="$(HOST_DIR)/bin/nasm" CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" LD="$(TARGET_LD)" \
		AR="$(TARGET_AR)" RANLIB="$(TARGET_RANLIB)" -C $(@D)/ -f Makefile platform="$(LIBRETRO_MUPEN64PLUS_NX_PLATFORM)" $(LIBRETRO_MUPEN64PLUS_NX_SUPP_OPT) \
		WITH_DYNAREC=$(LIBRETRO_MUPEN64PLUS_NX_DYNAREC)
endef

define LIBRETRO_MUPEN64PLUS_NX_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mupen64plus_next_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mupen64plus_next_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_init/bios/Mupen64plus/hires_texture
	mkdir -p $(TARGET_DIR)/recalbox/share_init/bios/Mupen64plus/cache
endef

$(eval $(generic-package))
