################################################################################
#
# CITRA_EMU
#
################################################################################

# Version nightly-1661
CITRA_EMU_VERSION = 94013c8e00ccf1bde9d02c2d86630e0df900c8e3
CITRA_EMU_SITE = https://github.com/citra-emu/citra.git
CITRA_EMU_SITE_METHOD = git
CITRA_EMU_GIT_SUBMODULES = YES
CITRA_EMU_LICENSE = GPLv2
CITRA_EMU_DEPENDENCIES = sdl2 fmt ffmpeg boost qt5base qt5tools qt5multimedia
CITRA_EMU_SUPPORTS_IN_SOURCE_BUILD = NO

CITRA_EMU_CONF_OPTS += -DENABLE_QT=ON
CITRA_EMU_CONF_OPTS += -DENABLE_CUBEB=OFF
CITRA_EMU_CONF_OPTS += -DENABLE_SDL2=ON
CITRA_EMU_CONF_OPTS += -DENABLE_WEB_SERVICE=ON
CITRA_EMU_CONF_OPTS += -DENABLE_QT_TRANSLATION=ON
CITRA_EMU_CONF_OPTS += -DENABLE_FFMPEG=ON
CITRA_EMU_CONF_OPTS += -DCMAKE_BUILD_TYPE=Release
CITRA_EMU_CONF_OPTS += -DBUILD_SHARED_LIBS=FALSE
CITRA_EMU_CONF_ENV += LDFLAGS=-lpthread

define CITRA_EMU_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/bin
	mkdir -p $(TARGET_DIR)/usr/lib
	$(INSTALL) -D $(@D)/buildroot-build/bin/Release/citra-qt \
		$(TARGET_DIR)/usr/bin/citra
	$(INSTALL) -D $(@D)/buildroot-build/bin/Release/citra-room \
		$(TARGET_DIR)/usr/bin/citra-room
endef

$(eval $(cmake-package))
