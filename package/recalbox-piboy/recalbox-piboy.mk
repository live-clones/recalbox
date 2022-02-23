################################################################################
#
# RECALBOX_PIBOY
#
################################################################################

RECALBOX_PIBOY_VERSION = custom
RECALBOX_PIBOY_SITE = $(TOPDIR)/../projects/piboy
RECALBOX_PIBOY_SITE_METHOD = local
RECALBOX_PIBOY_LICENSE = GPL-3.0
RECALBOX_PIBOY_LICENSE_FILES = LICENSE

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3),y)
define RECALBOX_PIBOY_BUILD_CMDS
	$(TARGET_CC) $(@D)/piboy-tester.c -D RPI3 -o $(@D)/piboy-tester
endef
endif

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI4)$(BR2_PACKAGE_RECALBOX_TARGET_RPI4_64),y)
define RECALBOX_PIBOY_BUILD_CMDS
	$(TARGET_CC) $(@D)/piboy-tester.c -D RPI4 -o $(@D)/piboy-tester
endef
endif

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3)$(BR2_PACKAGE_RECALBOX_TARGET_RPI4)$(BR2_PACKAGE_RECALBOX_TARGET_RPI4_64),y)
define RECALBOX_PIBOY_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 $(@D)/piboy-tester $(TARGET_DIR)/usr/bin/piboy-tester
endef
endif

$(eval $(generic-package))
