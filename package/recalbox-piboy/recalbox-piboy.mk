################################################################################
#
# RECALBOX_PIBOY
#
################################################################################

RECALBOX_PIBOY_VERSION = 0bcad43e36b5e6bc8a0963fc3ef8d842d20b18f5
RECALBOX_PIBOY_SITE = https://gitlab.com/recalbox/recalbox-piboy.git
RECALBOX_PIBOY_SITE_METHOD = git
RECALBOX_PIBOY_LICENSE = GPL-3.0
RECALBOX_PIBOY_LICENSE_FILES = LICENSE

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3),y)
define RECALBOX_PIBOY_BUILD_CMDS
	$(CC) $(@D)/piboy-tester.c -D RPI3 -o $(@D)/piboy-tester
endef
endif

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI4),y)
define RECALBOX_PIBOY_BUILD_CMDS
	$(CC) $(@D)/piboy-tester.c -D RPI4 -o $(@D)/piboy-tester
endef
endif

define RECALBOX_PIBOY_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 $(@D)/piboy-tester $(TARGET_DIR)/usr/bin/piboy-tester
endef

$(eval $(generic-package))
