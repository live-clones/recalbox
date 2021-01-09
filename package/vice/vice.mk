################################################################################
#
# Vice Emulation
#
################################################################################

VICE_VERSION = 3.4
VICE_SITE = https://sourceforge.net/projects/vice-emu/files/releases
VICE_LICENSE = GPL-2.0
VICE_LICENSE_FILES = COPYING

VICE_CONF_OPTS += \
	--disable-option-checking \
	--enable-sdlui2 \
	--enable-native-tools=gcc \
	--disable-sdltest \
	--with-sdl-prefix=$(STAGING_DIR)/usr \
	--sdl2-config=$(STAGING_DIR)/usr/bin/sdl2-config \
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_SO)" \
	CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_SO)" \
	LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_SO)"

VICE_DEPENDENCIES = sdl2

$(eval $(autotools-package))
