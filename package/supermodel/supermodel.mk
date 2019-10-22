################################################################################
#
# Supermodel
#
################################################################################

SUPERMODEL_VERSION = r777
SUPERMODEL_SITE = svn://svn.code.sf.net/p/model3emu/code/trunk
SUPERMODEL_SITE_METHOD = svn
SUPERMODEL_LICENSE = GPL2
SUPERMODEL_DEPENDENCIES = sdl2 zlib libpng libogg libvorbis sdl_sound sdl2_net

define SUPERMODEL_FIX_SDL_CONFIG_PATH
	$(SED) "s+sdl-config+$(STAGING_DIR)/usr/bin/sdl-config+g" $(@D)/Makefiles/Makefile.UNIX
		$(SED) "s+(SDL_LIBS)+(STAGING_DIR)/bin/+g" $(@D)/Makefiles/Makefile.UNIX
endef

TARGET_LDFLAGS = 

define SUPERMODEL_CONFIGURE_CMDS
	ln -s $(@D)/Makefiles/Makefile.UNIX $(@D)/Makefile; \
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" LDFLAGS="$(TARGET_LDFLAGS)" LD="$(TARGET_CC)" $(@D)/ NET_BOARD=1
endef

define SUPERMODEL_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/bin/supermodel \
		$(TARGET_DIR)/usr/bin/supermodel
	$(INSTALL) -D $(@D)/Configs/Games.xml \
		$(TARGET_DIR)/recalbox/share_init/roms/model3/Games.xml
	$(INSTALL) -D $(@D)/Configs/Supermodel.ini \
		$(TARGET_DIR)/recalbox/share_init/configs/model3/Supermodel.ini
endef

SUPERMODEL_PRE_CONFIGURE_HOOKS += SUPERMODEL_FIX_SDL_CONFIG_PATH

$(eval $(autotools-package))
