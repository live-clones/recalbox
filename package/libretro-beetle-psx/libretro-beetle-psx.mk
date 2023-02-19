################################################################################
#
# BEETLE_PSX
#
################################################################################

# Version of 24/10/2021
LIBRETRO_BEETLE_PSX_VERSION = 92b93bfe110f4033bb4f90eac9b0072a15c3cda4
LIBRETRO_BEETLE_PSX_SITE = $(call github,libretro,beetle-psx-libretro,$(LIBRETRO_BEETLE_PSX_VERSION))
LIBRETRO_BEETLE_PSX_LICENSE = GPL-2.0
LIBRETRO_BEETLE_PSX_LICENSE_FILES = COPYING

define LIBRETRO_BEETLE_PSX_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(RETROARCH_LIBRETRO_PLATFORM)"
endef

define LIBRETRO_BEETLE_PSX_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mednafen_psx_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mednafen_psx_libretro.so
	mkdir -p $(TARGET_DIR)/recalbox/share_upgrade/bios/psx
endef

define LIBRETRO_BEETLE_PSX_CROSS_FIXUP
	$(SED) 's|-I/usr/local/include|-I$(STAGING_DIR)/usr/include|g' $(@D)/Makefile.common
endef

LIBRETRO_BEETLE_PSX_PRE_CONFIGURE_HOOKS += LIBRETRO_BEETLE_PSX_CROSS_FIXUP

$(eval $(generic-package))
