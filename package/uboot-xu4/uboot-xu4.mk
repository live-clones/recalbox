################################################################################
#
# uboot-xu4
#
################################################################################

UBOOT_XU4_VERSION = 8a47b2602ed2e348ed2a57dc2cd874c4605c538d
UBOOT_XU4_SITE = $(call github,hardkernel,u-boot,$(UBOOT_XU4_VERSION))
UBOOT_XU4_INSTALL_TARGET = NO
UBOOT_XU4_INSTALL_IMAGES = YES

define UBOOT_XU4_INSTALL_IMAGES_CMDS
	cp -dpf $(@D)/sd_fuse/hardkernel/u-boot.bin.hardkernel \
		$(@D)/sd_fuse/hardkernel/bl1.bin.hardkernel \
		$(@D)/sd_fuse/hardkernel/bl2.bin.hardkernel \
		$(@D)/sd_fuse/hardkernel/tzsw.bin.hardkernel \
		$(BINARIES_DIR)
endef
$(eval $(generic-package))
