################################################################################
#
# ResidualVM
#
################################################################################

RESIDUALVM_VERSION = a98cca7f2263632e82e364fee6edbd983b169a70
RESIDUALVM_REPO = residualvm

RESIDUALVM_SITE = $(call github,$(RESIDUALVM_REPO),residualvm,$(RESIDUALVM_VERSION))

RESIDUALVM_LICENSE = GPL2
RESIDUALVM_DEPENDENCIES = sdl2 zlib jpeg libmpeg2 libogg libvorbis flac libmad libpng libtheora \
	faad2 fluidsynth freetype 

RESIDUALVM_ADDITIONAL_FLAGS= -I$(STAGING_DIR)/usr/include -I$(STAGING_DIR)/usr/include/interface/vcos/pthreads -I$(STAGING_DIR)/usr/include/interface/vmcs_host/linux -lpthread -lm -L$(STAGING_DIR)/usr/lib -lGLESv2 -lEGL

ifeq ($(BR2_PACKAGE_RPI_USERLAND),y)
        RESIDUALVM_ADDITIONAL_FLAGS += -lbcm_host -lvchostif
        RESIDUALVM_CONF_OPTS += --host=raspberrypi
endif

RESIDUALVM_CONF_ENV += RANLIB="$(TARGET_RANLIB)" STRIP="$(TARGET_STRIP)" AR="$(TARGET_AR) cru" AS="$(TARGET_AS)"
RESIDUALVM_CONF_OPTS += --disable-debug --enable-optimizations --enable-flac --enable-mad --enable-vorbis --disable-tremor \
                --disable-fluidsynth --disable-taskbar --disable-timidity --disable-alsa --enable-vkeybd --enable-keymapper \
                --prefix=/usr --with-sdl-prefix="$(STAGING_DIR)/usr/bin/" --with-freetype2-prefix="$(STAGING_DIR)/usr/bin/" --enable-release \
		--enable-opengl-shaders


RESIDUALVM_MAKE_OPTS += RANLIB="$(TARGET_RANLIB)" STRIP="$(TARGET_STRIP)" AR="$(TARGET_AR) cru" AS="$(TARGET_AS)" LD="$(TARGET_CXX)"

define RESIDUALVM_ADD_VIRTUAL_KEYBOARD
	cp $(@D)/backends/vkeybd/packs/vkeybd_default.zip $(TARGET_DIR)/usr/share/residualvm
	cp $(@D)/backends/vkeybd/packs/vkeybd_small.zip $(TARGET_DIR)/usr/share/residualvm
endef

RESIDUALVM_POST_INSTALL_TARGET_HOOKS += RESIDUALVM_ADD_VIRTUAL_KEYBOARD

$(eval $(autotools-package))
