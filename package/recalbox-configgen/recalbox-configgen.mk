################################################################################
#
# recalbox configgen version
#
################################################################################

RECALBOX_CONFIGGEN_VERSION = d4a6d1a2bf2504f27f365452712a58210056df0c
RECALBOX_CONFIGGEN_SITE = https://gitlab.com/recalbox/recalbox-configgen.git
RECALBOX_CONFIGGEN_SITE_METHOD = git

RECALBOX_CONFIGGEN_LICENSE = GPLv2
RECALBOX_CONFIGGEN_DEPENDENCIES = python3 python-pyudev python-pygame

RECALBOX_CONFIGGEN_SETUP_TYPE = distutils

define RECALBOX_CONFIGGEN_CREATE_PYC_FILE
	$(PYTHON3_FIX_TIME)
	PYTHONPATH="$(PYTHON3_PATH)" \
	$(HOST_DIR)/bin/python$(PYTHON3_VERSION_MAJOR) \
		$(TOPDIR)/support/scripts/pycompile.py \
		$(TARGET_DIR)/usr/bin/
endef

RECALBOX_CONFIGGEN_POST_INSTALL_TARGET_HOOKS += RECALBOX_CONFIGGEN_CREATE_PYC_FILE

$(eval $(python-package))
