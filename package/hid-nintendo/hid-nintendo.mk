################################################################################
#
# hid-nintendo
#
################################################################################

HID_NINTENDO_VERSION = 2712136b19eed75bff01c1a6ffe2a23daf78a7bb
HID_NINTENDO_SITE = $(call github,nicman23,dkms-hid-nintendo,$(HID_NINTENDO_VERSION))
HID_NINTENDO_LICENSE = GPL-2.0
HID_NINTENDO_LICENSE_FILES = LICENSE
HID_NINTENDO_MODULE_SUBDIRS = src

$(eval $(kernel-module))
$(eval $(generic-package))
