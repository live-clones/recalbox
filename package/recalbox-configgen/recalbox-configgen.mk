################################################################################
#
# recalbox configgen version
#
################################################################################

RECALBOX_CONFIGGEN_VERSION = e633d52a84cc9c63983a9cac00499b418c411eae
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
