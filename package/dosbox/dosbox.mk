################################################################################
#
# DosBox
#
################################################################################

DOSBOX_VERSION_TAG = 0.74
DOSBOX_VERSION = r4067
DOSBOX_SITE =  svn://svn.code.sf.net/p/dosbox/code-0/dosbox/trunk 
DOSBOX_SITE_METHOD = svn
DOSBOX_LICENSE = GPL2
DOSBOX_LICENSE_FILES = COPYING
DOSBOX_DEPENDENCIES = sdl2 zlib libpng libogg libvorbis sdl_sound sdl2_net

DOSBOX_LDFLAGS = -L$(STAGING_DIR)/usr/lib
DOSBOX_CFLAGS = -I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/usr/include/SDL2

define DOSBOX_RUN_AUTOGEN
	cd $(@D); ./autogen.sh
endef

DOSBOX_PRE_CONFIGURE_HOOKS += DOSBOX_RUN_AUTOGEN

DOSBOX_CONF_ENV = CFLAGS="$(DOSBOX_CFLAGS)" CXXFLAGS="$(DOSBOX_CFLAGS)" \
		CPPFLAGS="$(DOSBOX_CFLAGS)" LDFLAGS="$(DOSBOX_LDFLAGS)" \
                CROSS_COMPILE="$(HOST_DIR)/usr/bin/" LIBS="-lvorbisfile -lvorbis -logg"
DOSBOX_CONF_OPTS = --host="$(GNU_TARGET_NAME)" --enable-core-inline --prefix=/usr --enable-dynrec --enable-unaligned_memory \
                --disable-opengl --with-sdl=sdl2 --with-sdl-prefix="$(STAGING_DIR)/usr"

$(eval $(autotools-package))
