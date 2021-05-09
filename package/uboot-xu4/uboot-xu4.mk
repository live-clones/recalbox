################################################################################
#
# uboot-xu4
#
################################################################################

# 19/05/2020
UBOOT_XU4_VERSION = 42ac93dcfbbb8a08c2bdc02e19f96eb35a81891a
UBOOT_XU4_SITE = $(call github,hardkernel,u-boot,$(UBOOT_XU4_VERSION))
UBOOT_XU4_LICENSE = GPL-2.0+
UBOOT_XU4_LICENSE_FILES = Licenses/gpl-2.0.txt

UBOOT_XU4_INSTALL_TARGET = NO
UBOOT_XU4_INSTALL_IMAGES = YES

define UBOOT_XU4_INSTALL_IMAGES_CMDS
	cp -dpf $(@D)/sd_fuse/u-boot.bin.hardkernel \
		$(@D)/sd_fuse/bl1.bin.hardkernel \
		$(@D)/sd_fuse/bl2.bin.hardkernel.720k_uboot \
		$(@D)/sd_fuse/tzsw.bin.hardkernel \
		$(BINARIES_DIR)
endef
$(eval $(generic-package))
