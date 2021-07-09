################################################################################
#
# Recalbox bluetooth scripts
#
################################################################################

RECALBOX_BLUETOOTH_SOURCE = 
RECALBOX_BLUETOOTH_VERSION = 1.0

RECALBOX_BLUETOOTH_SCRIPTS_FILES = bluetoothcontrollers.sh \
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
		$(INSTALL) -D -m 0755 $(RECALBOX_BLUETOOTH_PKGDIR)/src/scripts/$(script) $(TARGET_DIR)/recalbox/scripts/bluetooth/$(script)
	)
endef

$(eval $(generic-package))
