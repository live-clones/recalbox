################################################################################
#
# recalbox configgen version
#
################################################################################

RECALBOX_CONFIGGEN_VERSION = cf1b50bc07ee8db7667f3e2c7a3e9e458ebd9a4c

RECALBOX_CONFIGGEN_SITE = https://gitlab.com/recalbox/recalbox-configgen.git
RECALBOX_CONFIGGEN_SITE_METHOD = git

RECALBOX_CONFIGGEN_LICENSE = GPL2
RECALBOX_CONFIGGEN_DEPENDENCIES = python

RECALBOX_CONFIGGEN_SETUP_TYPE = distutils

$(eval $(python-package))