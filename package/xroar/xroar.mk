################################################################################
#
# XROAR - TRS80 Coco & Dragon32/64
#
################################################################################

XROAR_VERSION = 0.37
XROAR_SITE = https://www.6809.org.uk/git/xroar.git
XROAR_SITE_METHOD = git
XROAR_DEPENDENCIES = sdl2
XROAR_LICENSE = GPL2 LGPL
XROAR_LICENSE_FILES = COPYING.GPL COMPYING.LGPL

define XROAR_RUN_AUTOGEN
	cd $(@D) && PATH=$(BR_PATH) ./autogen.sh
endef
XROAR_POST_PATCH_HOOKS += XROAR_RUN_AUTOGEN

XROAR_CONF_OPTS += CC="$(TARGET_CC)"
XROAR_CONF_OPTS += CFLAGS="$(COMPILER_COMMONS_CFLAGS_NOLTO)"
XROAR_CONF_OPTS += CPPFLAGS="$(COMPILER_COMMONS_CXXFLAGS_NOLTO)"
XROAR_CONF_OPTS += LDFLAGS="$(COMPILER_COMMONS_LDFLAGS_NOLTO)"
XROAR_CONF_OPTS += --enable-filereq-cli

define XROAR_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/src/xroar $(TARGET_DIR)/usr/bin/xroar
endef

$(eval $(autotools-package))
