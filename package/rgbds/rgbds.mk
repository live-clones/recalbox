################################################################################
#
# RGBDS
#
################################################################################

RGBDS_VERSION = v0.3.8
RGBDS_SITE = $(call github,rednex,rgbds,$(RGBDS_VERSION))
RGBDS_DEPENDENCIES = libpng
RGBDS_INSTALL_TARGET = NO
RGBDS_INSTALL_STAGING = YES

define RGBDS_BUILD_CMDS
	$(MAKE) -C $(@D)
endef

define RGBDS_INSTALL_STAGING_CMDS
	$(INSTALL) -D $(@D)/rgbasm $(STAGING_DIR)/usr/bin/
	$(INSTALL) -D $(@D)/rgbfix $(STAGING_DIR)/usr/bin/
	$(INSTALL) -D $(@D)/rgbgfx $(STAGING_DIR)/usr/bin/
	$(INSTALL) -D $(@D)/rgblink $(STAGING_DIR)/usr/bin/
endef

$(eval $(generic-package))
