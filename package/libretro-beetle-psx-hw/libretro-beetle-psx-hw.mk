################################################################################
#
# BEETLE_PSX_HW - Enhanced OpenGL Version with OpenGL renderer
#
################################################################################

LIBRETRO_BEETLE_PSX_HW_VERSION = 9f126e5752c018fc1c1c6f322ad6b3a2fca93c2d
LIBRETRO_BEETLE_PSX_HW_SITE = $(call github,libretro,beetle-psx-libretro,$(LIBRETRO_BEETLE_PSX_HW_VERSION))
LIBRETRO_BEETLE_PSX_HW_LICENSE = GPL-2.0
LIBRETRO_BEETLE_PSX_HW_LICENSE_FILES = COPYING

define LIBRETRO_BEETLE_PSX_HW_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(RETROARCH_LIBRETRO_PLATFORM)" HAVE_HW=1
endef

define LIBRETRO_BEETLE_PSX_HW_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/mednafen_psx_hw_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/mednafen_psx_hw_libretro.so
endef

define LIBRETRO_BEETLE_PSX_HW_CROSS_FIXUP
	$(SED) 's|-L/usr/local/lib|-L$(STAGING_DIR)/usr/lib|g' $(@D)/Makefile
	$(SED) 's|-I/usr/local/include|-I$(STAGING_DIR)/usr/include|g' $(@D)/Makefile.common
	$(SED) 's|FIRST_RENDERER EXT_RENDERER|EXT_RENDERER FIRST_RENDERER|g' $(@D)/libretro.cpp
	$(SED) 's`EXT_RENDERER "|opengl|software"`EXT_RENDERER "opengl|software|"`g' $(@D)/libretro.cpp
	$(SED) 's`EXT_RENDERER "|software"`EXT_RENDERER "software|"`g' $(@D)/libretro.cpp
endef

LIBRETRO_BEETLE_PSX_HW_PRE_CONFIGURE_HOOKS += LIBRETRO_BEETLE_PSX_HW_CROSS_FIXUP

$(eval $(generic-package))
