################################################################################
#
# recalbox-wpaf
#
################################################################################

RECALBOX_WPAF_VERSION = f7d13e6bba7bc7fdde80cd183b3430bf5a70c970
RECALBOX_WPAF_SITE = https://gitlab.com/recalbox/recalbox-wpaf
RECALBOX_WPAF_SITE_METHOD = git
RECALBOX_WPAF_DEPENDENCIES += wiringpi
RECALBOX_WPAF_AUTORECONF += YES

define RECALBOX_WPAF_INSTALL_INIT_SCRIPT
	$(INSTALL) -D -m 0755 $(RECALBOX_WPAF_PKGDIR)/wpaf.init $(BINARIES_DIR)/etc/init.d/S15wpaf
endef

RECALBOX_WPAF_POST_INSTALL_TARGET_HOOKS += RECALBOX_WPAF_INSTALL_INIT_SCRIPT

$(eval $(autotools-package))
