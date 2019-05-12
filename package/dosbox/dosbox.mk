################################################################################
#
# DosBox
#
################################################################################

DOSBOX_VERSION_TAG = 0.74
DOSBOX_VERSION = r4178
DOSBOX_SITE = svn://svn.code.sf.net/p/dosbox/code-0/dosbox/trunk
DOSBOX_SITE_METHOD = svn
DOSBOX_LICENSE = GPL2
DOSBOX_LICENSE_FILES = COPYING
DOSBOX_DEPENDENCIES = sdl2 zlib libpng libogg libvorbis sdl_sound sdl2_net
DOSBOX_AUTORECONF = YES
DOSBOX_AUTORECONF_OPTS = -i
DOSBOX_CONF_OPTS += --host="$(GNU_TARGET_NAME)" --enable-core-inline --prefix=/usr \
					--enable-dynrec --enable-unaligned_memory --disable-opengl \
					--with-sdl=sdl2 --with-sdl-prefix="$(STAGING_DIR)/usr"
DOSBOX_CONF_OPTS += LIBS="-lvorbisfile -lvorbis -logg"

DOSBOX_CFLAGS = -I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/usr/include/SDL2 $(COMPILER_COMMONS_CFLAGS_NOLTO)
DOSBOX_CXXFLAGS = -I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/usr/include/SDL2 $(COMPILER_COMMONS_CXXFLAGS_NOLTO)
DOSBOX_CPPFLAGS = -I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/usr/include/SDL2 $(COMPILER_COMMONS_CXXFLAGS_NOLTO)
DOSBOX_LDFLAGS = -L$(STAGING_DIR)/usr/lib $(COMPILER_COMMONS_LDFLAGS_NOLTO)

$(eval $(autotools-package))
