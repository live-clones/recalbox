################################################################################
#
# ROCKCHIP_MPP
#
################################################################################

ROCKCHIP_MPP_VERSION = b0d0a5aafd328078d19455f606ddfd0f8103d7de
ROCKCHIP_MPP_SITE = $(call github,rockchip-linux,mpp,$(ROCKCHIP_MPP_VERSION))
ROCKCHIP_MPP_LICENSE = Apache-2.0

ROCKCHIP_MPP_INSTALL_STAGING = YES

$(eval $(cmake-package))
