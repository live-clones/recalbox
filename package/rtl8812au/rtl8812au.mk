################################################################################
#
# RTL8812AU
#
################################################################################

RTL8812AU_VERSION = 2dad788f5d71d50df4f692b67a4428967ba3d42c
RTL8812AU_SITE = $(call github,gordboy,rtl8812au,$(RTL8812AU_VERSION))
RTL8812AU_LICENSE = GPL-2.0
RTL8812AU_LICENSE_FILES = LICENSE

RTL8812AU_MODULE_MAKE_OPTS = \
	CONFIG_RTL8812AU=m \
	KVER=$(LINUX_VERSION_PROBED) \
	KSRC=$(LINUX_DIR) \
	BuildDir=$(@D)

$(eval $(kernel-module))
$(eval $(generic-package))
