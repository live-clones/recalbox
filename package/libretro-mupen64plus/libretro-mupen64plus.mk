################################################################################
#
# MUPEN64PLUS (Archived)
#
################################################################################

LIBRETRO_MUPEN64PLUS_VERSION = ab8134ac90a567581df6de4fc427dd67bfad1b17
LIBRETRO_MUPEN64PLUS_SITE = $(call github,libretro,mupen64plus-libretro,$(LIBRETRO_MUPEN64PLUS_VERSION))

ifeq ($(BR2_PACKAGE_RPI_USERLAND),y)
LIBRETRO_MUPEN64PLUS_DEPENDENCIES += rpi-userland
endif

ifeq ($(BR2_PACKAGE_MESA3D),y)
LIBRETRO_MUPEN64PLUS_DEPENDENCIES += mesa3d
endif

ifeq ($(BR2_i386)$(BR2_x86_64),y)
LIBRETRO_MUPEN64PLUS_DEPENDENCIES += host-nasm
endif

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3),y)
LIBRETRO_MUPEN64PLUS_SUPP_OPT=FORCE_GLES=1 ARCH=arm
LIBRETRO_MUPEN64PLUS_PLATFORM=rpi3
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI2),y)
LIBRETRO_MUPEN64PLUS_SUPP_OPT=FORCE_GLES=1 ARCH=arm
LIBRETRO_MUPEN64PLUS_PLATFORM=rpi2
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_XU4),y)
LIBRETRO_MUPEN64PLUS_SUPP_OPT=FORCE_GLES=1 ARCH=arm
LIBRETRO_MUPEN64PLUS_PLATFORM=odroid
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_C2),y)
LIBRETRO_MUPEN64PLUS_SUPP_OPT=FORCE_GLES=1 ARCH=aarch64
LIBRETRO_MUPEN64PLUS_PLATFORM=unix
else ifeq ($(BR2_i386),y)
LIBRETRO_MUPEN64PLUS_SUPP_OPT=ARCH=i686
LIBRETRO_MUPEN64PLUS_PLATFORM=unix
else ifeq ($(BR2_x86_64),y)
LIBRETRO_MUPEN64PLUS_SUPP_OPT=ARCH=x86_64
LIBRETRO_MUPEN64PLUS_PLATFORM=unix
else
LIBRETRO_MUPEN64PLUS_PLATFORM=$(RETROARCH_LIBRETRO_PLATFORM)
LIBRETRO_MUPEN64PLUS_SUPP_OPT=
endif

define LIBRETRO_MUPEN64PLUS_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" \
		$(MAKE) NASM="$(HOST_DIR)/bin/nasm" CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(LIBRETRO_MUPEN64PLUS_PLATFORM)" $(LIBRETRO_MUPEN64PLUS_SUPP_OPT)
endef

define LIBRETRO_MUPEN64PLUS_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mupen64plus_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mupen64plus_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_init/bios/Mupen64plus/hires_texture
	mkdir -p $(TARGET_DIR)/recalbox/share_init/bios/Mupen64plus/cache
endef

define MUPEN64PLUS_CROSS_FIXUP
	$(SED) 's|/opt/vc/include|$(STAGING_DIR)/usr/include|g' $(@D)/Makefile
	$(SED) 's|/opt/vc/lib|$(STAGING_DIR)/usr/lib|g' $(@D)/Makefile
endef

LIBRETRO_MUPEN64PLUS_PRE_CONFIGURE_HOOKS += MUPEN64PLUS_CROSS_FIXUP

$(eval $(generic-package))
