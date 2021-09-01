################################################################################
#
# FBNEO
#
################################################################################

# Version 1.0.0.02 (25/08/21)
LIBRETRO_FBNEO_VERSION = 8ffe13155d3bda53cfa2de32274a291ddd9c9c80
LIBRETRO_FBNEO_SITE = $(call github,libretro,FBNeo,$(LIBRETRO_FBNEO_VERSION))
LIBRETRO_FBNEO_LICENSE = COPYRIGHT
LIBRETRO_FBNEO_LICENSE_FILES = LICENSE.md
LIBRETRO_FBNEO_NON_COMMERCIAL = y

ifeq ($(BR2_ARM_CPU_HAS_NEON),y)
LIBRETRO_FBNEO_OPTIONS += "HAVE_NEON=1"
else
LIBRETRO_FBNEO_OPTIONS += "HAVE_NEON=0"
endif

ifeq ($(BR2_arm),y)
LIBRETRO_FBNEO_OPTIONS += USE_CYCLONE=1
endif

ifeq ($(BR2_x86_64),y)
LIBRETRO_FBNEO_OPTIONS += USE_X64_DRC=1
endif

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_ODROIDGO2),y)
LIBRETRO_FBNEO_PLATFORM = odroidgo2
else
LIBRETRO_FBNEO_PLATFORM = $(RETROARCH_LIBRETRO_BOARD)
endif

define LIBRETRO_FBNEO_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/src/burner/libretro -f Makefile platform="$(LIBRETRO_FBNEO_PLATFORM)" $(LIBRETRO_FBNEO_OPTIONS)
endef

define LIBRETRO_FBNEO_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/src/burner/libretro/fbneo_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/fbneo_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_upgrade/bios/fbneo/samples
	cp -R $(@D)/dats/* $(TARGET_DIR)/recalbox/share_upgrade/bios/fbneo
	cp -R $(@D)/metadata/* $(TARGET_DIR)/recalbox/share_upgrade/bios/fbneo
endef

$(eval $(generic-package))
