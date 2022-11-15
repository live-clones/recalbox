################################################################################
#
# atari800
#
################################################################################

ATARI800_VERSION = 5fd4939365a081ea73055ed58ea097e638e63643
#ATARI800_VERSION = ATARI800_5_0_0
ATARI800_SITE = $(call github,atari800,atari800,$(ATARI800_VERSION))
ATARI800_LICENSE = GPL-2.0
ATARI800_NON_COMMERCIAL = y

ATARI800_AUTORECONF = YES

#define ATARI800_RUN_AUTOGEN
#	cd $(@D) && PATH=$(BR_PATH) ./autogen.sh
#endef

#ATARI800_PRE_CONFIGURE_HOOKS += ATARI800_RUN_AUTOGEN

#ATARI800_CONF_ENV += CFLAGS=-I$(STAGING_DIR)/usr/include/SDL LDFLAGS=-L$(STAGING_DIR)/usr/lib/

ATARI800_CONF_OPTS += \
    --target=default \
    --without-x \
	--prefix=$(TARGET_DIR)/usr \
	--exec-prefix=$(TARGET_DIR)/usr \
	--enable-pthreads \
	--disable-oss \
	--enable-onscreenkeyboard

ifeq ($(BR2_PACKAGE_SDL),y)
ATARI800_DEPENDENCIES += sdl
ATARI800_CONF_OPTS += \
	--with-video=sdl \
	--with-sound=sdl \
	--enable-opengl-by-default \
	--with-opengl \
	--with-sdl-prefix=$(STAGING_DIR)/usr \
	--with-sdl-exec-prefix=$(STAGING_DIR)/usr
endif

ifeq ($(BR2_PACKAGE_ALSA_LIB),y)
ATARI800_DEPENDENCIES += alsa-lib
ATARI800_CONF_OPTS += --enable-alsa
else
ATARI800_CONF_OPTS += --disable-alsa
endif

ifeq ($(BR2_PACKAGE_FREETYPE),y)
ATARI800_DEPENDENCIES += freetype
ATARI800_CONF_OPTS += \
	--enable-freetype \
	--with-freetype-prefix=$(STAGING_DIR)/usr
else
ATARI800_CONF_OPTS += --disable-freetype
endif

$(eval $(autotools-package))
