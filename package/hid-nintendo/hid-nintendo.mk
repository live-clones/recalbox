################################################################################
#
# hid-nintendo
#
################################################################################

HID_NINTENDO_VERSION = 86e53442872878e79cb454f5a16ccd1c9716f47f
HID_NINTENDO_SITE = $(call github,nicman23,dkms-hid-nintendo,$(HID_NINTENDO_VERSION))
HID_NINTENDO_LICENSE = GPL-2.0
HID_NINTENDO_LICENSE_FILES = LICENSE
HID_NINTENDO_MODULE_SUBDIRS = src

$(eval $(kernel-module))
$(eval $(generic-package))
