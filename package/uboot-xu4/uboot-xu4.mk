################################################################################
#
# uboot-xu4
#
################################################################################

UBOOT_XU4_VERSION = odroidxu4-v2017.05
UBOOT_XU4_SITE = $(call github,hardkernel,u-boot,$(UBOOT_XU4_VERSION))
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
