################################################################################
#
# recalbox configgen version https://github.com/digitalLumberjack/recalbox-configgen
#
################################################################################

RECALBOX_CONFIGGEN_VERSION = f348e77472f06130dcda3d7b8763f0ecda1047ce

RECALBOX_CONFIGGEN_SITE = https://gitlab.com/recalbox/recalbox-configgen.git
RECALBOX_CONFIGGEN_SITE_METHOD = git

RECALBOX_CONFIGGEN_LICENSE = GPL2
RECALBOX_CONFIGGEN_DEPENDENCIES = python

RECALBOX_CONFIGGEN_SETUP_TYPE = distutils

$(eval $(python-package))
