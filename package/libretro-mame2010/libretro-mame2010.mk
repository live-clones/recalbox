################################################################################
#
# MAME2010
#
################################################################################
LIBRETRO_MAME2010_VERSION = 917453b5547f921ea7eabe2c6c01421fc24fcbc2
LIBRETRO_MAME2010_SITE = $(call github,libretro,mame2010-libretro,$(LIBRETRO_MAME2010_VERSION))
LIBRETRO_MAME2010_LICENSE = MAME
LIBRETRO_MAME2010_DEPENDENCIES = zlib


LIBRETRO_MAME2010_BUILD_OPTS=

ifeq ($(BR2_arm),y)
	LIBRETRO_MAME2010_BUILD_OPTS=VRENDER=soft PTR64=0 ARM_ENABLED=1
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_C2),y)
	LIBRETRO_MAME2010_BUILD_OPTS=VRENDER=soft PTR64=1 ARM_ENABLED=1
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_X86_64),y)
	LIBRETRO_MAME2010_BUILD_OPTS=VRENDER=soft PTR64=1 ARM_ENABLED=0
else
	LIBRETRO_MAME2010_BUILD_OPTS=VRENDER=soft PTR64=0 ARM_ENABLED=0
endif

define LIBRETRO_MAME2010_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" $(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_CXX)" -C $(@D)/ -f Makefile platform="$(LIBRETRO_PLATFORM)" $(LIBRETRO_MAME2010_BUILD_OPTS)
endef

define LIBRETRO_MAME2010_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mame2010_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mame2010_libretro.so
endef

$(eval $(generic-package))
