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

DOSBOX_LDFLAGS = -L$(STAGING_DIR)/usr/lib
DOSBOX_CFLAGS = -I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/usr/include/SDL2

define DOSBOX_CONFIGURE_CMDS
	(cd $(@D); \
		./autogen.sh; \
		$(TARGET_CONFIGURE_ARGS) $(TARGET_CONFIGURE_OPTS) \
			CFLAGS="$(TARGET_CFLAGS) $(DOSBOX_CFLAGS) $(COMPILER_COMMONS_CFLAGS_NOLTO)" \
			CXXFLAGS="$(TARGET_CXXFLAGS) $(DOSBOX_CFLAGS) $(COMPILER_COMMONS_CXXFLAGS_NOLTO)" \
			CPPFLAGS="$(TARGET_CPPFLAGS) $(DOSBOX_CFLAGS) $(COMPILER_COMMONS_CXXFLAGS_NOLTO)" \
			LDFLAGS="$(TARGET_LDFLAGS) $(DOSBOX_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_NOLTO)" \
			CROSS_COMPILE="$(HOST_DIR)/usr/bin/" \
			LIBS="-lvorbisfile -lvorbis -logg" \
			./configure --host="$(GNU_TARGET_NAME)" \
				--enable-core-inline --prefix=/usr \
				--enable-dynrec --enable-unaligned_memory \
				--disable-opengl --with-sdl=sdl2 \
				--with-sdl-prefix="$(STAGING_DIR)/usr"; \
	)
endef

$(eval $(autotools-package))
