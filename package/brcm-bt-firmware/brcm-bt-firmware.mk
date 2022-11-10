################################################################################
#
# brcm-bt-firmware
#
################################################################################

BRCM_BT_FIRMWARE_VERSION = a0eb4805dbb232f02f156b9351a23790c1a4cec7
BRCM_BT_FIRMWARE_SITE = $(call github,winterheart,broadcom-bt-firmware,$(BRCM_BT_FIRMWARE_VERSION))
BRCM_BT_FIRMWARE_LICENSE = MULTIPLE
BRCM_BT_FIRMWARE_NON_COMMERCIAL = y

$(eval $(cmake-package))
