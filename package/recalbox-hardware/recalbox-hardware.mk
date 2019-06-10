################################################################################
#
# recalbox-hardware
#
################################################################################

RECALBOX_HARDWARE_VERSION = abe9ae0ccb0d1d17a8e4d93215e4eccccf21214a
RECALBOX_HARDWARE_SITE = https://gitlab.com/recalbox/recalbox-hardware
RECALBOX_HARDWARE_SITE_METHOD = git

define RECALBOX_HARDWARE_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/recalbox/system/hardware
	cp -r $(@D)/* \
		$(TARGET_DIR)/recalbox/system/hardware/
	cp -r $(RECALBOX_THEMES_BUILDDIR)/themes/recalbox-gpicase \
		$(TARGET_DIR)/recalbox/system/hardware/case/installers/gpi/assets
endef

$(eval $(generic-package))
