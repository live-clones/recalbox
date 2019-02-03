################################################################################
#
# recalbox configgen version https://gitlab.com/recalbox/recalbox-configgen
#
################################################################################

RECALBOX_CONFIGGEN_VERSION = 4d23775d67def583fc03934e1853ff285ed99d3b

RECALBOX_CONFIGGEN_SITE = https://gitlab.com/recalbox/recalbox-configgen.git
RECALBOX_CONFIGGEN_SITE_METHOD = git

RECALBOX_CONFIGGEN_LICENSE = GPL2
RECALBOX_CONFIGGEN_DEPENDENCIES = python

RECALBOX_CONFIGGEN_SETUP_TYPE = distutils

$(eval $(python-package))
