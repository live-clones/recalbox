################################################################################
#
# Recalbox wifi scripts
#
################################################################################

RECALBOX_WIFI_SOURCE =
RECALBOX_WIFI_VERSION = 1.0
RECALBOX_WIFI_LICENSE = MIT

RECALBOX_WIFI_SCRIPTS_FILES = \
	recalbox-wifi.sh \
	recalbox-start-wifi

define RECALBOX_WIFI_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(RECALBOX_WIFI_PKGDIR)wifi.init $(TARGET_DIR)/etc/init.d/S09wifi
	$(foreach script,$(RECALBOX_WIFI_SCRIPTS_FILES),\
		$(INSTALL) -D -m 0755 $(RECALBOX_WIFI_PKGDIR)$(script) $(TARGET_DIR)/recalbox/scripts/$(script)
	)
	$(INSTALL) -D -m 0644 $(RECALBOX_WIFI_PKGDIR)wifi.udev $(TARGET_DIR)/lib/udev/rules.d/92-wireless.rules
endef

$(eval $(generic-package))
