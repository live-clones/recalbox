################################################################################
#
# recalbox configgen version https://github.com/digitalLumberjack/recalbox-configgen
#
################################################################################

RECALBOX_CONFIGGEN_VERSION = 680266f3d64fe0fee6e151048e0e72d0d94eed93

RECALBOX_CONFIGGEN_SITE = https://gitlab.com/recalbox/recalbox-configgen.git
RECALBOX_CONFIGGEN_SITE_METHOD = git

RECALBOX_CONFIGGEN_LICENSE = GPL2
RECALBOX_CONFIGGEN_DEPENDENCIES = python

RECALBOX_CONFIGGEN_SETUP_TYPE = distutils

$(eval $(python-package))
