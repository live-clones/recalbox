################################################################################
#
# rockchip-mali
#
################################################################################

ROCKCHIP_MALI_VERSION = ad4c28932c3d07c75fc41dd4a3333f9013a25e7f
ROCKCHIP_MALI_SITE = $(call github,rockchip-linux,libmali,$(ROCKCHIP_MALI_VERSION))
ROCKCHIP_MALI_INSTALL_STAGING = YES

ROCKCHIP_MALI_PROVIDES = libegl libgles
ROCKCHIP_MALI_DEPENDENCIES = libdrm

ROCKCHIP_MALI_LICENSE = COPYRIGHT
ROCKCHIP_MALI_LICENSE_FILES = END_USER_LICENCE_AGREEMENT.txt

ifeq ($(BR2_PACKAGE_ROCKCHIP_MALI_BIFROST),y)
ROCKCHIP_MALI_CONF_OPTS = \
	-Dplatform=gbm \
	-Dgpu=bifrost-g31 \
	-Dversion=rxp0
else ifeq ($(BR2_PACKAGE_ROCKCHIP_MALI_MIDGARD),y)
ROCKCHIP_MALI_CONF_OPTS = \
	-Dplatform=gbm \
	-Dgpu=midgard-t86x \
	-Dversion=r18p0
else ifeq ($(BR2_PACKAGE_ROCKCHIP_MALI_UTGARD),y)
ROCKCHIP_MALI_CONF_OPTS = \
	-Dplatform=gbm \
	-Dgpu=utgard-450 \
	-Dversion=r7p0
endif

ifneq ($(BR2_PACKAGE_MESA3D),y)
ROCKCHIP_MALI_CONF_OPTS += -Dkhr-header=true
endif

$(eval $(meson-package))
