################################################################################
#
# wii-u-gc-adapter
#
################################################################################

WII_U_GC_ADAPTER_VERSION = fa098efa7f6b34f8cd82e2c249c81c629901976c
WII_U_GC_ADAPTER_SITE = $(call github,ToadKing,wii-u-gc-adapter,$(WII_U_GC_ADAPTER_VERSION))
WII_U_GC_ADAPTER_LICENSE = GPL-2.0
WII_U_GC_ADAPTER_LICENSE_FILES = LICENSE

WII_U_GC_ADAPTER_DEPENDENCIES = libusb udev

define WII_U_GC_ADAPTER_BUILD_CMDS
	$(SED) "s|-O2|-O3|g" $(@D)/Makefile
	$(SED) "s|udev|libudev|g" $(@D)/Makefile
	CFLAGS="$(TARGET_CFLAGS)" \
		CXXFLAGS="$(TARGET_CXXFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)" \
		$(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D) -f Makefile
endef

define WII_U_GC_ADAPTER_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/wii-u-gc-adapter $(TARGET_DIR)/usr/bin/wii-u-gc-adapter
	$(INSTALL) -M 755 wii-u-gc-adapter $(TARGET_DIR)/usr/bin/
endef

$(eval $(generic-package))
