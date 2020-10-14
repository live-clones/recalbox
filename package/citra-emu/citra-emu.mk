################################################################################
#
# CITRA_EMU
#
################################################################################

##on version "nightly-1549"
CITRA_EMU_VERSION = c4ba7b362696ff424044856974d9ae8e2263dd73
CITRA_EMU_SITE = https://github.com/citra-emu/citra.git
CITRA_EMU_SITE_METHOD = git
CITRA_EMU_GIT_SUBMODULES = YES
CITRA_EMU_LICENSE = GPLv2
CITRA_EMU_DEPENDENCIES = fmt ffmpeg boost qt5base qt5tools qt5multimedia
CITRA_EMU_SUPPORTS_IN_SOURCE_BUILD = NO

CITRA_EMU_CONF_OPTS += -DENABLE_QT=ON
CITRA_EMU_CONF_OPTS += -DENABLE_CUBEB=OFF
CITRA_EMU_CONF_OPTS += -DENABLE_SDL2=ON
CITRA_EMU_CONF_OPTS += -DENABLE_WEB_SERVICE=ON
CITRA_EMU_CONF_OPTS += -DENABLE_QT_TRANSLATION=ON
CITRA_EMU_CONF_OPTS += -DENABLE_FFMPEG_AUDIO_DECODER=ON
CITRA_EMU_CONF_OPTS += -DENABLE_FFMPEG_VIDEO_DUMPER=ON
CITRA_EMU_CONF_OPTS += -DCMAKE_BUILD_TYPE=Release
CITRA_EMU_CONF_ENV += LDFLAGS=-lpthread

define CITRA_EMU_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/usr/bin
	mkdir -p $(TARGET_DIR)/usr/lib
	$(INSTALL) -D $(@D)/buildroot-build/bin/citra-qt \
		$(TARGET_DIR)/usr/bin/citra
	$(INSTALL) -D $(@D)/buildroot-build/bin/citra-room \
		$(TARGET_DIR)/usr/bin/citra-room
	cp -pr $(@D)/buildroot-build/externals/inih/*.so \
		$(TARGET_DIR)/usr/lib/
	cp -pr $(@D)/buildroot-build/externals/dynarmic/src/*.so \
		$(TARGET_DIR)/usr/lib/
	cp -pr $(@D)/buildroot-build/externals/teakra/src/*.so \
		$(TARGET_DIR)/usr/lib/
	cp -pr $(@D)/buildroot-build/externals/libressl/tls/*.so \
		$(TARGET_DIR)/usr/lib/
	cp -pr $(@D)/buildroot-build/externals/libressl/ssl/*.so \
		$(TARGET_DIR)/usr/lib/
	cp -pr $(@D)/buildroot-build/externals/libressl/crypto/*.so \
		$(TARGET_DIR)/usr/lib/
	cp -pr $(@D)/buildroot-build/externals/lurlparser/*.so \
		$(TARGET_DIR)/usr/lib/
	cp -pr $(@D)/buildroot-build/externals/fmt/*.so \
		$(TARGET_DIR)/usr/lib/
	cp -pr $(@D)/buildroot-build/externals/lodepng/*.so \
		$(TARGET_DIR)/usr/lib/
endef

$(eval $(cmake-package))
