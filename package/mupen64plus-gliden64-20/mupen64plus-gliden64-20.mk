################################################################################
#
# mupen64plus video GLIDEN64
#
################################################################################

# commit of 19/10/2015
MUPEN64PLUS_GLIDEN64_20_VERSION = 34fa719a61fa2338025facc9810ad2ef1a00e7ee
MUPEN64PLUS_GLIDEN64_20_SITE = $(call github,gonetz,GLideN64,$(MUPEN64PLUS_GLIDEN64_20_VERSION))
MUPEN64PLUS_GLIDEN64_20_LICENSE = MIT
MUPEN64PLUS_GLIDEN64_20_DEPENDENCIES = sdl2 alsa-lib mupen64plus-core
MUPEN64PLUS_GLIDEN64_20_CONF_OPTS = -DMUPENPLUSAPI=On
MUPEN64PLUS_GLIDEN64_20_SUBDIR = /src/

ifeq ($(BR2_PACKAGE_HAS_LIBGLES),y)
MUPEN64PLUS_GLIDEN64_20_DEPENDENCIES += libgles
MUPEN64PLUS_GLIDEN64_20_CONF_OPTS += -DGLES2=On
endif

ifeq ($(BR2_PACKAGE_HAS_LIBEGL),y)
MUPEN64PLUS_GLIDEN64_20_DEPENDENCIES += libegl
endif

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI4)$(BR2_PACKAGE_RECALBOX_TARGET_RPI4_64)$(BR2_PACKAGE_RECALBOX_TARGET_ODROIDGO2),y)
MUPEN64PLUS_GLIDEN64_20_CONF_OPTS += -DCMAKE_C_FLAGS="-DEGL_NO_X11"
MUPEN64PLUS_GLIDEN64_20_CONF_OPTS += -DCMAKE_CXX_FLAGS="-DEGL_NO_X11"
endif

ifeq ($(BR2_PACKAGE_SDL2_KMSDRM),y)
MUPEN64PLUS_GLIDEN64_20_CONF_OPTS += -DSDL=ON
endif

define MUPEN64PLUS_GLIDEN64_20_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/recalbox/share_init/system/configs/mupen64/hires_texture
	$(INSTALL) -D $(@D)/src/plugin/release/mupen64plus-video-GLideN64.so \
		$(TARGET_DIR)/usr/lib/mupen64plus/mupen64plus-video-gliden64_20.so
	$(INSTALL) -D $(@D)/ini/* \
		$(TARGET_DIR)/recalbox/share_init/system/configs/mupen64/
endef

define MUPEN64PLUS_GLIDEN64_20_PRE_CONFIGURE_FIXUP
	chmod +x $(@D)/src/getRevision.sh
	sh $(@D)/src/getRevision.sh
	$(SED) 's|/opt/vc/include|$(STAGING_DIR)/usr/include|g' $(@D)/src/CMakeLists.txt
	$(SED) 's|/opt/vc/lib|$(STAGING_DIR)/usr/lib|g' $(@D)/src/CMakeLists.txt
endef

MUPEN64PLUS_GLIDEN64_20_PRE_CONFIGURE_HOOKS += MUPEN64PLUS_GLIDEN64_20_PRE_CONFIGURE_FIXUP

$(eval $(cmake-package))
