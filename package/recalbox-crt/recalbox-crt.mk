################################################################################
#
# recalbox crt
#
################################################################################

RECALBOX_CRT_SOURCE =
RECALBOX_CRT_VERSION = 1
RECALBOX_CRT_LICENSE = MIT

define RECALBOX_CRT_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 644 $(RECALBOX_CRT_PKGDIR)/crt-config/empty-file.txt $(BINARIES_DIR)/rpi-firmware/crt/recalbox-crt-config.txt
	$(INSTALL) -D -m 644 $(RECALBOX_CRT_PKGDIR)/crt-config/empty-file.txt $(BINARIES_DIR)/rpi-firmware/crt/recalbox-crt-options.cfg
	$(INSTALL) -D -m 0744 $(RECALBOX_CRT_PKGDIR)/S13crt $(TARGET_DIR)/etc/init.d/
	mkdir -p $(TARGET_DIR)/recalbox/system/configs/crt/
	mkdir -p $(TARGET_DIR)/recalbox/share_init/system/configs/crt/
	echo "Place your extra CRT configurations in this directory." > $(TARGET_DIR)/recalbox/share_init/system/configs/crt/README.txt
	$(INSTALL) -D -m 644 $(RECALBOX_CRT_PKGDIR)/crt-config/modes.txt $(TARGET_DIR)/recalbox/system/configs/crt/
	$(INSTALL) -D -m 644 $(RECALBOX_CRT_PKGDIR)/crt-config/systems.txt $(TARGET_DIR)/recalbox/system/configs/crt/
	$(INSTALL) -D -m 644 $(RECALBOX_CRT_PKGDIR)/crt-config/arcade_games.txt $(TARGET_DIR)/recalbox/system/configs/crt/
endef

$(eval $(generic-package))
