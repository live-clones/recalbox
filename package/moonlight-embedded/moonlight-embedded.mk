################################################################################
#
# moonlight-embedded
#
################################################################################

MOONLIGHT_EMBEDDED_VERSION = 7c8795febc9fad98084aa10fc5918985cbd8081e
MOONLIGHT_EMBEDDED_SITE = git://github.com/irtimmer/moonlight-embedded.git
MOONLIGHT_EMBEDDED_GIT_SUBMODULES=y
MOONLIGHT_EMBEDDED_DEPENDENCIES = opus expat libevdev avahi alsa-lib udev libcurl libcec ffmpeg sdl2 libenet pulseaudio
MOONLIGHT_EMBEDDED_LICENSE = GPL-1.0
MOONLIGHT_EMBEDDED_LICENSE_FILES = LICENSE

MOONLIGHT_EMBEDDED_CONF_OPTS = "-DCMAKE_INSTALL_SYSCONFDIR=/etc"

ifeq ($(BR2_PACKAGE_LIBAMCODEC),y)
MOONLIGHT_EMBEDDED_DEPENDENCIES += libamcodec
endif

ifeq ($(BR2_PACKAGE_RPI_FIRMWARE),y)
MOONLIGHT_EMBEDDED_DEPENDENCIES += rpi-firmware
endif

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI4),y)
define MOONLIGHT_EMBEDDED_CUSTOM_CONFIG_HOOK
	$(INSTALL) -D $(MOONLIGHT_EMBEDDED_PKGDIR)/moonlight-rpi4.conf $(TARGET_DIR)/recalbox/share_init/system/configs/moonlight/moonlight-board.conf
endef
else
define MOONLIGHT_EMBEDDED_CUSTOM_CONFIG_HOOK
	$(INSTALL) -D $(MOONLIGHT_EMBEDDED_PKGDIR)/moonlight-other.conf $(TARGET_DIR)/recalbox/share_init/system/configs/moonlight/moonlight-board.conf
endef
endif

MOONLIGHT_EMBEDDED_POST_INSTALL_TARGET_HOOKS += MOONLIGHT_EMBEDDED_CUSTOM_CONFIG_HOOK

$(eval $(cmake-package))
