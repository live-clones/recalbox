################################################################################
#
# recalbox-mplayer
#
################################################################################

RECALBOX_MPLAYER_VERSION = 1.4
RECALBOX_MPLAYER_SOURCE = MPlayer-$(RECALBOX_MPLAYER_VERSION).tar.xz
RECALBOX_MPLAYER_SITE = http://www.mplayerhq.hu/MPlayer/releases
RECALBOX_MPLAYER_DEPENDENCIES = host-pkgconf
RECALBOX_MPLAYER_LICENSE = GPL-2.0
RECALBOX_MPLAYER_LICENSE_FILES = LICENSE Copyright
RECALBOX_MPLAYER_CFLAGS = $(TARGET_CFLAGS)
RECALBOX_MPLAYER_LDFLAGS = $(TARGET_LDFLAGS)

ifeq ($(BR2_PACKAGE_ALSA_LIB)$(BR2_PACKAGE_ALSA_LIB_MIXER)$(BR2_PACKAGE_ALSA_LIB_PCM),yyy)
RECALBOX_MPLAYER_DEPENDENCIES += alsa-lib
RECALBOX_MPLAYER_CONF_OPTS += --enable-alsa
else
RECALBOX_MPLAYER_CONF_OPTS += --disable-alsa
endif

ifeq ($(BR2_ENDIAN),"BIG")
RECALBOX_MPLAYER_CONF_OPTS += --enable-big-endian
else
RECALBOX_MPLAYER_CONF_OPTS += --disable-big-endian
endif

ifeq ($(BR2_PACKAGE_FREETYPE),y)
RECALBOX_MPLAYER_CONF_OPTS += \
	--enable-freetype \
	--with-freetype-config=$(STAGING_DIR)/usr/bin/freetype-config
RECALBOX_MPLAYER_DEPENDENCIES += freetype
else
RECALBOX_MPLAYER_CONF_OPTS += --disable-freetype
endif

ifeq ($(BR2_PACKAGE_FONTCONFIG),y)
RECALBOX_MPLAYER_DEPENDENCIES += fontconfig
else
RECALBOX_MPLAYER_CONF_OPTS += --disable-fontconfig
endif

ifeq ($(BR2_PACKAGE_LIBFRIBIDI),y)
RECALBOX_MPLAYER_DEPENDENCIES += libfribidi
else
RECALBOX_MPLAYER_CONF_OPTS += --disable-fribidi
endif

ifeq ($(BR2_PACKAGE_LIBICONV),y)
RECALBOX_MPLAYER_DEPENDENCIES += libiconv
else
RECALBOX_MPLAYER_CONF_OPTS += --disable-iconv
endif

ifeq ($(BR2_PACKAGE_NCURSES),y)
RECALBOX_MPLAYER_DEPENDENCIES += ncurses
else
RECALBOX_MPLAYER_CONF_OPTS += --disable-termcap
endif

ifeq ($(BR2_PACKAGE_FAAD2),y)
RECALBOX_MPLAYER_DEPENDENCIES += faad2
RECALBOX_MPLAYER_CONF_OPTS += --enable-faad
else
RECALBOX_MPLAYER_CONF_OPTS += --disable-faad
endif

ifeq ($(BR2_PACKAGE_LAME),y)
RECALBOX_MPLAYER_DEPENDENCIES += lame
RECALBOX_MPLAYER_CONF_OPTS += --enable-mp3lame
else
RECALBOX_MPLAYER_CONF_OPTS += --disable-mp3lame
endif

ifeq ($(BR2_PACKAGE_LIBASS),y)
RECALBOX_MPLAYER_DEPENDENCIES += libass
endif

ifeq ($(BR2_PACKAGE_LIBMPEG2),y)
RECALBOX_MPLAYER_DEPENDENCIES += libmpeg2
RECALBOX_MPLAYER_CONF_OPTS += --disable-libmpeg2-internal
endif

ifeq ($(BR2_PACKAGE_MPG123),y)
RECALBOX_MPLAYER_DEPENDENCIES += mpg123
else
RECALBOX_MPLAYER_CONF_OPTS += --disable-mpg123
endif

ifeq ($(BR2_PACKAGE_TREMOR),y)
RECALBOX_MPLAYER_DEPENDENCIES += tremor
RECALBOX_MPLAYER_CONF_OPTS += --enable-tremor
endif

