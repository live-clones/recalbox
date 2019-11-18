################################################################################
#
# Supermodel
#
################################################################################

SUPERMODEL_VERSION = r782
SUPERMODEL_SITE = svn://svn.code.sf.net/p/model3emu/code/trunk
SUPERMODEL_SITE_METHOD = svn
SUPERMODEL_LICENSE = GPL2
SUPERMODEL_DEPENDENCIES = sdl2 zlib libpng libogg libvorbis sdl_sound sdl2_net

define SUPERMODEL_BUILD_CMDS
	cp $(@D)/Makefiles/Makefile.UNIX $(@D)/Makefile
	$(SED) "s|CC = gcc|CC = $(TARGET_CC)|g" $(@D)/Makefile
	$(SED) "s|CXX = g++|CXX = $(TARGET_CXX)|g" $(@D)/Makefile
	$(SED) "s|LD = gcc|LD = $(TARGET_CC)|g" $(@D)/Makefile
	$(SED) "s|sdl-config|$(STAGING_DIR)/usr/bin/sdl-config|g" $(@D)/Makefile
	$(TARGET_CONFIGURE_OPTS) $(MAKE) -C $(@D) -f Makefile
endef

define SUPERMODEL_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/bin/supermodel $(TARGET_DIR)/usr/bin/supermodel
	$(INSTALL) -D -m 0755 $(@D)/Configs/Games.xml $(TARGET_DIR)/recalbox/share_init/roms/model3/Games.xml
	$(INSTALL) -D -m 0755 $(@D)/Configs/Supermodel.ini $(TARGET_DIR)/recalbox/share_init/configs/model3/Supermodel.ini
endef

$(eval $(generic-package))
