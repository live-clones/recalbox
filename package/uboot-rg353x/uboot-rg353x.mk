################################################################################
#
# uboot-rg353x
#
################################################################################

UBOOT_RG353X_VERSION = 4a075f59d3c5e0c7170a4e85fd6ce0901c8568f9
UBOOT_RG353X_SITE = https://gitlab.com/recalbox/packages/hardware/uboot-rg353x.git
UBOOT_RG353X_SITE_METHOD = git
UBOOT_RG353X_LICENSE = unknown
UBOOT_RG353X_DEPENDENCIES = linux host-python3

UBOOT_RG353X_INSTALL_TARGET = NO
UBOOT_RG353X_INSTALL_IMAGES = YES

define UBOOT_RG353X_BUILD_CMDS
	$(INSTALL) $(UBOOT_RG353X_PKGDIR)/assets/logo.bmp $(LINUX_DIR)/logo_kernel.bmp
	$(INSTALL) $(UBOOT_RG353X_PKGDIR)/assets/logo2.bmp $(LINUX_DIR)/logo.bmp
	$(INSTALL) $(UBOOT_RG353X_PKGDIR)/assets/battery_0.bmp $(LINUX_DIR)/battery_0.bmp
	$(INSTALL) $(UBOOT_RG353X_PKGDIR)/assets/battery_1.bmp $(LINUX_DIR)/battery_1.bmp
	$(INSTALL) $(UBOOT_RG353X_PKGDIR)/assets/battery_2.bmp $(LINUX_DIR)/battery_2.bmp
	$(INSTALL) $(UBOOT_RG353X_PKGDIR)/assets/battery_3.bmp $(LINUX_DIR)/battery_3.bmp
	$(INSTALL) $(UBOOT_RG353X_PKGDIR)/assets/battery_4.bmp $(LINUX_DIR)/battery_4.bmp
	$(INSTALL) $(UBOOT_RG353X_PKGDIR)/assets/battery_5.bmp $(LINUX_DIR)/battery_5.bmp
	$(INSTALL) $(UBOOT_RG353X_PKGDIR)/assets/battery_fail.bmp $(LINUX_DIR)/battery_fail.bmp
	cd $(LINUX_DIR); \
		./scripts/mkmultidtb.py rg353x
endef

define UBOOT_RG353X_INSTALL_IMAGES_CMDS
	$(INSTALL) -D $(@D)/uboot.img $(BINARIES_DIR)/
	$(INSTALL) -D $(@D)/idbloader.img $(BINARIES_DIR)/
	$(INSTALL) -D $(LINUX_DIR)/resource.img $(BINARIES_DIR)/
endef

$(eval $(generic-package))
