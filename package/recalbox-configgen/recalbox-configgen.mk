################################################################################
#
# recalbox configgen version https://gitlab.com/recalbox/recalbox-configgen
#
################################################################################

RECALBOX_CONFIGGEN_VERSION = 8bccfbe9fe07e4f8f76c9f0fce95853bf39d51bb

RECALBOX_CONFIGGEN_SITE = https://gitlab.com/recalbox/recalbox-configgen.git
RECALBOX_CONFIGGEN_SITE_METHOD = git

RECALBOX_CONFIGGEN_LICENSE = GPL2
RECALBOX_CONFIGGEN_DEPENDENCIES = python

RECALBOX_CONFIGGEN_SETUP_TYPE = distutils

$(eval $(python-package))
