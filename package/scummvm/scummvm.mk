################################################################################
#
# ScummVM
#
################################################################################

#Release version 2.4.0
SCUMMVM_VERSION = dffa1971c8fb932939ffafe5e8ed53549ebf6992
SCUMMVM_SITE = $(call github,scummvm,scummvm,$(SCUMMVM_VERSION))
SCUMMVM_LICENSE = GPL2
SCUMMVM_LICENSE_FILES = COPYING
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
		--enable-all-engines --enable-release --enable-opengl-shaders

SCUMMVM_MAKE_OPTS += RANLIB="$(TARGET_RANLIB)" STRIP="$(TARGET_STRIP)" AR="$(TARGET_AR) cru" AS="$(TARGET_AS)" LD="$(TARGET_CXX)"

define SCUMMVM_ADD_VIRTUAL_KEYBOARD
	cp $(@D)/backends/vkeybd/packs/vkeybd_default.zip $(TARGET_DIR)/usr/share/scummvm
	cp $(@D)/backends/vkeybd/packs/vkeybd_small.zip $(TARGET_DIR)/usr/share/scummvm
endef
define SCUMMVM_CONTROLLERS_LINK
	ln -fs /tmp/gamecontrollerdb.txt $(TARGET_DIR)/usr/share/scummvm
endef

SCUMMVM_POST_INSTALL_TARGET_HOOKS += SCUMMVM_ADD_VIRTUAL_KEYBOARD
SCUMMVM_POST_INSTALL_TARGET_HOOKS += SCUMMVM_CONTROLLERS_LINK

$(eval $(autotools-package))
