################################################################################
#
# recalbox configgen version https://gitlab.com/recalbox/recalbox-configgen
#
################################################################################

RECALBOX_CONFIGGEN_VERSION = 35031acce4be0006690a533b8bc6c6410fbb9ca2

RECALBOX_CONFIGGEN_SITE = https://gitlab.com/recalbox/recalbox-configgen.git
RECALBOX_CONFIGGEN_SITE_METHOD = git

RECALBOX_CONFIGGEN_LICENSE = GPL2
RECALBOX_CONFIGGEN_DEPENDENCIES = python

RECALBOX_CONFIGGEN_SETUP_TYPE = distutils

$(eval $(python-package))
