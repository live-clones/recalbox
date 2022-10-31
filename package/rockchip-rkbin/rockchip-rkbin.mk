################################################################################
#
# rockchip-rkbin
#
################################################################################

ROCKCHIP_RKBIN_VERSION = 7d631e0d5b2d373b54d4533580d08fb9bd2eaad4
ROCKCHIP_RKBIN_SITE = $(call github,rockchip-linux,rkbin,$(ROCKCHIP_RKBIN_VERSION))

ROCKCHIP_RKBIN_LICENSE = unknown
HOST_ROCKCHIP_RKBIN_DEPENDENCIES = host-util-linux

define HOST_ROCKCHIP_RKBIN_INSTALL_CMDS
	mkdir -p $(HOST_DIR)/rkbin
	cp -rvp $(@D)/. $(HOST_DIR)/rkbin/
endef

$(eval $(generic-package))
$(eval $(host-generic-package))
