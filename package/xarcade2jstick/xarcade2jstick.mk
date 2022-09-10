################################################################################
#
# xarcade2jstick
#
################################################################################

XARCADE2JSTICK_VERSION = 1b5d545b63d49c849b0231bcdb686d238803ee15
XARCADE2JSTICK_SITE =  $(call github,ian57,xarcade2jstick,$(XARCADE2JSTICK_VERSION))
XARCADE2JSTICK_LICENSE = GPL-3.0
XARCADE2JSTICK_LICENSE_FILES = gpl3.txt

XARCADE2JSTICK_DEPENDENCIES = linux

define XARCADE2JSTICK_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D) all
endef

define XARCADE2JSTICK_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/src/xarcade2jstick $(TARGET_DIR)/usr/bin/xarcade2jstick
endef

$(eval $(generic-package))
