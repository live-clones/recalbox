################################################################################
#
# Vice Emulation
#
################################################################################
VICE_VERSION = 3.0
VICE_SOURCE = vice-$(VICE_VERSION).tar.gz
VICE_SITE = https://sourceforge.net/projects/vice-emu/files/releases
VICE_INSTALL_TARGET = YES
VICE_INSTALL_STAGING = NO
VICE_DEPENDENCIES = sdl

VICE_CONF_OPTS += \
	--disable-option-checking \
	--enable-sdlui2 \
	--enable-native-tools=gcc \
	--without-pulse \
	--without-oss \
	--disable-realdevice \
	--enable-rs232 \
	--disable-midi \
	--with-uithreads \
	--with-sdlsound \
	--includedir="$(STAGING_DIR)/usr/include/SDL2" \
	CPPFLAGS="-I$(STAGING_DIR)/usr/include"

define VICE_MAKEFILE_PATH
	find $(@D) -name Makefile -exec $(SED) "s+-I/usr/include/SDL2+-I$(STAGING_DIR)/usr/include/SDL2+g" {} \; 
endef 

VICE_POST_CONFIGURE_HOOKS += VICE_MAKEFILE_PATH

$(eval $(autotools-package))


