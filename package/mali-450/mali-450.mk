################################################################################
#
# mali-450
#
################################################################################

MALI_450_VERSION = r6p1-01rel0
MALI_450_SITE = $(call github,recalbox,opengl-meson-gxbb,$(MALI_450_VERSION))
MALI_450_LICENSE = MIT

MALI_450_INSTALL_STAGING = YES
MALI_450_DEPENDENCIES = mali-opengles-sdk
MALI_450_PROVIDES = libegl libgles

MALI_450_TARGET_DIR=$(TARGET_DIR)
MALI_450_STAGING_DIR=$(STAGING_DIR)

define MALI_450_INSTALL_STAGING_CMDS
	@mkdir -p $(MALI_450_STAGING_DIR)
	@cp -r $(@D)/* $(MALI_450_STAGING_DIR)
endef

define MALI_450_INSTALL_TARGET_CMDS
	@mkdir -p $(MALI_450_TARGET_DIR)
	@cp -r $(@D)/* $(MALI_450_TARGET_DIR)
endef

$(eval $(generic-package))
