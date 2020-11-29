################################################################################
#
# recalbox configgen version
#
################################################################################

RECALBOX_CONFIGGEN_VERSION = 6a5e90151fa8e69eff324b8388faf836746b3a59

RECALBOX_CONFIGGEN_SITE = https://gitlab.com/recalbox/recalbox-configgen.git
RECALBOX_CONFIGGEN_SITE_METHOD = git

RECALBOX_CONFIGGEN_LICENSE = GPLv2
RECALBOX_CONFIGGEN_DEPENDENCIES = python

RECALBOX_CONFIGGEN_SETUP_TYPE = distutils

$(eval $(python-package))
