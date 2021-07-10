################################################################################
#
# Recalbox bluetooth scripts
#
################################################################################

RECALBOX_BLUETOOTH_SOURCE =
RECALBOX_BLUETOOTH_VERSION = 1.0
RECALBOX_BLUETOOTH_LICENSE = MIT

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI0)$(BR2_PACKAGE_RECALBOX_TARGET_RPI1)$(BR2_PACKAGE_RECALBOX_TARGET_RPI2)$(BR2_PACKAGE_RECALBOX_TARGET_RPI3)$(BR2_PACKAGE_RECALBOX_TARGET_RPI4),y)
RECALBOX_BLUETOOTH_BT_CONTROLLERS=rpi
else
RECALBOX_BLUETOOTH_BT_CONTROLLERS=others
endif

RECALBOX_BLUETOOTH_SCRIPTS_FILES = \
	bluezutils.py \
	btDaemon \
	eslist.sh \
	recalpair \
	simple-agent \
	test-device \
	test-discovery
define RECALBOX_BLUETOOTH_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(RECALBOX_BLUETOOTH_PKGDIR)src/init/bluetooth $(TARGET_DIR)/etc/init.d/S07bluetooth
	$(foreach script,$(RECALBOX_BLUETOOTH_SCRIPTS_FILES),\
		$(INSTALL) -D -m 0755 $(RECALBOX_BLUETOOTH_PKGDIR)src/scripts/$(script) $(TARGET_DIR)/recalbox/scripts/bluetooth/$(script)
	)
	$(INSTALL) -D -m 0755 $(RECALBOX_BLUETOOTH_PKGDIR)src/scripts/bluetoothcontrollers.sh.$(RECALBOX_BLUETOOTH_BT_CONTROLLERS) \
		$(TARGET_DIR)/recalbox/scripts/bluetooth/bluetoothcontrollers.sh
endef

$(eval $(generic-package))
