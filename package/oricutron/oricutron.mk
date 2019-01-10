################################################################################
#
# Oric/Atmos Emulation
#
################################################################################
ORICUTRON_VERSION = a79eec0d49836f24e78575bf05bc706d87ed27dd
ORICUTRON_SITE = $(call github,pete-gordon,oricutron,$(ORICUTRON_VERSION))
ORICUTRON_DEPENDENCIES = sdl2

define ORICUTRON_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" $(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile SDL_LIB=sdl2 PLATFORM="rpi"
endef

define ORICUTRON_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/oricutron \
		$(TARGET_DIR)/usr/bin/oricutron/oricutron
	cp -R $(@D)/roms $(TARGET_DIR)/usr/bin/oricutron/
	cp -R $(@D)/images $(TARGET_DIR)/usr/bin/oricutron/
endef

define ORICUTRON_POST_EXTRACT_FIX_SDL2_PATH
    @echo CHANGING MAKEFILE
    /bin/sed -i -E -e "s|\\$$\(shell PKG_CONFIG_PATH=/usr/\\$$\(BASELIBDIR\)/pkgconfig pkg-config \\$$\(SDL_LIB\) --cflags\)|`$(STAGING_DIR)/usr/bin/sdl2-config --cflags`|g" $(@D)/Makefile
    /bin/sed -i -E -e "s|-L/usr/\\$$\(BASELIBDIR\)\s\\$$\(shell PKG_CONFIG_PATH=/usr/\\$$\(BASELIBDIR\)/pkgconfig pkg-config \\$$\(SDL_LIB\) --libs\)|`$(STAGING_DIR)/usr/bin/sdl2-config --libs`|g" $(@D)/Makefile
endef
 
ORICUTRON_POST_EXTRACT_HOOKS += ORICUTRON_POST_EXTRACT_FIX_SDL2_PATH

$(eval $(autotools-package))


