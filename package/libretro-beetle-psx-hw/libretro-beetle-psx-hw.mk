################################################################################
#
# BEETLE_PSX_HW - Enhanced OpenGL Version with OpenGL renderer
#
################################################################################
LIBRETRO_BEETLE_PSX_HW_VERSION = 46df877d5df91a7f5d97093cdce89766d4fd30b7
LIBRETRO_BEETLE_PSX_HW_SITE = $(call github,libretro,beetle-psx-libretro,$(LIBRETRO_BEETLE_PSX_HW_VERSION))


define LIBRETRO_BEETLE_PSX_HW_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" $(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile platform="$(LIBRETRO_PLATFORM)" HAVE_HW=1
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
