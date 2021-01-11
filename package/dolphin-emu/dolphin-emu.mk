################################################################################
#
# DOLPHIN_EMU
#
################################################################################

# Version: 5.0-13178
DOLPHIN_EMU_VERSION = a34823df61df65168aa40ef5e82e44defd4a0138
DOLPHIN_EMU_SITE = $(call github,dolphin-emu,dolphin,$(DOLPHIN_EMU_VERSION))
DOLPHIN_EMU_LICENSE = GPL-2.0+
DOLPHIN_EMU_LICENSE_FILES = license.txt
DOLPHIN_EMU_DEPENDENCIES = libevdev ffmpeg zlib libpng lzo libusb libcurl bluez5_utils hidapi xz host-xz xserver_xorg-server qt5base
DOLPHIN_EMU_SUPPORTS_IN_SOURCE_BUILD = NO

DOLPHIN_EMU_CONF_OPTS += -DDISTRIBUTOR='Recalbox'
DOLPHIN_EMU_CONF_OPTS += -DENABLE_LTO=OFF
DOLPHIN_EMU_CONF_OPTS += -DENABLE_EGL=OFF
DOLPHIN_EMU_CONF_OPTS += -DENABLE_NOGUI=OFF
DOLPHIN_EMU_CONF_OPTS += -DUSE_DISCORD_PRESENCE=OFF
DOLPHIN_EMU_CONF_OPTS += -DTHREADS_PTHREAD_ARG=OFF
DOLPHIN_EMU_CONF_OPTS += -DBUILD_SHARED_LIBS=OFF

$(eval $(cmake-package))