ifeq ($(BR2_PACKAGE_LIBVORBIS),y)
RECALBOX_MPLAYER_DEPENDENCIES += libvorbis
endif

ifeq ($(BR2_PACKAGE_LIBMAD),y)
RECALBOX_MPLAYER_DEPENDENCIES += libmad
RECALBOX_MPLAYER_CONF_OPTS += --enable-mad
else
RECALBOX_MPLAYER_CONF_OPTS += --disable-mad
endif

ifeq ($(BR2_PACKAGE_LZO),y)
RECALBOX_MPLAYER_DEPENDENCIES += lzo
RECALBOX_MPLAYER_CONF_OPTS += --enable-liblzo
else
RECALBOX_MPLAYER_CONF_OPTS += --disable-liblzo
endif

RECALBOX_MPLAYER_DEPENDENCIES += $(if $(BR2_PACKAGE_BZIP2),bzip2)
RECALBOX_MPLAYER_DEPENDENCIES += $(if $(BR2_PACKAGE_LIBTHEORA),libtheora)
RECALBOX_MPLAYER_DEPENDENCIES += $(if $(BR2_PACKAGE_LIBPNG),libpng)
RECALBOX_MPLAYER_DEPENDENCIES += $(if $(BR2_PACKAGE_LIBVPX),libvpx)
RECALBOX_MPLAYER_DEPENDENCIES += $(if $(BR2_PACKAGE_JPEG),jpeg)
RECALBOX_MPLAYER_DEPENDENCIES += $(if $(BR2_PACKAGE_OPUS),opus)

ifeq ($(BR2_ARM_CPU_ARMV5),y)
RECALBOX_MPLAYER_CONF_OPTS += --enable-armv5te
endif

ifeq ($(BR2_ARM_CPU_ARMV6)$(BR2_ARM_CPU_ARMV7A),y)
RECALBOX_MPLAYER_CONF_OPTS += --enable-armv6
endif

ifeq ($(BR2_aarch64),y)
RECALBOX_MPLAYER_CONF_OPTS += --enable-armv8
endif

ifeq ($(BR2_ARM_SOFT_FLOAT),)
ifeq ($(BR2_ARM_CPU_HAS_NEON),y)
RECALBOX_MPLAYER_CONF_OPTS += --enable-neon
RECALBOX_MPLAYER_CFLAGS += -mfpu=neon
endif
endif

ifeq ($(BR2_X86_CPU_HAS_MMX),y)
RECALBOX_MPLAYER_CONF_OPTS += \
	--enable-mmx \
	--yasm=$(HOST_DIR)/bin/yasm
RECALBOX_MPLAYER_DEPENDENCIES += host-yasm
else
RECALBOX_MPLAYER_CONF_OPTS += \
	--disable-mmx \
	--yasm=''
endif

ifeq ($(BR2_X86_CPU_HAS_SSE),y)
RECALBOX_MPLAYER_CONF_OPTS += --enable-mmxext --enable-sse
else
RECALBOX_MPLAYER_CONF_OPTS += --disable-mmxext --disable-sse
endif

ifeq ($(BR2_X86_CPU_HAS_SSE2),y)
RECALBOX_MPLAYER_CONF_OPTS += --enable-sse2
else
RECALBOX_MPLAYER_CONF_OPTS += --disable-sse2
endif

ifeq ($(BR2_X86_CPU_HAS_SSE3),y)
RECALBOX_MPLAYER_CONF_OPTS += --enable-sse3
else
RECALBOX_MPLAYER_CONF_OPTS += --disable-sse3
endif

ifeq ($(BR2_X86_CPU_HAS_SSSE3),y)
RECALBOX_MPLAYER_CONF_OPTS += --enable-ssse3
else
RECALBOX_MPLAYER_CONF_OPTS += --disable-ssse3
endif

ifeq ($(BR2_X86_CPU_HAS_SSE4),y)
RECALBOX_MPLAYER_CONF_OPTS += --enable-sse4
else
RECALBOX_MPLAYER_CONF_OPTS += --disable-sse4
endif

ifeq ($(BR2_X86_CPU_HAS_SSE42),y)
RECALBOX_MPLAYER_CONF_OPTS += --enable-sse42
else
RECALBOX_MPLAYER_CONF_OPTS += --disable-sse42
endif

