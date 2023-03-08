################################################################################
#
# recalbox configgen version
#
################################################################################

RECALBOX_CONFIGGEN_VERSION = custom
RECALBOX_CONFIGGEN_SITE = $(TOPDIR)/../projects/configgen
RECALBOX_CONFIGGEN_SITE_METHOD = local

RECALBOX_CONFIGGEN_LICENSE = GPLv2
RECALBOX_CONFIGGEN_DEPENDENCIES = python3 python-pyudev python-pygame

RECALBOX_CONFIGGEN_SETUP_TYPE = distutils

define RECALBOX_CONFIGGEN_CREATE_PYC_FILE
	$(PYTHON3_FIX_TIME)
	PYTHONPATH="$(PYTHON3_PATH)" \
	$(HOST_DIR)/bin/python$(PYTHON3_VERSION_MAJOR) \
		$(PYTHON3_DIR)/Lib/compileall.py \
		-s $(TARGET_DIR) \
		-p / \
		-b \
		$(TARGET_DIR)/usr/lib/python$(PYTHON3_VERSION_MAJOR)/site-packages/configgen/
endef

RECALBOX_CONFIGGEN_POST_INSTALL_TARGET_HOOKS += RECALBOX_CONFIGGEN_CREATE_PYC_FILE

$(eval $(python-package))
