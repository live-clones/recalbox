################################################################################
#
# MESEN
#
################################################################################

LIBRETRO_MESEN_VERSION = 3154f19467aa606493591687070e1bdd6fd867ab
LIBRETRO_MESEN_SITE = $(call github,SourMesen,Mesen,$(LIBRETRO_MESEN_VERSION))
LIBRETRO_MESEN_LICENSE = GPL-3.0
LIBRETRO_MESEN_LICENSE_FILES = LICENSE

define LIBRETRO_MESEN_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Libretro/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/Libretro -f Makefile platform="unix"
endef

define LIBRETRO_MESEN_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/Libretro/mesen_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mesen_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_upgrade/bios/HdPacks
endef

$(eval $(generic-package))
