################################################################################
#
# recalbox configgen version https://gitlab.com/recalbox/recalbox-configgen
#
################################################################################

RECALBOX_CONFIGGEN_VERSION = 27a4f7f5908d52aaa9db797705a6f27c1bce19f9

RECALBOX_CONFIGGEN_SITE = https://gitlab.com/recalbox/recalbox-configgen.git
RECALBOX_CONFIGGEN_SITE_METHOD = git

RECALBOX_CONFIGGEN_LICENSE = GPL2
RECALBOX_CONFIGGEN_DEPENDENCIES = python

RECALBOX_CONFIGGEN_SETUP_TYPE = distutils

$(eval $(python-package))
