################################################################################
#
# RECALBOX_RGB_DUAL_GUNCON2
#
################################################################################

RECALBOX_RGB_DUAL_GUNCON2_VERSION = e50b612d7c77879fb3df9402a3eaa88a5526fe3f
RECALBOX_RGB_DUAL_GUNCON2_SITE = https://github.com/beardypig/guncon2
RECALBOX_RGB_DUAL_GUNCON2_SITE_METHOD = git
RECALBOX_RGB_DUAL_GUNCON2_LICENSE = GPLV2
RECALBOX_RGB_DUAL_GUNCON2_DEPENDENCIES = linux

$(eval $(kernel-module))
$(eval $(generic-package))
