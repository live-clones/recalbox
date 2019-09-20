################################################################################
#
# rkmpp
#
################################################################################

# efe951032bb06a54432ca09a4da61043d2b9a3ce - previously used - works well
#RKMPP_SITE = $(call github,ayufan-rock64,mpp,$(RKMPP_VERSION))

RKMPP_VERSION = 1babbc9cc277692d02fc4727daef2e91f91ce8b0 
RKMPP_SITE = $(call github,rockchip-linux,mpp,$(RKMPP_VERSION))
RKMPP_LICENSE = APL
RKMPP_DEPENDENCIES = libdrm
RKMPP_INSTALL_STAGING = YES

ifneq (,$(findstring rk3328,$(BR2_LINUX_KERNEL_INTREE_DTS_NAME)))
	RKMPP_CONF_OPTS +=-DENABLE_VP9D=ON
else
	ifneq (,$(findstring rk3399,$(BR2_LINUX_KERNEL_INTREE_DTS_NAME)))
		RKMPP_CONF_OPTS +=-DENABLE_VP9D=ON
	else
		RKMPP_CONF_OPTS +=-DENABLE_VP9D=OFF
	endif
endif


RKMPP_CONF_OPTS += \
	-DRKPLATFORM=ON \
	-DENABLE_AVSD=OFF \
	-DENABLE_H263D=OFF \
	-DENABLE_H264D=ON \
	-DENABLE_H265D=ON \
	-DENABLE_MPEG2D=ON \
	-DENABLE_MPEG4D=ON \
	-DENABLE_VP8D=ON \
	-DENABLE_JPEGD=OFF \
	-DHAVE_DRM=ON

$(eval $(cmake-package))
