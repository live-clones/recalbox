################################################################################
#
# Fluidsynth
#
################################################################################

FLUIDSYNTH_VERSION = v2.1.2
FLUIDSYNTH_SITE = https://github.com/FluidSynth/fluidsynth.git
FLUIDSYNTH_SITE_METHOD = git
FLUIDSYNTH_INSTALL_STAGING = YES
FLUIDSYNTH_LICENSE = GPL2
FLUIDSYNTH_DEPENDENCIES =

$(eval $(cmake-package))
