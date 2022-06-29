################################################################################
#
# RECALBOX_VOLUMED
#
################################################################################

RECALBOX_VOLUMED_VERSION = 0dc27498eb4e5503921128367fe86b103a983f2c
RECALBOX_VOLUMED_SITE = https://gitlab.com/recalbox/recalbox-volumed.git
RECALBOX_VOLUMED_SITE_METHOD = git
RECALBOX_VOLUMED_LICENSE = GPL-3.0
RECALBOX_VOLUMED_LICENSE_FILES = LICENSE
RECALBOX_VOLUMED_DEPENDENCIES = pulseaudio
RECALBOX_VOLUMED_AUTORECONF = YES

$(eval $(autotools-package))
