################################################################################
#
# mali-t62x
#
################################################################################

MALI_T62X_DEPENDENCIES = mali-opengles-sdk libdrm
MALI_T62X_LICENSE = COPYRIGHT

ifeq ($(BR2_PACKAGE_MALI_T62X_BIGMALI),y)
MALI_T62X_VERSION = unknown
MALI_T62X_SOURCE = bigmali.tar
MALI_T62X_SITE = http://deb.odroid.in
else
ifeq ($(BR2_PACKAGE_MALI_T62X_FBDEV),y)
MALI_T62X_FLAVOR = fbdev
else ifeq ($(BR2_PACKAGE_MALI_T62X_GBM), y)
MALI_T62X_FLAVOR = wayland
MALI_T62X_DEPENDENCIES += wayland
endif
MALI_T62X_VERSION = r12p004rel0
MALI_T62X_SOURCE = malit62x$(MALI_T62X_VERSION)linux1$(MALI_T62X_FLAVOR).tar.gz
MALI_T62X_SITE = https://developer.arm.com/-/media/Files/downloads/mali-drivers/user-space/odroid-xu3
endif

MALI_T62X_INSTALL_STAGING = YES
MALI_T62X_PROVIDES = libegl libgles

MALI_T62X_TARGET_DIR=$(TARGET_DIR)/usr/lib
MALI_T62X_STAGING_DIR=$(STAGING_DIR)/usr/lib

ifeq ($(BR2_PACKAGE_MALI_T62X_BIGMALI),y)
define MALI_T62X_EXTRACT_CMDS
	cat $(MALI_T62X_DL_DIR)/$(MALI_T62X_SOURCE) | tar -C $(@D) -xf -
endef
define MALI_T62X_INSTALL_STAGING_CMDS
	$(INSTALL) -D $(@D)/libmali.so $(MALI_T62X_STAGING_DIR)/libmali.so
	ln -rsf $(MALI_T62X_STAGING_DIR)/libmali.so $(MALI_T62X_STAGING_DIR)/libEGL.so
	ln -rsf $(MALI_T62X_STAGING_DIR)/libmali.so $(MALI_T62X_STAGING_DIR)/libEGL.so.1
	ln -rsf $(MALI_T62X_STAGING_DIR)/libmali.so $(MALI_T62X_STAGING_DIR)/libgbm.so
	ln -rsf $(MALI_T62X_STAGING_DIR)/libmali.so $(MALI_T62X_STAGING_DIR)/libgbm.so.1
	ln -rsf $(MALI_T62X_STAGING_DIR)/libmali.so $(MALI_T62X_STAGING_DIR)/libGLESv1_CM.so
	ln -rsf $(MALI_T62X_STAGING_DIR)/libmali.so $(MALI_T62X_STAGING_DIR)/libGLESv1_CM.so.1
	ln -rsf $(MALI_T62X_STAGING_DIR)/libmali.so $(MALI_T62X_STAGING_DIR)/libGLESv2.so
	ln -rsf $(MALI_T62X_STAGING_DIR)/libmali.so $(MALI_T62X_STAGING_DIR)/libGLESv2.so.2
	ln -rsf $(MALI_T62X_STAGING_DIR)/libmali.so $(MALI_T62X_STAGING_DIR)/libOpenCL.so
	ln -rsf $(MALI_T62X_STAGING_DIR)/libmali.so $(MALI_T62X_STAGING_DIR)/libOpenCL.so.1
endef
define MALI_T62X_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/libmali.so $(MALI_T62X_TARGET_DIR)/libmali.so
	ln -rsf $(MALI_T62X_TARGET_DIR)/libmali.so $(MALI_T62X_TARGET_DIR)/libEGL.so
	ln -rsf $(MALI_T62X_TARGET_DIR)/libmali.so $(MALI_T62X_TARGET_DIR)/libEGL.so.1
	ln -rsf $(MALI_T62X_TARGET_DIR)/libmali.so $(MALI_T62X_TARGET_DIR)/libgbm.so
	ln -rsf $(MALI_T62X_TARGET_DIR)/libmali.so $(MALI_T62X_TARGET_DIR)/libgbm.so.1
	ln -rsf $(MALI_T62X_TARGET_DIR)/libmali.so $(MALI_T62X_TARGET_DIR)/libGLESv1_CM.so
	ln -rsf $(MALI_T62X_TARGET_DIR)/libmali.so $(MALI_T62X_TARGET_DIR)/libGLESv1_CM.so.1
	ln -rsf $(MALI_T62X_TARGET_DIR)/libmali.so $(MALI_T62X_TARGET_DIR)/libGLESv2.so
	ln -rsf $(MALI_T62X_TARGET_DIR)/libmali.so $(MALI_T62X_TARGET_DIR)/libGLESv2.so.2
	ln -rsf $(MALI_T62X_TARGET_DIR)/libmali.so $(MALI_T62X_TARGET_DIR)/libOpenCL.so
	ln -rsf $(MALI_T62X_TARGET_DIR)/libmali.so $(MALI_T62X_TARGET_DIR)/libOpenCL.so.1
endef
else
define MALI_T62X_INSTALL_STAGING_CMDS
	mkdir -p $(MALI_T62X_STAGING_DIR)
	cp -r $(@D)/* $(MALI_T62X_STAGING_DIR)
endef

define MALI_T62X_INSTALL_TARGET_CMDS
	mkdir -p $(MALI_T62X_TARGET_DIR)
	cp -r $(@D)/* $(MALI_T62X_TARGET_DIR)
endef
endif

$(eval $(generic-package))
