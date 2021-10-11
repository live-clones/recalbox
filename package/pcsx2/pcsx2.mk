################################################################################
#
# PCSX2
#
################################################################################

PCSX2_VERSION = efed4a07ef43844a5f9d76fa88a7c5d9b7d38568
PCSX2_SITE = https://github.com/pcsx2/pcsx2.git
PCSX2_LICENSE = GPLv2 GPLv3 LGPLv2.1 LGPLv3
PCSX2_DEPENDENCIES = xserver_xorg-server alsa-lib freetype zlib libpng wxwidgets libaio portaudio libsoundtouch sdl2 libpcap yaml-cpp libsamplerate fmt
PCSX2_LICENSE_FILES = COPYING.GPLv2

PCSX2_SITE_METHOD = git
PCSX2_GIT_SUBMODULES = YES

PCSX2_CONF_OPTS += -DBUILD_SHARED_LIBS=ON
PCSX2_CONF_OPTS += -DCMAKE_BUILD_TYPE=Release
PCSX2_CONF_OPTS += -DDISABLE_ADVANCE_SIMD=ON
PCSX2_CONF_OPTS += -DDISABLE_PCSX2_WRAPPER=1
PCSX2_CONF_OPTS += -DENABLE_TESTS=OFF
PCSX2_CONF_OPTS += -DEXTRA_PLUGINS=TRUE
PCSX2_CONF_OPTS += -DGTK2_API=TRUE
PCSX2_CONF_OPTS += -DPACKAGE_MODE=FALSE
PCSX2_CONF_OPTS += -DPCSX2_TARGET_ARCHITECTURES=x86_64
PCSX2_CONF_OPTS += -DSDL2_API=TRUE
PCSX2_CONF_OPTS += -DUSE_LTO=TRUE
PCSX2_CONF_OPTS += -DUSE_SYSTEM_YAML=ON
PCSX2_CONF_OPTS += -DUSE_VTUNE=OFF
PCSX2_CONF_OPTS += -DwxWidgets_CONFIG_EXECUTABLE="$(STAGING_DIR)/usr/bin/wx-config"
PCSX2_CONF_OPTS += -DXDG_STD=FALSE

define PCSX2_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D $(@D)/pcsx2/PCSX2 $(TARGET_DIR)/usr/bin/PCSX2/PCSX2
	cp -pr $(@D)/bin/Langs $(TARGET_DIR)/usr/bin/PCSX2
	cp $(@D)/bin/GameIndex.yaml $(TARGET_DIR)/usr/bin/PCSX2
	mkdir -p $(TARGET_DIR)/usr/bin/PCSX2/plugins
	cp -pr $(@D)/plugins/*/*.so $(TARGET_DIR)/usr/bin/PCSX2/plugins
endef

$(eval $(cmake-package))
