################################################################################
#
# NESTOPIA
#
################################################################################

LIBRETRO_NESTOPIA_VERSION = 357e1463a01fe2ca0dd91941aacaaa9944f95e4d
LIBRETRO_NESTOPIA_SITE = $(call github,libretro,nestopia,$(LIBRETRO_NESTOPIA_VERSION))
LIBRETRO_NESTOPIA_LICENSE = GPL-2.0
LIBRETRO_NESTOPIA_LICENSE_FILES = COPYING

define LIBRETRO_NESTOPIA_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/libretro/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/libretro/ platform="$(RETROARCH_LIBRETRO_BOARD)"
endef

define LIBRETRO_NESTOPIA_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/libretro/nestopia_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/nestopia_libretro.so
	# NstDatabase needed for proper emulation (backed into the core)
	# cp $(@D)/NstDatabase.xml $(TARGET_DIR)/recalbox/share_upgrade/bios
endef

$(eval $(generic-package))
