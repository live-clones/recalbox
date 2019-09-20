################################################################################
#
# rockchip-mali
#
################################################################################

ROCKCHIP_MALI_SOURCE =  $(ROCKCHIP_MALI_VERSION).tar.gz
# NOTE: Newer commits do not work correctly with utgard - kodi starts but ES does not
ROCKCHIP_MALI_VERSION = 12daf22c405a4f8faf6cbc4d2e88b85b36dc61d9
ROCKCHIP_MALI_SITE = https://github.com/rockchip-linux/libmali/archive
ROCKCHIP_MALI_LICENSE = PROPRIETARY
ROCKCHIP_MALI_LICENSE_FILES = END_USER_LICENCE_AGREEMENT.txt
ROCKCHIP_MALI_INSTALL_STAGING = YES
ROCKCHIP_MALI_DEPENDENCIES = mali-opengles-sdk libdrm

# If Mesa is included in the build, compile it first
ifeq ($(BR2_PACKAGE_MESA3D),y)
ROCKCHIP_MALI_DEPENDENCIES += mesa3d
else
ROCKCHIP_MALI_PROVIDES = libgles libegl
endif

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RK3328),y)
ROCKCHIP_MALI_CONF_OPTS += -DTARGET_SOC=rk3328 -DGPU_ARCHITECTURE=utgard 
MALI_ARCH=arm-linux-gnueabihf
MALI_FILE=libmali-utgard-450-r7p0
MALI_DISP=gbm
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RK3399),y)
ROCKCHIP_MALI_CONF_OPTS += -DTARGET_SOC=rk3399 -DGPU_ARCHITECTURE=midgard -DGPU_HWREV=r2p0 
MALI_ARCH=arm-linux-gnueabihf
MALI_FILE=libmali-midgard-t86x-r14p0
MALI_DISP=gbm
endif

ROCKCHIP_MALI_CONF_OPTS += -DDP_FEATURE=$(MALI_DISP) -DABI=$(MALI_ARCH)

define ROCKCHIP_MALI_STAGING_POSTHOOKS
	mkdir -p $(STAGING_DIR)/usr/lib
	rm -f $(STAGING_DIR)/usr/lib/libmali*
	rm -f $(STAGING_DIR)/usr/lib/libMali*

	$(INSTALL) -m 755 $(@D)/lib/$(MALI_ARCH)/$(MALI_FILE)-$(MALI_DISP).so $(STAGING_DIR)/usr/lib/libmali.so

	# Defintely more symlinks that needed, but have seen all of these used at some point so it never hurts
	ln -sfv libmali.so $(STAGING_DIR)/usr/lib/libMali.so
	ln -sfv libmali.so $(STAGING_DIR)/usr/lib/libEGL.so
	ln -sfv libmali.so $(STAGING_DIR)/usr/lib/libEGL.so.1
	ln -sfv libmali.so $(STAGING_DIR)/usr/lib/libEGL.so.1.0
	ln -sfv libmali.so $(STAGING_DIR)/usr/lib/libEGL.so.1.0.0
	ln -sfv libmali.so $(STAGING_DIR)/usr/lib/libEGL.so.1.4
	ln -sfv libmali.so $(STAGING_DIR)/usr/lib/libbrcmEGL.so
	ln -sfv libmali.so $(STAGING_DIR)/usr/lib/libgbm.so
	ln -sfv libmali.so $(STAGING_DIR)/usr/lib/libgbm.so.1
	ln -sfv libmali.so $(STAGING_DIR)/usr/lib/libgbm.so.1.0.0
	ln -sfv libmali.so $(STAGING_DIR)/usr/lib/libGLESv1.so
	ln -sfv libmali.so $(STAGING_DIR)/usr/lib/libGLESv1_CM.so
	ln -sfv libmali.so $(STAGING_DIR)/usr/lib/libGLESv1_CM.so.1
	ln -sfv libmali.so $(STAGING_DIR)/usr/lib/libGLESv1_CM.so.1.1
	ln -sfv libmali.so $(STAGING_DIR)/usr/lib/libGLESv1_CM.so.1.1.0
	ln -sfv libmali.so $(STAGING_DIR)/usr/lib/libGLES_CM.so
	ln -sfv libmali.so $(STAGING_DIR)/usr/lib/libGLESv2.so
	ln -sfv libmali.so $(STAGING_DIR)/usr/lib/libGLESv2.so.2
	ln -sfv libmali.so $(STAGING_DIR)/usr/lib/libGLESv2.so.2.0
	ln -sfv libmali.so $(STAGING_DIR)/usr/lib/libGLESv2.so.2.0.0
	ln -sfv libmali.so $(STAGING_DIR)/usr/lib/libbrcmGLESv2.so
	ln -sfv libmali.so $(STAGING_DIR)/usr/lib/libMaliOpenCL.so
	ln -sfv libmali.so $(STAGING_DIR)/usr/lib/libOpenCL.so
