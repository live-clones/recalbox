################################################################################
#
# GLUPEN64
#
################################################################################
LIBRETRO_GLUPEN64_VERSION = 407bcd40b3a42bff6b856a6d6f88a7d5d670bf9e
LIBRETRO_GLUPEN64_SITE = $(call github,libretro,mupen64plus-libretro,$(LIBRETRO_GLUPEN64_VERSION))

ifeq ($(BR2_PACKAGE_RPI_USERLAND),y)
	LIBRETRO_GLUPEN64_DEPENDENCIES += rpi-userland
endif

SUPP_OPT=

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3),y)
	LIBRETRO_GLUPEN64_PLATFORM=rpi3
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI2),y)
	LIBRETRO_GLUPEN64_PLATFORM=rpi2
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_XU4),y)
	LIBRETRO_GLUPEN64_PLATFORM=odroid-ODROID-XU3
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_C2),y)
	SUPP_OPT=FORCE_GLES=1 ARCH=aarch64
	LIBRETRO_GLUPEN64_PLATFORM=linux
else ifeq ($(BR2_x86_i586),y)
	SUPP_OPT=ARCH=i586
	LIBRETRO_GLUPEN64_PLATFORM=linux
else
	LIBRETRO_GLUPEN64_PLATFORM=$(LIBRETRO_PLATFORM)
endif

define LIBRETRO_GLUPEN64_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" $(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(LIBRETRO_GLUPEN64_PLATFORM)" $(SUPP_OPT)
endef

define LIBRETRO_GLUPEN64_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mupen64plus_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/glupen64_libretro.so
endef

define GLUPEN64_CROSS_FIXUP
	$(SED) 's|/opt/vc/include|$(STAGING_DIR)/usr/include|g' $(@D)/Makefile
	$(SED) 's|/opt/vc/lib|$(STAGING_DIR)/usr/lib|g' $(@D)/Makefile
endef

GLUPEN64_PRE_CONFIGURE_HOOKS += GLUPEN64_FIXUP

$(eval $(generic-package))
