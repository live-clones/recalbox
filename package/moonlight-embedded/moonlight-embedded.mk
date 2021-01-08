################################################################################
#
# moonlight-embedded
#
################################################################################

MOONLIGHT_EMBEDDED_VERSION = v2.4.11
MOONLIGHT_EMBEDDED_SITE = git://github.com/irtimmer/moonlight-embedded.git
MOONLIGHT_EMBEDDED_GIT_SUBMODULES=y
MOONLIGHT_EMBEDDED_DEPENDENCIES = opus expat libevdev avahi alsa-lib udev libcurl libcec ffmpeg sdl2 libenet
MOONLIGHT_EMBEDDED_LICENSE = GPL-1.0
MOONLIGHT_EMBEDDED_LICENSE_FILES = LICENSE

MOONLIGHT_EMBEDDED_CONF_OPTS = "-DCMAKE_INSTALL_SYSCONFDIR=/etc"

ifeq ($(BR2_PACKAGE_LIBAMCODEC),y)
MOONLIGHT_EMBEDDED_DEPENDENCIES += libamcodec
endif

ifeq ($(BR2_PACKAGE_RPI_FIRMWARE),y)
MOONLIGHT_EMBEDDED_DEPENDENCIES += rpi-firmware
endif

ifeq ($(BR2_PACKAGE_RPI_USERLAND_TOOLS),y)
MOONLIGHT_EMBEDDED_DEPENDENCIES += rpi-userland-tools
endif

$(eval $(cmake-package))
