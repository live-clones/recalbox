################################################################################
#
# recalbox-wpaf
#
################################################################################

RECALBOX_WPAF_VERSION = 8a5407b4f2438405380271ec73d287a91b4bc378
RECALBOX_WPAF_SITE = https://gitlab.com/recalbox/recalbox-wpaf
RECALBOX_WPAF_SITE_METHOD = git
RECALBOX_WPAF_DEPENDENCIES += bcm2835
RECALBOX_WPAF_LICENSE = GPL-3.0
RECALBOX_WPAF_LICENSE_FILES = COPYING
RECALBOX_WPAF_AUTORECONF += YES

define RECALBOX_WPAF_INSTALL_INIT_SCRIPT
	$(INSTALL) -D -m 0755 $(RECALBOX_WPAF_PKGDIR)/wpaf.init $(TARGET_DIR)/etc/init.d/S15wpaf
endef

RECALBOX_WPAF_POST_INSTALL_TARGET_HOOKS += RECALBOX_WPAF_INSTALL_INIT_SCRIPT

$(eval $(autotools-package))
