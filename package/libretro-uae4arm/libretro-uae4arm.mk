################################################################################
#
# UAE4ARM
#
################################################################################

# commit of 25/09/2021
LIBRETRO_UAE4ARM_VERSION = 4d2e723667a84fcdca73102eb91a83b3c60edba3
LIBRETRO_UAE4ARM_SITE = $(call github,Chips-fr,uae4arm-rpi,$(LIBRETRO_UAE4ARM_VERSION))
LIBRETRO_UAE4ARM_BRANCH = master
LIBRETRO_UAE4ARM_LICENSE = GPL-2.0
LIBRETRO_UAE4ARM_LICENSE_FILES = COPYING
LIBRETRO_UAE4ARM_DEPENDENCIES = flac mpg123

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI4),y)
UAE4ARMPLATFORM = unix-neon
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3)$(BR2_PACKAGE_RECALBOX_TARGET_RPIZERO2LEGACY),y)
UAE4ARMPLATFORM = rpi3
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI2),y)
UAE4ARMPLATFORM = rpi2
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI1),y)
UAE4ARMPLATFORM = rpi
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_ODROIDXU4),y)
UAE4ARMPLATFORM = unix-neon
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_ODROIDGO2),y)
UAE4ARMPLATFORM = unix-aarch64
endif

LIBRETRO_UAE4ARM_COMPILER_COMMONS_CFLAGS = $(COMPILER_COMMONS_CFLAGS_NOLTO)
LIBRETRO_UAE4ARM_COMPILER_COMMONS_CXXFLAGS = $(COMPILER_COMMONS_CXXFLAGS_NOLTO)
LIBRETRO_UAE4ARM_COMPILER_COMMONS_LDFLAGS = $(COMPILER_COMMONS_LDFLAGS_NOLTO)

define LIBRETRO_UAE4ARM_BUILD_CMDS
	    CFLAGS="$(TARGET_CFLAGS) $(LIBRETRO_UAE4ARM_COMPILER_COMMONS_CFLAGS)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" AS="$(TARGET_AS)" -C $(@D)/ -f Makefile.libretro platform="$(UAE4ARMPLATFORM)"
endef

define LIBRETRO_UAE4ARM_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/uae4arm_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/uae4arm_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_upgrade/bios/uae_data
endef

$(eval $(generic-package))
