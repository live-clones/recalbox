################################################################################
#
# WILDMIDI
#
################################################################################

WILDMIDI_VERSION = 8f3856e9486d13cd0bd384837fec48fd10a2c854
WILDMIDI_SITE = git://github.com/Mindwerks/wildmidi.git
WILDMIDI_SITE_METHOD = git
WILDMIDI_LICENSE = LGPLv3
WILDMIDI_LICENSE_FILE = COPYING

WILDMIDI_CONF_OPTS += -WANT_PLAYER=OFF -DWANT_ALSA=ON

$(eval $(cmake-package))
