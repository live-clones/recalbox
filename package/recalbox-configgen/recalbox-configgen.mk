################################################################################
#
# recalbox configgen version https://github.com/digitalLumberjack/recalbox-configgen
#
################################################################################

RECALBOX_CONFIGGEN_VERSION = 49ba41579639df1c328f9f1dc19a50c1e801c5d2

RECALBOX_CONFIGGEN_SITE = $(call github,recalbox,recalbox-configgen,$(RECALBOX_CONFIGGEN_VERSION))

RECALBOX_CONFIGGEN_LICENSE = GPL2
RECALBOX_CONFIGGEN_DEPENDENCIES = python

RECALBOX_CONFIGGEN_SETUP_TYPE = distutils

$(eval $(python-package))
