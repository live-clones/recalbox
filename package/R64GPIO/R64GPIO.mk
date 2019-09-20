################################################################################
#
# R64GPIO - Rockchip GPIO API implementation
#
################################################################################

R64GPIO_VERSION = 389098b75dffdf2914f8a80d5457ac7fc8c4887e
R64GPIO_SITE = $(call github,mrfixit2001,Rock64-R64.GPIO,$(R64GPIO_VERSION))

define R64GPIO_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/lib/python2.7/site-packages/R64
	cp -r $(@D)/R64 $(TARGET_DIR)/usr/lib/python2.7/site-packages
endef

$(eval $(generic-package))
