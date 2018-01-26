################################################################################
#
# uboot-c2
#
################################################################################

UBOOT_C2_VERSION = 491e9bb63a4adbd07ae6b15439e257a36063c6d9
UBOOT_C2_SITE = $(call github,mdrjr,c2_uboot_binaries,$(UBOOT_C2_VERSION))
UBOOT_C2_INSTALL_TARGET = NO
UBOOT_C2_INSTALL_IMAGES = YES

define UBOOT_C2_INSTALL_IMAGES_CMDS
	cp -dpf $(@D)/u-boot.bin $(@D)/bl1.bin.hardkernel $(BINARIES_DIR)
endef
$(eval $(generic-package))
