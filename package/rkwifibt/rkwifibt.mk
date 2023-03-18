################################################################################
#
# rkwifibt
#
################################################################################

RKWIFIBT_VERSION = a03e128aa4b52fc5d904634c9838623a74ade393
RKWIFIBT_SITE = $(call github,Caesar-github,rkwifibt,$(RKWIFIBT_VERSION))
RKWIFIBT_LICENSE = BSD
RKWIFIBT_LICENSE_FILES = LICENSE

define RKWIFIBT_BUILD_CMDS
	$(MAKE) $(TARGET_CONFIGURE_OPTS) -C $(@D)/realtek/rtk_hciattach/ all
endef

define RKWIFIBT_INSTALL_HCIATTACH_CMDS
	$(INSTALL) -D -m 755 $(@D)/realtek/rtk_hciattach/rtk_hciattach $(TARGET_DIR)/usr/bin/rtk_hciattach
endef

define RKWIFIBT_INSTALL_FIRMWARE_CMDS
	$(INSTALL) -D -m 644 $(@D)/realtek/RTL8821CS/rtl8821c_config $(TARGET_DIR)/lib/firmware/rtlbt/rtl8821c_config
	$(INSTALL) -D -m 644 $(@D)/realtek/RTL8821CS/rtl8821c_fw $(TARGET_DIR)/lib/firmware/rtlbt/rtl8821c_fw
endef

RKWIFIBT_POST_INSTALL_TARGET_HOOKS += RKWIFIBT_INSTALL_HCIATTACH_CMDS \
	RKWIFIBT_INSTALL_FIRMWARE_CMDS

$(eval $(generic-package))
