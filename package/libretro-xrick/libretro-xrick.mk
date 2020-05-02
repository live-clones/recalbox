################################################################################
#
# XRICK
#
################################################################################

LIBRETRO_XRICK_VERSION = f5d7b06a54060ec8a867f666b8e7b379629a7512
LIBRETRO_XRICK_SITE = $(call github,libretro,xrick-libretro,$(LIBRETRO_XRICK_VERSION))

define LIBRETRO_XRICK_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile.libretro
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile.libretro platform="unix"
endef

define LIBRETRO_XRICK_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/xrick_libretro.so \
		$(TARGET_DIR)/usr/lib/libretro/xrick_libretro.so
	mkdir -p "$(TARGET_DIR)/recalbox/share_init/roms/ports/Rick Dangerous"
	$(INSTALL) -D $(@D)/data.zip "$(TARGET_DIR)/recalbox/share_init/roms/ports/Rick Dangerous/Rick Dangerous.zip"
endef

$(eval $(generic-package))