ifeq ($(BR2_X86_CPU_HAS_AVX),y)
RECALBOX_MPLAYER_CONF_OPTS += --enable-avx
else
RECALBOX_MPLAYER_CONF_OPTS += --disable-avx
endif

ifeq ($(BR2_X86_CPU_HAS_AVX2),y)
RECALBOX_MPLAYER_CONF_OPTS += --enable-avx2
else
RECALBOX_MPLAYER_CONF_OPTS += --disable-avx2
endif

RECALBOX_MPLAYER_MISC_OPTS = \
	--enable-mplayer \
	--enable-dynamic-plugins \
	--enable-networking \
	--disable-lirc \
	--disable-lircc \
	--disable-joystick \
	--disable-apple-remote \
	--disable-apple-ir \
	--disable-vm \
	--disable-xf86keysym \
	--disable-radio \
	--disable-radio-capture \
	--disable-radio-v4l2 \
	--disable-radio-bsdbt848 \
	--disable-tv \
	--disable-tv-v4l1 \
	--disable-tv-v4l2 \
	--disable-tv-bsdbt848 \
	--disable-pvr \
	--disable-rtc \
	--disable-winsock2_h \
	--disable-smb \
	--disable-live \
	--disable-nemesi \
	--disable-librtmp \
	--disable-vcd \
	--disable-bluray \
	--disable-dvdnav \
	--disable-dvdread \
	--disable-cdparanoia \
	--disable-cddb \
	--disable-unrarexec \
	--disable-menu \
	--disable-maemo \
	--disable-macosx-finder \
	--disable-macosx-bundle \
	--disable-inet6 \
	--disable-sctp \
	--disable-gethostbyname2 \
	--disable-ftp \
	--disable-vstream \
	--disable-enca \
	--disable-librtmp \
	--disable-speex

RECALBOX_MPLAYER_CODECS_OPTS = \
	--enable-ffmpeg_a \
	--disable-ffmpeg_so \
	--disable-decoder=apng \
	--disable-encoder=apng \
	--disable-decoder=tdsc \
	--disable-libcdio \
	--disable-gif \
	--disable-tga \
	--disable-libmpeg2

RECALBOX_MPLAYER_VIDEO_OUT_OPTS = \
	--enable-fbdev \
	--disable-v4l2 \
	--disable-matrixview \
	--disable-gl \
	--disable-sdl \
	--disable-jpeg \
	--disable-xv \
	--disable-x11 \
	--disable-yuv4mpeg \
	--disable-md5sum \
	--disable-ivtv \
	--disable-dvb \
	--disable-pnm

RECALBOX_MPLAYER_AUDIO_OUT_OPTS = \
	--disable-v4l2 \
	--disable-sdl \
	--disable-ivtv \
	--disable-pulse

define RECALBOX_MPLAYER_CONFIGURE_CMDS
	(cd $(@D); rm -rf config.cache; \
		$(TARGET_CONFIGURE_OPTS) \
		$(TARGET_CONFIGURE_ARGS) \
		./configure \
		--prefix=/usr \
		--confdir=/etc \
		--target=$(GNU_TARGET_NAME) \
		--host-cc="$(HOSTCC)" \
		--cc="$(TARGET_CC)" \
		--as="$(TARGET_AS)" \
		--charset=UTF-8 \
		--extra-cflags="$(RECALBOX_MPLAYER_CFLAGS)" \
		--extra-ldflags="$(RECALBOX_MPLAYER_LDFLAGS)" \
		--enable-cross-compile \
		--enable-runtime-cpudetection \
		$(RECALBOX_MPLAYER_CONF_OPTS) \
		$(RECALBOX_MPLAYER_MISC_OPTS) \
		$(RECALBOX_MPLAYER_CODECS_OPTS) \
		$(RECALBOX_MPLAYER_VIDEO_OUT_OPTS) \
		$(RECALBOX_MPLAYER_AUDIO_OUT_OPTS) \
	)
endef

define RECALBOX_MPLAYER_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D)
endef

define RECALBOX_MPLAYER_INSTALL_TARGET_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) DESTDIR=$(TARGET_DIR) -C $(@D) install
endef

$(eval $(generic-package))
