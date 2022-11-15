################################################################################
#
# XA65
#
################################################################################

XA65_VERSION = b5c1e9b902da49d4111d8f8a916f68b114b822bb
XA65_SITE =  $(call github,fachat,xa65,$(XA65_VERSION))
XA65_LICENSE = CUSTOM
XA65_LICENSE_FILES = README

define HOST_XA65_BUILD_CMDS
	$(MAKE) -C $(@D)/xa
endef

define HOST_XA65_INSTALL_CMDS
	$(INSTALL) -D $(@D)/xa/xa $(HOST_DIR)/usr/bin/xa
endef

$(eval $(host-generic-package))
