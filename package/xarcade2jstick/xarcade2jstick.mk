################################################################################
#
# xarcade2jstick
#
################################################################################

#XARCADE2JSTICK_VERSION = a7a93c951b12a81dac8bcd48ef0d760f56c72ba5
XARCADE2JSTICK_VERSION = d080123958dee9b2197e656e28c7859564a30893
XARCADE2JSTICK_SITE =  $(call github,petrockblog,xarcade2jstick,$(XARCADE2JSTICK_VERSION))
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
