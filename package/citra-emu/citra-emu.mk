################################################################################
#
# CITRA_EMU
#
################################################################################

CITRA_EMU_VERSION = nightly-1311
CITRA_EMU_SITE = https://github.com/citra-emu/citra-nightly.git
CITRA_EMU_SITE_METHOD = git
CITRA_EMU_GIT_SUBMODULES = YES
CITRA_EMU_LICENSE = GPLv2
CITRA_EMU_DEPENDENCIES = sdl2 fmt ffmpeg boost
CITRA_EMU_SUPPORTS_IN_SOURCE_BUILD = NO

CITRA_EMU_CONF_OPTS += -DENABLE_QT=OFF
CITRA_EMU_CONF_OPTS += -DENABLE_SDL2=ON
CITRA_EMU_CONF_OPTS += -DENABLE_WEB_SERVICE=ON
CITRA_EMU_CONF_OPTS += -DENABLE_FFMPEG=ON
CITRA_EMU_CONF_OPTS += -DCMAKE_BUILD_TYPE=Release
CITRA_EMU_CONF_ENV += LDFLAGS=-lpthread

define CITRA_EMU_INSTALL_TARGET_CMDS
    mkdir -p $(TARGET_DIR)/usr/bin
    mkdir -p $(TARGET_DIR)/usr/lib
	$(INSTALL) -D $(@D)/buildroot-build/bin/citra \
		$(TARGET_DIR)/usr/bin/citra
	$(INSTALL) -D $(@D)/buildroot-build/bin/citra-room \
		$(TARGET_DIR)/usr/bin/citra-room
	cp -pr $(@D)/buildroot-build/externals/inih/*.so \
		$(TARGET_DIR)/usr/lib/
	cp -pr $(@D)/buildroot-build/externals/cubeb/*.so \
		$(TARGET_DIR)/usr/lib/
	cp -pr $(@D)/buildroot-build/externals/dynarmic/src/*.so \
		$(TARGET_DIR)/usr/lib/	
	cp -pr $(@D)/buildroot-build/externals/teakra/src/*.so \
		$(TARGET_DIR)/usr/lib/
endef

$(eval $(cmake-package))