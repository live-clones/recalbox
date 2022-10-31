################################################################################
#
# uboot-rg353x
#
################################################################################

UBOOT_RG353X_VERSION = 286d6b404db538b36736af9a3f3a6f86f6ba927e
UBOOT_RG353X_SITE = https://gitlab.com/recalbox/packages/hardware/uboot-rg353x.git
UBOOT_RG353X_SITE_METHOD = git
UBOOT_RG353X_LICENSE = unknown

UBOOT_RG353X_INSTALL_TARGET = NO
UBOOT_RG353X_INSTALL_IMAGES = YES

define UBOOT_RG353X_INSTALL_IMAGES_CMDS
	$(INSTALL) -D $(@D)/uboot.img $(BINARIES_DIR)/
	$(INSTALL) -D $(@D)/idbloader.img $(BINARIES_DIR)/
endef

$(eval $(generic-package))
