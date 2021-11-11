################################################################################
#
# GENESIS-PLUS-GX-WIDE
#
################################################################################

# Commit version of 2022/09/16
LIBRETRO_GENESISPLUSGXWIDE_VERSION = f634cc53fd8dd8c09a24aac5314d147635857e62
LIBRETRO_GENESISPLUSGXWIDE_SITE = $(call github,libretro,Genesis-Plus-GX-Wide,$(LIBRETRO_GENESISPLUSGXWIDE_VERSION))
LIBRETRO_GENESISPLUSGXWIDE_LICENSE = COPYRIGHT
LIBRETRO_GENESISPLUSGXWIDE_LICENSE_FILES = LICENSE.txt
LIBRETRO_GENESISPLUSGXWIDE_NON_COMMERCIAL = y

define LIBRETRO_GENESISPLUSGXWIDE_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile.libretro
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D) -f Makefile.libretro platform="$(RETROARCH_LIBRETRO_BOARD)"
endef

define LIBRETRO_GENESISPLUSGXWIDE_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/genesis_plus_gx_wide_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/genesisplusgxwide_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_upgrade/bios/gamegear
	mkdir -p $(TARGET_DIR)/recalbox/share_upgrade/bios/mastersystem
	mkdir -p $(TARGET_DIR)/recalbox/share_upgrade/bios/megadrive
	mkdir -p $(TARGET_DIR)/recalbox/share_upgrade/bios/segacd
endef

$(eval $(generic-package))
