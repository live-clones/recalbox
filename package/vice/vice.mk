################################################################################
#
# Vice Emulation
#
################################################################################

VICE_DEPENDENCIES = sdl2

ifeq ($(BR2_PACKAGE_RECALBOX_TARGETGROUP_ROCKCHIP),y)
# New 3.3 version would not compile correctly, couldn't detect SDL was working, then random compile errors, etc... maybe come back to it later
VICE_VERSION = 2.4.24
VICE_SOURCE = vice-$(VICE_VERSION).tar.gz
VICE_SITE = https://sourceforge.net/projects/vice-emu/files/development-releases
VICE_CONF_OPTS += \
   --disable-option-checking \
   --enable-sdlui \
   --enable-native-tools=gcc \
   CFLAGS="-march=armv7-a -mfpu=neon-vfpv4 -mfloat-abi=hard -O2"
else
VICE_VERSION = 3.3
VICE_SITE = https://sourceforge.net/projects/vice-emu/files/releases
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
endif

$(eval $(autotools-package))
