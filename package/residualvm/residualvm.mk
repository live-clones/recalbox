################################################################################
#
# ResidualVM
#
################################################################################

RESIDUALVM_VERSION = b5d73d4c2296bab033d06da3fdbf9b0c0775bf43
RESIDUALVM_SITE = $(call github,residualvm,residualvm,$(RESIDUALVM_VERSION))
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
	--prefix=/usr --with-sdl-prefix="$(STAGING_DIR)/usr/bin/" --with-freetype2-prefix="$(STAGING_DIR)/usr/bin/" \
	--enable-release --enable-opengl-shaders

RESIDUALVM_MAKE_OPTS += RANLIB="$(TARGET_RANLIB)" STRIP="$(TARGET_STRIP)" AR="$(TARGET_AR) cru" AS="$(TARGET_AS)" LD="$(TARGET_CXX)"

define RESIDUALVM_ADD_VIRTUAL_KEYBOARD
	cp $(@D)/backends/vkeybd/packs/vkeybd_default.zip $(TARGET_DIR)/usr/share/residualvm
	cp $(@D)/backends/vkeybd/packs/vkeybd_small.zip $(TARGET_DIR)/usr/share/residualvm
endef
define RESIDUALVM_CONTROLLERS_LINK
	ln -fs /tmp/gamecontrollerdb.txt $(TARGET_DIR)/usr/share/residualvm
endef

RESIDUALVM_POST_INSTALL_TARGET_HOOKS += RESIDUALVM_ADD_VIRTUAL_KEYBOARD
RESIDUALVM_POST_INSTALL_TARGET_HOOKS += RESIDUALVM_CONTROLLERS_LINK

$(eval $(autotools-package))
