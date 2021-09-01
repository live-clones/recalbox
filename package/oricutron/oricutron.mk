################################################################################
#
# Oric/Atmos Emulation
#
################################################################################

ORICUTRON_VERSION = cb57f0f2dc5487bc7cf179ed1dd3e228ac5ce8f9
ORICUTRON_SITE = $(call github,pete-gordon,oricutron,$(ORICUTRON_VERSION))
ORICUTRON_DEPENDENCIES = sdl2
ORICUTRON_LICENSE = GPL-2.0

ORICUTRON_SUPP_OPT=

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI4),y)
ORICUTRON_PLATFORM=rpi
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3),y)
ORICUTRON_PLATFORM=rpi
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI2),y)
ORICUTRON_PLATFORM=rpi
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI1),y)
ORICUTRON_PLATFORM=rpi
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_ODROIDXU4),y)
ORICUTRON_PLATFORM=rpi
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_ODROIDGO2),y)
ORICUTRON_PLATFORM=rpi
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_C2),y)
ORICUTRON_PLATFORM=rpi
else ifeq ($(BR2_i386),y)
ORICUTRON_PLATFORM=linux
ORICUTRON_POST_EXTRACT_CUSTOM_FIX=$(SED) "s|-m64|-m32|g" $(@D)/Makefile
else ifeq ($(BR2_x86_64),y)
ORICUTRON_PLATFORM=linux
else
ORICUTRON_PLATFORM=$(RETROARCH_LIBRETRO_PLATFORM)
endif

define ORICUTRON_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
		LFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" \
			-C $(@D)/ -f Makefile SDL_LIB=sdl2 PLATFORM=$(ORICUTRON_PLATFORM) NOGTK=y
endef

define ORICUTRON_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/oricutron \
		$(TARGET_DIR)/usr/bin/oricutron/oricutron
	# Copy emulator resources
	cp -R $(@D)/images $(TARGET_DIR)/usr/bin/oricutron/
	# Copy rom patch (required to enable turbo-tapes)
	mkdir -p $(TARGET_DIR)/recalbox/share_upgrade/bios/oricutron
	cp $(@D)/roms/*.pch $(TARGET_DIR)/recalbox/share_upgrade/bios/oricutron/
	# Copy rom symbols (for who wants to play with Oric/ATmos debugger!)
	cp $(@D)/roms/*.pch $(TARGET_DIR)/recalbox/share_upgrade/bios/oricutron/
	cp $(@D)/roms/*.rom $(TARGET_DIR)/recalbox/share_upgrade/bios/oricutron/
endef

define ORICUTRON_POST_EXTRACT_FIX_SDL2_PATH
	@echo CHANGING MAKEFILE
	sed -i -E -e "s|\\$$\(shell PKG_CONFIG_PATH=/usr/\\$$\(BASELIBDIR\)/pkgconfig pkg-config \\$$\(SDL_LIB\) --cflags\)|`$(STAGING_DIR)/usr/bin/sdl2-config --cflags`|g" $(@D)/Makefile
	sed -i -E -e "s|-L/usr/\\$$\(BASELIBDIR\)\s\\$$\(shell PKG_CONFIG_PATH=/usr/\\$$\(BASELIBDIR\)/pkgconfig pkg-config \\$$\(SDL_LIB\) --libs\)|`$(STAGING_DIR)/usr/bin/sdl2-config --libs`|g" $(@D)/Makefile
	$(ORICUTRON_POST_EXTRACT_CUSTOM_FIX)
endef

ORICUTRON_POST_EXTRACT_HOOKS += ORICUTRON_POST_EXTRACT_FIX_SDL2_PATH

$(eval $(generic-package))