endef
ROCKCHIP_MALI_POST_INSTALL_STAGING_HOOKS += ROCKCHIP_MALI_STAGING_POSTHOOKS

define ROCKCHIP_MALI_INSTALL_POSTHOOKS
	mkdir -p $(TARGET_DIR)/usr/lib
	rm -f $(TARGET_DIR)/usr/lib/libmali*
	rm -f $(TARGET_DIR)/usr/lib/libMali*
	$(INSTALL) -m 755 $(@D)/lib/$(MALI_ARCH)/$(MALI_FILE)-$(MALI_DISP).so $(TARGET_DIR)/usr/lib/libmali.so

	# Defintely more symlinks that needed, but have seen all of these used at some point so it never hurts
	ln -sfv libmali.so $(TARGET_DIR)/usr/lib/libMali.so
	ln -sfv libmali.so $(TARGET_DIR)/usr/lib/libEGL.so
	ln -sfv libmali.so $(TARGET_DIR)/usr/lib/libEGL.so.1
	ln -sfv libmali.so $(TARGET_DIR)/usr/lib/libEGL.so.1.0
	ln -sfv libmali.so $(TARGET_DIR)/usr/lib/libEGL.so.1.0.0
	ln -sfv libmali.so $(TARGET_DIR)/usr/lib/libEGL.so.1.4
	ln -sfv libmali.so $(TARGET_DIR)/usr/lib/libbrcmEGL.so
	ln -sfv libmali.so $(TARGET_DIR)/usr/lib/libgbm.so
	ln -sfv libmali.so $(TARGET_DIR)/usr/lib/libgbm.so.1
	ln -sfv libmali.so $(TARGET_DIR)/usr/lib/libgbm.so.1.0.0
	ln -sfv libmali.so $(TARGET_DIR)/usr/lib/libGLESv1.so
	ln -sfv libmali.so $(TARGET_DIR)/usr/lib/libGLESv1_CM.so
	ln -sfv libmali.so $(TARGET_DIR)/usr/lib/libGLESv1_CM.so.1
	ln -sfv libmali.so $(TARGET_DIR)/usr/lib/libGLESv1_CM.so.1.1
	ln -sfv libmali.so $(TARGET_DIR)/usr/lib/libGLESv1_CM.so.1.1.0
	ln -sfv libmali.so $(TARGET_DIR)/usr/lib/libGLES_CM.so
	ln -sfv libmali.so $(TARGET_DIR)/usr/lib/libGLESv2.so
	ln -sfv libmali.so $(TARGET_DIR)/usr/lib/libGLESv2.so.2
	ln -sfv libmali.so $(TARGET_DIR)/usr/lib/libGLESv2.so.2.0
	ln -sfv libmali.so $(TARGET_DIR)/usr/lib/libGLESv2.so.2.0.0
	ln -sfv libmali.so $(TARGET_DIR)/usr/lib/libbrcmGLESv2.so
	ln -sfv libmali.so $(TARGET_DIR)/usr/lib/libMaliOpenCL.so
	ln -sfv libmali.so $(TARGET_DIR)/usr/lib/libOpenCL.so
endef
ROCKCHIP_MALI_POST_INSTALL_TARGET_HOOKS += ROCKCHIP_MALI_INSTALL_POSTHOOKS

$(eval $(cmake-package))
$(eval $(host-cmake-package))
