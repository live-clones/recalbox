################################################################################
#
# lineapple-recalbox
#
################################################################################

LINAPPLE_RECALBOX_VERSION = 2a2d557f7eaa3243e5546cfab7d64d5fe6c777d8
LINAPPLE_RECALBOX_SITE = $(call gitlab,recalbox/packages/standalone,linapple-new,$(LINAPPLE_RECALBOX_VERSION))
LINAPPLE_RECALBOX_LICENSE = GPL2
LINAPPLE_RECALBOX_LICENSE_FILES = COPYING

LINAPPLE_RECALBOX_DEPENDENCIES = sdl sdl_image libcurl zlib libzip
ifeq ($(BR2_PACKAGE_RPI_USERLAND),y)
LINAPPLE_RECALBOX_DEPENDENCIES += rpi-userland
endif

LINAPPLE_RECALBOX_MAKE_ENV = CFLAGS="$(TARGET_CFLAGS) $(COMPILER_COMMONS_CFLAGS_NOLTO)" CXXFLAGS="$(TARGET_CXXFLAGS) $(COMPILER_COMMONS_CXXFLAGS_NOLTO)" LDFLAGS="$(TARGET_LDFLAGS) $(COMPILER_COMMONS_LDFLAGS_NOLTO)"
LINAPPLE_RECALBOX_MAKE_OPTS = \
	-C $(@D)/ \
	CC="$(TARGET_CC)" \
	CXX="$(TARGET_CXX) -I $(STAGING_DIR)/usr/lib/libzip/include/" \
	LD="$(TARGET_LD)" \
	SDL_CONFIG=$(STAGING_DIR)/usr/bin/sdl-config \
	CURL_CONFIG=$(STAGING_DIR)/usr/bin/curl-config

define LINAPPLE_RECALBOX_FIX_EXTRACT
	$(SED) "s|strip|$(STAGING_DIR)/../bin/strip|g"  $(@D)/Makefile
	$(SED) "s|mkdir \"$(INSTDIR)|mkdir -p \"$(INSTDIR)|g" $(@D)/Makefile
endef

define LINAPPLE_RECALBOX_BUILD_CMDS
	$(LINAPPLE_RECALBOX_MAKE_ENV) $(MAKE) all $(LINAPPLE_RECALBOX_MAKE_OPTS)
endef

LINAPPLE_RECALBOX_CONFDIR = $(TARGET_DIR)/recalbox/share_init/system/.config/linapple
LINAPPLE_RECALBOX_CONFFILE = $(LINAPPLE_RECALBOX_CONFDIR)/linapple.conf.origin
define LINAPPLE_RECALBOX_INSTALL_TARGET_CMDS
	cp $(@D)/build/bin/linapple $(TARGET_DIR)/usr/bin/
	mkdir -p $(LINAPPLE_RECALBOX_CONFDIR)
	cp $(@D)/build/share/linapple/Master.dsk $(LINAPPLE_RECALBOX_CONFDIR)/
	cp $(@D)/build/etc/linapple/linapple.conf $(LINAPPLE_RECALBOX_CONFFILE)
	$(SED) "s|^\(\s*\)Slot 6 Directory =.*|\1Slot 6 Directory = /recalbox/share/roms/apple2|g" $(LINAPPLE_RECALBOX_CONFFILE)
	$(SED) "s|^\(\s*\)Save State Directory =.*|\1Save State Directory = /recalbox/share/saves/apple2|g" $(LINAPPLE_RECALBOX_CONFFILE)
	$(SED) "s|^\(\s*\)FTP Local Dir =.*|\1FTP Local Dir = /recalbox/share/roms/apple2|g" $(LINAPPLE_RECALBOX_CONFFILE)
endef

LINAPPLE_RECALBOX_POST_EXTRACT_HOOKS += LINAPPLE_RECALBOX_FIX_EXTRACT

$(eval $(generic-package))
