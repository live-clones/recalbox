################################################################################
#
# RECALBOX_VOLUMED
#
################################################################################

RECALBOX_VOLUMED_VERSION = 2cf5a6c6e64bc0434c837addceb2d13f9bf245ff
RECALBOX_VOLUMED_SITE = https://gitlab.com/recalbox/recalbox-volumed.git
RECALBOX_VOLUMED_SITE_METHOD = git
RECALBOX_VOLUMED_LICENSE = GPL-3.0
RECALBOX_VOLUMED_LICENSE_FILES = LICENSE
RECALBOX_VOLUMED_DEPENDENCIES = pulseaudio
RECALBOX_VOLUMED_AUTORECONF = YES

$(eval $(autotools-package))
