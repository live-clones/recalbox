################################################################################
#
# Recalbox TFT assets
#
################################################################################

RECALBOX_TFT_ASSETS_SOURCE=
RECALBOX_TFT_ASSETS_VERSION=1.0

RECALBOX_TFT_ASSETS_LICENSE = COPYRIGHT

define RECALBOX_TFT_ASSETS_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/recalbox/share_init/system
	cp -r $(RECALBOX_TFT_ASSETS_PKGDIR)/tft_logos $(TARGET_DIR)/recalbox/share_init/system/
endef

$(eval $(generic-package))
