################################################################################
#
# recalbox-rockchip-mali
#
################################################################################

RECALBOX_ROCKCHIP_MALI_VERSION = ad4c28932c3d07c75fc41dd4a3333f9013a25e7f
RECALBOX_ROCKCHIP_MALI_SITE = $(call github,Txwv,rockchip-linux-libmali,$(RECALBOX_ROCKCHIP_MALI_VERSION))
RECALBOX_ROCKCHIP_MALI_INSTALL_STAGING = YES

RECALBOX_ROCKCHIP_MALI_DEPENDENCIES = libdrm
ifeq ($(RECALBOX_ROCKCHIP_MALI_BIFROST_VULKAN),)
RECALBOX_ROCKCHIP_MALI_PROVIDES = libegl libgles libgbm
endif
RECALBOX_ROCKCHIP_MALI_LICENSE = COPYRIGHT
RECALBOX_ROCKCHIP_MALI_LICENSE_FILES = END_USER_LICENCE_AGREEMENT.txt

ifeq ($(BR2_PACKAGE_RECALBOX_ROCKCHIP_MALI_BIFROST_G31),y)
RECALBOX_ROCKCHIP_MALI_CONF_OPTS += \
	-Dplatform=gbm \
	-Dgpu=bifrost-g31 \
	-Dversion=rxp0
else ifeq ($(BR2_PACKAGE_RECALBOX_ROCKCHIP_MALI_BIFROST_G52),y)
RECALBOX_ROCKCHIP_MALI_CONF_OPTS += \
	-Dplatform=gbm \
	-Dgpu=bifrost-g52 \
	-Dversion=g2p0
else ifeq ($(BR2_PACKAGE_RECALBOX_ROCKCHIP_MALI_MIDGARD),y)
RECALBOX_ROCKCHIP_MALI_CONF_OPTS += \
	-Dplatform=gbm \
	-Dgpu=midgard-t86x \
	-Dversion=r18p0
else ifeq ($(BR2_PACKAGE_RECALBOX_ROCKCHIP_MALI_UTGARD),y)
RECALBOX_ROCKCHIP_MALI_CONF_OPTS += \
	-Dplatform=gbm \
	-Dgpu=utgard-450 \
	-Dversion=r7p0
endif

ifneq ($(BR2_PACKAGE_MESA3D),y)
RECALBOX_ROCKCHIP_MALI_CONF_OPTS += -Dkhr-header=true
endif

$(eval $(meson-package))
