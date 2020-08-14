################################################################################
#
# recalbox configgen version
#
################################################################################

RECALBOX_CONFIGGEN_VERSION = f13e772975dce9aaca20a7fc46c8411979aaa15f

RECALBOX_CONFIGGEN_SITE = https://gitlab.com/recalbox/recalbox-configgen.git
RECALBOX_CONFIGGEN_SITE_METHOD = git

RECALBOX_CONFIGGEN_LICENSE = GPLv2
RECALBOX_CONFIGGEN_DEPENDENCIES = python

RECALBOX_CONFIGGEN_SETUP_TYPE = distutils

$(eval $(python-package))
