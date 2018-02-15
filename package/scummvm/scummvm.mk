################################################################################
#
# Scummvm
#
################################################################################

SCUMMVM_VERSION = fae5ccd1ed7100478a23f3afc41ea935d2ee7abc
SCUMMVM_REPO = scummvm
SCUMMVM_SITE = $(call github,$(SCUMMVM_REPO),scummvm,$(SCUMMVM_VERSION))
SCUMMVM_LICENSE = GPL2
SCUMMVM_DEPENDENCIES = sdl2 zlib jpeg-turbo libmpeg2 libogg libvorbis flac libmad libpng libtheora faad2 fluidsynth freetype 

SCUMMVM_ADDITIONAL_FLAGS= -I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/usr/include/interface/vcos/pthreads -I$(STAGING_DIR)/usr/include/interface/vmcs_host/linux -lpthread -lm -L$(STAGING_DIR)/usr/lib -lGLESv2 -lEGL

ifeq ($(BR2_PACKAGE_RPI_USERLAND),y)
	SCUMMVM_ADDITIONAL_FLAGS += -lbcm_host -lvchostif
	SCUMMVM_CONF_OPTS += --host=raspberrypi
endif

SCUMMVM_CONF_ENV += RANLIB="$(TARGET_RANLIB)" STRIP="$(TARGET_STRIP)" AR="$(TARGET_AR) cru" AS="$(TARGET_AS)"
SCUMMVM_CONF_OPTS += --enable-opengl --disable-debug --enable-optimizations --enable-mt32emu --enable-flac --enable-mad --enable-vorbis --disable-tremor \
		--enable-fluidsynth --disable-taskbar --disable-timidity --disable-alsa --enable-vkeybd --enable-keymapper --disable-eventrecorder \
		--prefix=/usr --with-sdl-prefix="$(STAGING_DIR)/usr/bin/" --with-freetype2-prefix="$(STAGING_DIR)/usr/bin/" \
		--enable-all-engines --enable-release

SCUMMVM_MAKE_OPTS += RANLIB="$(TARGET_RANLIB)" STRIP="$(TARGET_STRIP)" AR="$(TARGET_AR) cru" AS="$(TARGET_AS)" LD="$(TARGET_CXX)"

define SCUMMVM_ADD_VIRTUAL_KEYBOARD
	cp $(@D)/backends/vkeybd/packs/vkeybd_default.zip $(TARGET_DIR)/usr/share/scummvm
	cp $(@D)/backends/vkeybd/packs/vkeybd_small.zip $(TARGET_DIR)/usr/share/scummvm
endef

SCUMMVM_POST_INSTALL_TARGET_HOOKS += SCUMMVM_ADD_VIRTUAL_KEYBOARD

$(eval $(autotools-package))
