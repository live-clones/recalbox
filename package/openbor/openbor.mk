################################################################################
#
# OPENBOR
#
################################################################################

OPENBOR_VERSION = 668aec85293a024e1421def947494c8a7e5bdcc3
OPENBOR_SITE = https://gitlab.com/Bkg2k/openbor
OPENBOR_SITE_METHOD = git
OPENBOR_DEPENDENCIES = sdl2 sdl2_gfx sdl2_mixer libpng libvpx
OPENBOR_LICENSE = Custom
OPENBOR_LICENSE_FILES = LICENSE

ifeq ($(BR2_arm),y)
OPENBOR_BUILD_TARGET=BUILD_PANDORA
endif

ifeq ($(BR2_aarch64),y)
OPENBOR_BUILD_TARGET=BUILD_PANDORA64
endif

ifeq ($(BR2_i386),y)
OPENBOR_PLATFORM_OPTS=-m32
OPENBOR_BUILD_TARGET=BUILD_LINUX
OPENBOR_DEPENDENCIES += host-yasm
endif

ifeq ($(BR2_x86_64),y)
OPENBOR_PLATFORM_OPTS=-m64
OPENBOR_BUILD_TARGET=BUILD_LINUX
OPENBOR_BUILD_TARGET64=LINUX64=1
OPENBOR_DEPENDENCIES += host-yasm
endif

define OPENBOR_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/engine/Makefile
	(cd $(@D)/engine && ./version.sh)
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO) $(OPENBOR_PLATFORM_OPTS)" \
		CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO) $(OPENBOR_PLATFORM_OPTS)" \
		LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)" \
		$(MAKE) RECALBOXSTRIP="$(TARGET_STRIP)" YASM="$(HOST_DIR)/bin/yasm" CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D)/engine -f Makefile $(OPENBOR_BUILD_TARGET)=1 $(OPENBOR_BUILD_TARGET64) TARGET_GCC=$(TARGET_CXX) PNDDEV=$(STAGING_DIR)
endef

define OPENBOR_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/engine/OpenBOR \
		$(TARGET_DIR)/usr/bin/OpenBOR
endef

define OPENBOR_POST_EXTRACT_FIX_SDL2_PATH
	# SDL2 Path
	/bin/sed -i -E -e "s|\`pkg-config sdl2 --cflags\`|`$(STAGING_DIR)/usr/bin/sdl2-config --cflags`|g" $(@D)/engine/Makefile
	# Replace strip
	#/bin/sed -i -E -e "s|\\$$\(PNDDEV\)/bin/arm-none-linux-gnueabi-strip|$(TARGET_STRIP)|g" $(@D)/engine/Makefile
	# Replace all "Saves" in a row
	/bin/sed -i -E -e "s|\"Saves\"|\"saves/openbor\"|g" $(@D)/engine/openbor.c
	/bin/sed -i -E -e "s|\"Saves\"|\"saves/openbor\"|g" $(@D)/engine/openborscript.c
endef

OPENBOR_POST_EXTRACT_HOOKS += OPENBOR_POST_EXTRACT_FIX_SDL2_PATH

ifeq ($(BR2_PACKAGE_HAS_LIBGL),y)
OPENBOR_DEPENDENCIES += libgl
endif

ifeq ($(BR2_PACKAGE_HAS_LIBGLES),y)
OPENBOR_DEPENDENCIES += libgles
endif

$(eval $(generic-package))
