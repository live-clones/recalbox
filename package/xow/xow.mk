################################################################################
#
# XOW
#
################################################################################

XOW_VERSION = 700529b2517df7d17ad5a2fa0bd679143ac48666
XOW_SITE =  $(call github,medusalix,xow,$(XOW_VERSION))
XOW_LICENSE = GPL-2.0
XOW_LICENSE_FILES = LICENSE

XOW_DEPENDENCIES = libusb

define XOW_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS) -I$(STAGING_DIR)/usr/include" CXXFLAGS="$(TARGET_CXXFLAGS) -I$(STAGING_DIR)/usr/include" LD_FLAGS="$(TARGET_LDFLAGS)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" LD="$(TARGET_LD)" STRIP="$(TARGET_STRIP)" -C $(@D) PREFIX="/usr"
endef

define XOW_INSTALL_TARGET_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" LD="$(TARGET_LD)" STRIP="$(TARGET_STRIP)" -C $(@D) install DESTDIR="$(TARGET_DIR)" PREFIX="/usr"
endef

define XOW_INIT_INSTALL
	$(INSTALL) -D -m 0755 $(XOW_PKGDIR)/xow-init $(TARGET_DIR)/etc/init.d/S17xow
endef

XOW_POST_INSTALL_TARGET_HOOKS += XOW_INIT_INSTALL

$(eval $(generic-package))
