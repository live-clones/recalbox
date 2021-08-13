################################################################################
#
# recalbox crt
#
################################################################################

RECALBOX_CRT_VERSION = 1
RECALBOX_CRT_LICENSE = MIT

define RECALBOX_CRT_INSTALL_TARGET_CMDS
	mkdir -p $(BINARIES_DIR)/rpi-firmware/crt-config/
	$(INSTALL) -D -m 644 $(RECALBOX_CRT_PKGDIR)/crt-config/* $(BINARIES_DIR)/rpi-firmware/crt-config/
	$(INSTALL) -D -m 0744 $(RECALBOX_CRT_PKGDIR)/S13crt $(TARGET_DIR)/etc/init.d/
endef

$(eval $(virtual-package))
