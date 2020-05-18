################################################################################
#
# recalbox configgen version
#
################################################################################

RECALBOX_CONFIGGEN_VERSION = a7b5b627612a1ec4443d3a04791789f9dc5c2aa7

RECALBOX_CONFIGGEN_SITE = https://gitlab.com/recalbox/recalbox-configgen.git
RECALBOX_CONFIGGEN_SITE_METHOD = git

RECALBOX_CONFIGGEN_LICENSE = GPLv2
RECALBOX_CONFIGGEN_DEPENDENCIES = python

RECALBOX_CONFIGGEN_SETUP_TYPE = distutils

$(eval $(python-package))
