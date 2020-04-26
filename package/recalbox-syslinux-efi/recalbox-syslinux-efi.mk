################################################################################
#
# recalbox-syslinux-efi
#
################################################################################

RECALBOX_SYSLINUX_EFI_VERSION = 6.04.pre2.r11.gbf6db5b4-2
RECALBOX_SYSLINUX_EFI_SOURCE =
RECALBOX_SYSLINUX_EFI_SITE = binaries

define RECALBOX_SYSLINUX_EFI_EXTRACT_CMDS
	cp -R $(BR2_EXTERNAL_RECALBOX_PATH)/package/recalbox-syslinux-efi/binaries/* $(@D)
endef

define RECALBOX_SYSLINUX_EFI_INSTALL_TARGET_CMDS
	mkdir -p $(BINARIES_DIR)/syslinux/efi64
	cp $(@D)/bootx64.efi  $(BINARIES_DIR)/syslinux/
	cp $(@D)/ldlinux.e64  $(BINARIES_DIR)/syslinux/
	cp $(@D)/bootia32.efi $(BINARIES_DIR)/syslinux/
	cp $(@D)/ldlinux.e32  $(BINARIES_DIR)/syslinux/
	cp $(@D)/menu.c32     $(BINARIES_DIR)/syslinux/efi64
	cp $(@D)/libutil.c32  $(BINARIES_DIR)/syslinux/efi64
endef

$(eval $(generic-package))
