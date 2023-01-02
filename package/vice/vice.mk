################################################################################
#
# Vice Emulation
#
################################################################################

VICE_VERSION = 3.6.1
VICE_SITE = https://sourceforge.net/projects/vice-emu/files/releases
VICE_LICENSE = GPL-2.0
VICE_LICENSE_FILES = COPYING

VICE_DEPENDENCIES = sdl2 ffmpeg libpng giflib zlib lame pulseaudio jpeg sdl2_image host-xa65 host-dos2unix

VICE_CONF_OPTS += \
	--disable-option-checking \
	--disable-sdltest \
	--disable-pdf-docs \
	--disable-html-docs \
	--disable-platformdox \
	--disable-debug-gtk3ui \
	--disable-native-gtk3ui \
	--enable-sdlui2 \
	--enable-arch=sdl \
	--enable-external-ffmpeg \
	--enable-midi \
	--enable-lame \
	--with-alsa \
	--with-zlib \
	--with-jpeg \
	--with-png \
	--with-pulse \
	--enable-x64 \
	--bindir=/usr/bin/vice \
	CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_NOLTO)" \
	CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_NOLTO)" \
	LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_NOLTO)"

$(eval $(autotools-package))
