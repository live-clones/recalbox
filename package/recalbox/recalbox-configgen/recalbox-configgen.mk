################################################################################
#
# recalbox configgen version https://github.com/digitalLumberjack/recalbox-configgen
#
################################################################################

RECALBOX_CONFIGGEN_VERSION = 09d031505a27e90f48b968adfb5006a9203bf7ba

RECALBOX_CONFIGGEN_SITE = https://gitlab.com/recalbox/recalbox-configgen.git
RECALBOX_CONFIGGEN_SITE_METHOD = git

RECALBOX_CONFIGGEN_LICENSE = GPL2
RECALBOX_CONFIGGEN_DEPENDENCIES = python

RECALBOX_CONFIGGEN_SETUP_TYPE = distutils

$(eval $(python-package))
