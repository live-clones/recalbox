################################################################################
#
# recalbox-bootia32
#
################################################################################

RECALBOX_BOOTIA32_VERSION = 88f4100e9726925f947e7a09c6541167b39d2839
RECALBOX_BOOTIA32_SITE = $(call github,hirotakaster,baytail-bootia32.efi,$(RECALBOX_BOOTIA32_VERSION))
RECALBOX_BOOTIA32_LICENSE = COPYRIGHT
RECALBOX_BOOTIA32_NON_COMMERCIAL = y

RECALBOX_BOOTIA32_DEPENDENCIES = grub2

define RECALBOX_BOOTIA32_BINARIES_INSTALLATION
	cp $(@D)/bootia32.efi $(BINARIES_DIR)
endef

RECALBOX_BOOTIA32_INSTALL_TARGET_CMDS += $(RECALBOX_BOOTIA32_BINARIES_INSTALLATION)

$(eval $(generic-package))
