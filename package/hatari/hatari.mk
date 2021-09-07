################################################################################
#
# Hatari emulator
#
################################################################################

HATARI_VERSION = de7a0e8ded3070fadbb359b1b9248b935f393db3
HATARI_SITE = https://github.com/hatari/hatari.git
HATARI_SITE_METHOD = git
HATARI_LICENSE = GPL2.0
HATARI_DEPENDENCIES = sdl2 readline zlib udev libcapsimage

ifeq ($(BR2_ENABLE_DEBUG),y)
HATARI_CONF_OPTS += -DCMAKE_BUILD_TYPE=Debug
else
HATARI_CONF_OPTS += -DCMAKE_BUILD_TYPE=Release
endif

HATARI_CONF_OPTS += -DBUILD_SHARED_LIBS=OFF
HATARI_CONF_OPTS += -DCMAKE_C_ARCHIVE_CREATE="<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>"
HATARI_CONF_OPTS += -DCMAKE_C_ARCHIVE_FINISH=true
HATARI_CONF_OPTS += -DCMAKE_AR="$(TARGET_CC)-ar"
HATARI_CONF_OPTS += -DCMAKE_C_COMPILER="$(TARGET_CC)"
HATARI_CONF_OPTS += -DCMAKE_CXX_COMPILER="$(TARGET_CXX)"
HATARI_CONF_OPTS += -DCMAKE_LINKER="$(TARGET_LD)"
HATARI_CONF_OPTS += -DCMAKE_C_FLAGS="$(COMPILER_COMMONS_CFLAGS_EXE)"
HATARI_CONF_OPTS += -DCMAKE_CXX_FLAGS="$(COMPILER_COMMONS_CXXFLAGS_EXE)"
HATARI_CONF_OPTS += -DCMAKE_LD_FLAGS="$(COMPILER_COMMONS_CXXFLAGS_EXE)"
HATARI_CONF_OPTS += -DCMAKE_EXE_LINKER_FLAGS="$(COMPILER_COMMONS_LDFLAGS_EXE)"
HATARI_CONF_OPTS += -DCAPSIMAGE_INCLUDE_DIR="($STAGING_DIR)/usr/include/caps"

define HATARI_INSTALL_TARGET_CMDS
	$(INSTALL) -D $(@D)/src/hatari $(TARGET_DIR)/usr/bin/hatari
	mkdir -p $(TARGET_DIR)/usr/share/hatari
	$(INSTALL) -D $(@D)/tools/hmsa/hmsa $(TARGET_DIR)/usr/share/hatari/hmsa
	$(INSTALL) -D $(@D)/tools/zip2st.sh $(TARGET_DIR)/usr/share/hatari/zip2st.sh
	$(INSTALL) -D $(@D)/tools/atari-hd-image.sh $(TARGET_DIR)/usr/share/hatari/atari-hd-image.sh
endef

$(eval $(cmake-package))
