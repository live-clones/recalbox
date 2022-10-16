################################################################################
#
# FROTZ_RECALBOX
#
################################################################################

FROTZ_RECALBOX_VERSION = 2.54
FROTZ_RECALBOX_SITE = $(call gitlab,DavidGriffith,frotz,$(FROTZ_RECALBOX_VERSION))
FROTZ_RECALBOX_DEPENDENCIES = host-pkgconf libpng libjpeg sdl2 freetype zlib libao
FROTZ_RECALBOX_LICENSE = GPL-2.0+
FROTZ_RECALBOX_LICENSE_FILES = COPYING

ifeq ($(BR2_PACKAGE_HAS_LIBGL),y)
FROTZ_RECALBOX_DEPENDENCIES += libgl
endif

ifeq ($(BR2_PACKAGE_HAS_LIBGLES),y)
FROTZ_RECALBOX_DEPENDENCIES += libgles
endif

define FROTZ_RECALBOX_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_NOLTO) -std=c99" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_NOLTO)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_NOLTO)" \
		PKG_CONFIG="$(HOST_DIR)/bin/pkg-config"
		PKG_CONFIG_PATH="$(STAGING_DIR)/usr/lib/pkgconfig/" \
		SDL2_CONFIG="$(STAGING_DIR)/usr/bin/sdl2-config" \
		CROSS_DIR="$(STAGING_DIR)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" RANLIB="$(TARGET_RANLIB)" -C $(@D)/ \
		PREFIX=/usr CONFIG_DIR=/etc \
		sdl
endef

define FROTZ_RECALBOX_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/sfrotz $(TARGET_DIR)/usr/bin/sfrotz
endef

#define FROTZ_RECALBOX_POST_EXTRACT_FIX_SDL2_PATH
#	@echo CHANGING MAKEFILE
#	sed -i -E -e "s|SDL_LDFLAGS\s+=.*||g" $(@D)/Makefile
#	#sed -i -E -e "s|-L/usr/\\$$\(BASELIBDIR\)\s\\$$\(shell PKG_CONFIG_PATH=/usr/\\$$\(BASELIBDIR\)/pkgconfig pkg-config \\$$\(SDL_LIB\) --libs\)|`$(STAGING_DIR)/usr/bin/sdl2-config --libs`|g" $(@D)/Makefile
#endef

#FROTZ_RECALBOX_POST_EXTRACT_HOOKS += FROTZ_RECALBOX_POST_EXTRACT_FIX_SDL2_PATH

$(eval $(generic-package))

