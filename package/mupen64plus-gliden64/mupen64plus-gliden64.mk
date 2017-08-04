################################################################################
#
# mupen64plus video GLIDEN64
#
################################################################################


#Public_Release_2_0
MUPEN64PLUS_GLIDEN64_VERSION = 72872963b8981e39dec66cfac77324f126c79e80
MUPEN64PLUS_GLIDEN64_SITE = $(call github,gonetz,GLideN64,$(MUPEN64PLUS_GLIDEN64_VERSION))
MUPEN64PLUS_GLIDEN64_LICENSE = MIT
MUPEN64PLUS_GLIDEN64_DEPENDENCIES = sdl2 alsa-lib mupen64plus-core
MUPEN64PLUS_GLIDEN64_CONF_OPTS = -DMUPENPLUSAPI=On
MUPEN64PLUS_GLIDEN64_SUBDIR = /src/

ifeq ($(BR2_PACKAGE_RPI_USERLAND),y)
	MUPEN64PLUS_GLIDEN64_DEPENDENCIES += rpi-userland
	MUPEN64PLUS_GLIDEN64_CONF_OPTS += -DGLES2=On -DVEC4_OPT=On -DNEON_OPT=On
endif

define MUPEN64PLUS_GLIDEN64_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/recalbox/configs/mupen64/
	mkdir -p $(TARGET_DIR)/usr/share/mupen64plus/
	$(INSTALL) -D $(@D)/src/plugin/release/mupen64plus-video-GLideN64.so \
		$(TARGET_DIR)/usr/lib/mupen64plus/mupen64plus-video-gliden64.so
	$(INSTALL) -D $(@D)/ini/* \
		$(TARGET_DIR)/usr/share/mupen64plus/
	$(INSTALL) -D $(@D)/ini/* \
		$(TARGET_DIR)/recalbox/configs/mupen64/
endef

define MUPEN64PLUS_GLIDEN64_PRE_CONFIGURE_FIXUP
	chmod +x $(@D)/src/getRevision.sh
	sh $(@D)/src/getRevision.sh
	$(SED) 's|/opt/vc/include|$(STAGING_DIR)/usr/include|g' $(@D)/src/CMakeLists.txt
	$(SED) 's|/opt/vc/lib|$(STAGING_DIR)/usr/lib|g' $(@D)/src/CMakeLists.txt
	#must be removed during the next bump and use -DUSE_SYSTEM_LIBS=On option (not available in the public release 2.0)
	$(SED) 's|elseif(BCMHOST)|elseif(UNIX OR BCMHOST)|g' $(@D)/src/GLideNHQ/CMakeLists.txt

endef

MUPEN64PLUS_GLIDEN64_PRE_CONFIGURE_HOOKS += MUPEN64PLUS_GLIDEN64_PRE_CONFIGURE_FIXUP


$(eval $(cmake-package))


