################################################################################
#
# HHPC
#
################################################################################
HHPC_VERSION = e8803cd5c2b51f340fbb76c3ace87ad1c932161b
HHPC_SITE = $(call github,aktau,hhpc,$(HHPC_VERSION))
HHPC_LICENSE = BSD-3c
HHPC_DEPENDENCIES = xserver_xorg-server

define HHPC_BUILD_CMDS
	CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" $(MAKE) CXX="$(TARGET_CXX)" CC="$(TARGET_CC)" -C $(@D) DESTDIR="$(TARGET_DIR)" PREFIX="/usr"
endef

define HHPC_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/hhpc \
		$(TARGET_DIR)/usr/bin/hhpc
endef

$(eval $(generic-package))
