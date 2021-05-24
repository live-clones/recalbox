################################################################################
#
# moonlight-qt
#
################################################################################

MOONLIGHT_QT_VERSION = v3.1.3
MOONLIGHT_QT_SITE = https://github.com/moonlight-stream/moonlight-qt.git
MOONLIGHT_QT_SITE_METHOD = git
MOONLIGHT_QT_GIT_SUBMODULES=y
MOONLIGHT_QT_DEPENDENCIES = opus expat libevdev avahi alsa-lib udev libcurl libcec ffmpeg sdl2 libenet \
			qt5quickcontrols2 qt5svg pulseaudio openssl
MOONLIGHT_QT_LICENSE = GPL-3.0
MOONLIGHT_QT_LICENSE_FILES = LICENSE

MOONLIGHT_QT_CONF_OPTS = "CONFIG+=embedded"

ifeq ($(BR2_PACKAGE_HAS_LIBEGL),y)
MOONLIGHT_QT_DEPENDENCIES += libegl
endif

ifeq ($(BR2_PACKAGE_HAS_GLES),y)
MOONLIGHT_QT_DEPENDENCIES += libgles
endif

ifeq ($(BR2_PACKAGE_HAS_GL),y)
MOONLIGHT_QT_DEPENDENCIES += libgl
endif

ifeq ($(BR2_PACKAGE_LIBAMCODEC),y)
MOONLIGHT_QT_DEPENDENCIES += libamcodec
endif

ifeq ($(BR2_PACKAGE_RPI_USERLAND_TOOLS),y)
MOONLIGHT_QT_DEPENDENCIES += rpi-userland-tools
endif

$(eval $(qmake-package))
