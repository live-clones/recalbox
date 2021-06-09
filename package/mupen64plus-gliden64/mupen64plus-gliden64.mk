################################################################################
#
# mupen64plus video GLIDEN64
#
################################################################################

# commit of 26/05/2021
MUPEN64PLUS_GLIDEN64_VERSION = 15668b9064bbc0aca524d9a74ece98aaa9260908
MUPEN64PLUS_GLIDEN64_SITE = $(call github,gonetz,GLideN64,$(MUPEN64PLUS_GLIDEN64_VERSION))
MUPEN64PLUS_GLIDEN64_LICENSE = MIT
MUPEN64PLUS_GLIDEN64_DEPENDENCIES = sdl2 alsa-lib mupen64plus-core
MUPEN64PLUS_GLIDEN64_CONF_OPTS = -DMUPENPLUSAPI=On -DVEC4_OPT=On -DUSE_SYSTEM_LIBS=On -DUNIX=On
MUPEN64PLUS_GLIDEN64_SUBDIR = /src/

MUPEN64PLUS_GLIDEN64_CONF_OPTS += -DCMAKE_BUILD_TYPE=Release
MUPEN64PLUS_GLIDEN64_CONF_OPTS += -DCMAKE_FIND_ROOT_PATH=$(STAGING_DIR)

ifeq ($(BR2_PACKAGE_HAS_LIBGLES),y)
MUPEN64PLUS_GLIDEN64_DEPENDENCIES += libgles
MUPEN64PLUS_GLIDEN64_CONF_OPTS += -DGLES2=On
endif

ifeq ($(BR2_PACKAGE_HAS_LIBGL),y)
MUPEN64PLUS_GLIDEN64_DEPENDENCIES += libgl
endif

ifeq ($(BR2_PACKAGE_HAS_LIBEGL),y)
MUPEN64PLUS_GLIDEN64_DEPENDENCIES += libegl
MUPEN64PLUS_GLIDEN64_CONF_OPTS += -DEGL=On
endif

ifeq ($(BR2_PACKAGE_MESA3D),y)
MUPEN64PLUS_GLIDEN64_CONF_OPTS += -DMESA=On
endif

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI4)$(BR2_PACKAGE_RECALBOX_TARGET_ODROIDGO2),y)
MUPEN64PLUS_GLIDEN64_CONF_OPTS += -DCMAKE_C_FLAGS="-DEGL_NO_X11"
MUPEN64PLUS_GLIDEN64_CONF_OPTS += -DCMAKE_CXX_FLAGS="-DEGL_NO_X11"
endif

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_ODROIDXU4),y)
MUPEN64PLUS_GLIDEN64_CONF_OPTS += -DODROID=On
endif

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_X86)$(BR2_PACKAGE_RECALBOX_TARGET_X86_64),y)
MUPEN64PLUS_GLIDEN64_CONF_OPTS += -DX86_OPT=On
endif

ifeq ($(BR2_ARM_CPU_HAS_NEON),y)
MUPEN64PLUS_GLIDEN64_CONF_OPTS += -DNEON_OPT=ON
endif

ifeq ($(BR2_PACKAGE_SDL2_KMSDRM),y)
MUPEN64PLUS_GLIDEN64_CONF_OPTS += -DSDL=ON
endif

define MUPEN64PLUS_GLIDEN64_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/recalbox/share_init/system/configs/mupen64/hires_texture
	$(INSTALL) -D $(@D)/src/plugin/Release/mupen64plus-video-GLideN64.so \
		$(TARGET_DIR)/usr/lib/mupen64plus/mupen64plus-video-gliden64.so
	$(INSTALL) -D $(@D)/ini/* \
		$(TARGET_DIR)/recalbox/share_init/system/configs/mupen64/
endef

define MUPEN64PLUS_GLIDEN64_PRE_CONFIGURE_FIXUP
	chmod +x $(@D)/src/getRevision.sh
	sh $(@D)/src/getRevision.sh
	$(SED) 's|/opt/vc/include|/usr/include|g' $(@D)/src/CMakeLists.txt
	$(SED) 's|/opt/vc/lib|/usr/lib|g' $(@D)/src/CMakeLists.txt
endef

MUPEN64PLUS_GLIDEN64_PRE_CONFIGURE_HOOKS += MUPEN64PLUS_GLIDEN64_PRE_CONFIGURE_FIXUP

$(eval $(cmake-package))
