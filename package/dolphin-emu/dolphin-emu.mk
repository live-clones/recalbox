################################################################################
#
# DOLPHIN_EMU
#
################################################################################

# Commit of Jul 21, 2022
DOLPHIN_EMU_SITE = https://github.com/dolphin-emu/dolphin.git
DOLPHIN_EMU_VERSION = 57f106d521e1ec65cc0b10fc6f68dcd56fcbc0d8
DOLPHIN_EMU_LICENSE = GPL-2.0+
DOLPHIN_EMU_LICENSE_FILES = license.txt
DOLPHIN_EMU_DEPENDENCIES = libevdev ffmpeg zlib libpng lzo libusb libcurl bluez5_utils hidapi xz host-xz
DOLPHIN_EMU_SUPPORTS_IN_SOURCE_BUILD = NO
DOLPHIN_EMU_SITE_METHOD = git
DOLPHIN_EMU_GIT_SUBMODULES = YES

DOLPHIN_EMU_CONF_OPTS += -DDISTRIBUTOR='Recalbox'
DOLPHIN_EMU_CONF_OPTS += -DENABLE_LTO=OFF
DOLPHIN_EMU_CONF_OPTS += -DUSE_DISCORD_PRESENCE=OFF
DOLPHIN_EMU_CONF_OPTS += -DTHREADS_PTHREAD_ARG=OFF
DOLPHIN_EMU_CONF_OPTS += -DBUILD_SHARED_LIBS=OFF
DOLPHIN_EMU_CONF_OPTS += -DUSE_MGBA=OFF

ifeq ($(BR2_PACKAGE_XSERVER_XORG_SERVER),y)
DOLPHIN_EMU_DEPENDENCIES += xserver_xorg-server qt5base
DOLPHIN_EMU_CONF_OPTS += -DENABLE_NOGUI=OFF
DOLPHIN_EMU_CONF_OPTS += -DENABLE_EGL=OFF
endif

$(eval $(cmake-package))
