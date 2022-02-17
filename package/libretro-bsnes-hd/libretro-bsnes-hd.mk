################################################################################
#
# BSNES_HD
#
################################################################################

LIBRETRO_BSNES_HD_VERSION = 63f111932e51288a89772a25f4840931679e95e7
LIBRETRO_BSNES_HD_SITE = $(call gitlab,recalbox/packages/libretro,bsnes-hd,$(LIBRETRO_BSNES_HD_VERSION))
LIBRETRO_BSNES_HD_LICENSE = COPYRIGHT
LIBRETRO_BSNES_HD_LICENSE_FILES = LICENSE
LIBRETRO_BSNES_HD_NON_COMMERCIAL = y

define LIBRETRO_BSNES_HD_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D) platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_BSNES_HD_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/bsnes_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/bsneshd_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_upgrade/bios/sgb
endef

$(eval $(generic-package))
