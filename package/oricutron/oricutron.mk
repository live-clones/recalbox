################################################################################
#
# Oric/Atmos Emulation
#
################################################################################
ORICUTRON_VERSION = a79eec0d49836f24e78575bf05bc706d87ed27dd
ORICUTRON_SITE = $(call github,pete-gordon,oricutron,$(ORICUTRON_VERSION))
ORICUTRON_DEPENDENCIES = sdl2

ORICUTRON_SUPP_OPT=

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3),y)
	ORICUTRON_PLATFORM=rpi
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI2),y)
	ORICUTRON_PLATFORM=rpi
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI1),y)
	ORICUTRON_PLATFORM=rpi
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_XU4),y)
	ORICUTRON_PLATFORM=rpi
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_C2),y)
	ORICUTRON_PLATFORM=rpi
else ifeq ($(BR2_i386),y)
	ORICUTRON_PLATFORM=linux
	ORICUTRON_POST_EXTRACT_CUSTOM_FIX=$(SED) "s|-m64|-m32|g" $(@D)/Makefile
else ifeq ($(BR2_x86_64),y)
	ORICUTRON_PLATFORM=linux
else
	ORICUTRON_PLATFORM=$(LIBRETRO_PLATFORM)
endif

define ORICUTRON_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" $(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/ -f Makefile SDL_LIB=sdl2 PLATFORM=$(ORICUTRON_PLATFORM) NOGTK=y
endef

define ORICUTRON_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/oricutron \
		$(TARGET_DIR)/usr/bin/oricutron/oricutron
	# Copy emulator resources
	cp -R $(@D)/images $(TARGET_DIR)/usr/bin/oricutron/
	# Copy rom patch (required to enable turbo-tapes)
	mkdir -p $(TARGET_DIR)/recalbox/share_init/bios/oricutron
	cp $(@D)/roms/*.pch $(TARGET_DIR)/recalbox/share_init/bios/oricutron/
    # Copy rom symbols (for who wants to play with Oric/ATmos debugger!)
	cp $(@D)/roms/*.pch $(TARGET_DIR)/recalbox/share_init/bios/oricutron/
endef

define ORICUTRON_POST_EXTRACT_FIX_SDL2_PATH
    @echo CHANGING MAKEFILE
    /bin/sed -i -E -e "s|\\$$\(shell PKG_CONFIG_PATH=/usr/\\$$\(BASELIBDIR\)/pkgconfig pkg-config \\$$\(SDL_LIB\) --cflags\)|`$(STAGING_DIR)/usr/bin/sdl2-config --cflags`|g" $(@D)/Makefile
    /bin/sed -i -E -e "s|-L/usr/\\$$\(BASELIBDIR\)\s\\$$\(shell PKG_CONFIG_PATH=/usr/\\$$\(BASELIBDIR\)/pkgconfig pkg-config \\$$\(SDL_LIB\) --libs\)|`$(STAGING_DIR)/usr/bin/sdl2-config --libs`|g" $(@D)/Makefile
    $(ORICUTRON_POST_EXTRACT_CUSTOM_FIX)
endef
 
ORICUTRON_POST_EXTRACT_HOOKS += ORICUTRON_POST_EXTRACT_FIX_SDL2_PATH

$(eval $(autotools-package))


