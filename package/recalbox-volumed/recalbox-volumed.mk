################################################################################
#
# RECALBOX_VOLUMED
#
################################################################################

RECALBOX_VOLUMED_VERSION = 0bcad43e36b5e6bc8a0963fc3ef8d842d20b18f5
RECALBOX_VOLUMED_SITE = https://gitlab.com/recalbox/recalbox-volumed.git
RECALBOX_VOLUMED_SITE_METHOD = git
RECALBOX_VOLUMED_LICENSE = GPL-3.0
RECALBOX_VOLUMED_LICENSE_FILES = LICENSE
RECALBOX_VOLUMED_DEPENDENCIES = pulseaudio
RECALBOX_VOLUMED_AUTORECONF = YES

$(eval $(autotools-package))
