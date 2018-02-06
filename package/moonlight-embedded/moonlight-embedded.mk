################################################################################
#
# moonlight-embedded
#
################################################################################

MOONLIGHT_EMBEDDED_VERSION = v2.4.6
MOONLIGHT_EMBEDDED_SITE = git://github.com/irtimmer/moonlight-embedded.git
MOONLIGHT_EMBEDDED_GIT_SUBMODULES=y
MOONLIGHT_EMBEDDED_DEPENDENCIES = opus expat libevdev avahi alsa-lib udev libcurl libcec ffmpeg sdl2 libenet

MOONLIGHT_EMBEDDED_CONF_OPTS = "-DCMAKE_INSTALL_SYSCONFDIR=/etc"

ifeq ($(BR2_PACKAGE_LIBAMCODEC),y)
	MOONLIGHT_EMBEDDED_DEPENDENCIES += libamcodec
endif

ifeq ($(BR2_PACKAGE_RPI_FIRMWARE),y)
	MOONLIGHT_EMBEDDED_DEPENDENCIES += rpi-firmware
endif

$(eval $(cmake-package))
