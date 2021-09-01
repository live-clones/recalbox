################################################################################
#
# UAE4ARM
#
################################################################################

LIBRETRO_UAE4ARM_VERSION = a63d159ac1ad4cec6f178fb28e8f7413fd6c73e5
LIBRETRO_UAE4ARM_SITE = $(call github,Chips-fr,uae4arm-rpi,$(LIBRETRO_UAE4ARM_VERSION))
LIBRETRO_UAE4ARM_BRANCH = master
LIBRETRO_UAE4ARM_LICENSE = GPL-2.0
LIBRETRO_UAE4ARM_LICENSE_FILES = COPYING

ifeq ($(BR2_PACKAGE_RPI_USERLAND),y)
UAEPLATFORM=rpi
else
UAEPLATFORM=unix-neon
ifeq ($(BR2_aarch64),y)
UAEPLATFORM=unix-aarch64
endif
endif

LIBRETRO_UAE4ARM_COMPILER_COMMONS_CFLAGS = $(COMPILER_COMMONS_CFLAGS_SO)
LIBRETRO_UAE4ARM_COMPILER_COMMONS_CXXFLAGS = $(COMPILER_COMMONS_CXXFLAGS_SO)
LIBRETRO_UAE4ARM_COMPILER_COMMONS_LDFLAGS = $(COMPILER_COMMONS_LDFLAGS_SO)

define LIBRETRO_UAE4ARM_BUILD_CMDS
	    CFLAGS="$(TARGET_CFLAGS) $(LIBRETRO_UAE4ARM_COMPILER_COMMONS_CFLAGS)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" AS="$(TARGET_AS)" -C $(@D)/ -f Makefile.libretro platform="$(UAEPLATFORM)"
endef

define LIBRETRO_UAE4ARM_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/uae4arm_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/uae4arm_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_upgrade/bios/uae_data
endef

$(eval $(generic-package))
