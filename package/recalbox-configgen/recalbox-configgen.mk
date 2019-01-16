################################################################################
#
# recalbox configgen version https://gitlab.com/recalbox/recalbox-configgen
#
################################################################################

RECALBOX_CONFIGGEN_VERSION = ff05d21daea0217327ab84df36c72b9c4d447dc9

RECALBOX_CONFIGGEN_SITE = https://gitlab.com/recalbox/recalbox-configgen.git
RECALBOX_CONFIGGEN_SITE_METHOD = git

RECALBOX_CONFIGGEN_LICENSE = GPL2
RECALBOX_CONFIGGEN_DEPENDENCIES = python

RECALBOX_CONFIGGEN_SETUP_TYPE = distutils

$(eval $(python-package))