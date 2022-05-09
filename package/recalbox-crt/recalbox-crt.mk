################################################################################
#
# recalbox crt
#
################################################################################

RECALBOX_CRT_VERSION = 1
RECALBOX_CRT_LICENSE = MIT

RECALBOX_CRT_SYSTEM_FILE = systems.txt
ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3)$(BR2_PACKAGE_RECALBOX_TARGET_RPIZERO2),y)
RECALBOX_CRT_SYSTEM_FILE = systems.rpi3.txt
endif

define RECALBOX_CRT_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 644 $(RECALBOX_CRT_PKGDIR)/crt-config/empty-file.txt $(BINARIES_DIR)/rpi-firmware/crt/recalbox-crt-config.txt
	$(INSTALL) -D -m 644 $(RECALBOX_CRT_PKGDIR)/crt-config/empty-file.txt $(BINARIES_DIR)/rpi-firmware/crt/recalbox-crt-options.cfg
	$(INSTALL) -D -m 0744 $(RECALBOX_CRT_PKGDIR)/S13crt $(TARGET_DIR)/etc/init.d/
	mkdir -p $(TARGET_DIR)/recalbox/system/configs/crt/
	mkdir -p $(TARGET_DIR)/recalbox/share_init/system/configs/crt/
	echo "Place your extra CRT configurations in this directory." > $(TARGET_DIR)/recalbox/share_init/system/configs/crt/README.txt
	$(INSTALL) -D -m 644 $(RECALBOX_CRT_PKGDIR)/crt-config/modes.txt $(TARGET_DIR)/recalbox/system/configs/crt/
	$(INSTALL) -D -m 644 $(RECALBOX_CRT_PKGDIR)/crt-config/$(RECALBOX_CRT_SYSTEM_FILE) $(TARGET_DIR)/recalbox/system/configs/crt/systems.txt
	$(INSTALL) -D -m 644 $(RECALBOX_CRT_PKGDIR)/crt-config/arcade_games.txt $(TARGET_DIR)/recalbox/system/configs/crt/
endef

$(eval $(virtual-package))
