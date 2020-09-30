################################################################################
#
# EmulationStation 2 - recalbox version https://gitlab.com/recalbox/recalbox-emulationstation
#
################################################################################

RECALBOX_EMULATIONSTATION2_VERSION = 4c13b301fd4b491cac642b04542ab9e96faa1e2e
RECALBOX_EMULATIONSTATION2_SITE = https://gitlab.com/recalbox/recalbox-emulationstation.git
RECALBOX_EMULATIONSTATION2_SITE_METHOD = git
RECALBOX_EMULATIONSTATION2_LICENSE = MIT
RECALBOX_EMULATIONSTATION2_DEPENDENCIES = sdl2 sdl2_mixer freeimage freetype alsa-lib \
	libcurl openssl libzip ffmpeg

ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI3),y)
RECALBOX_EMULATIONSTATION2_CONF_OPTS = -DRPI_VERSION=3
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI2),y)
RECALBOX_EMULATIONSTATION2_CONF_OPTS = -DRPI_VERSION=2
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI1),y)
RECALBOX_EMULATIONSTATION2_CONF_OPTS = -DRPI_VERSION=1
else ifeq ($(BR2_PACKAGE_RECALBOX_TARGET_RPI0),y)
RECALBOX_EMULATIONSTATION2_CONF_OPTS = -DRPI_VERSION=1
endif

ifeq ($(BR2_PACKAGE_HAS_LIBGL),y)
RECALBOX_EMULATIONSTATION2_DEPENDENCIES += libgl
endif

ifeq ($(BR2_PACKAGE_HAS_LIBGLES),y)
RECALBOX_EMULATIONSTATION2_DEPENDENCIES += libgles
endif

define RECALBOX_EMULATIONSTATION2_RPI_FIXUP
	$(SED) 's|/opt/vc/include|$(STAGING_DIR)/usr/include|g' $(@D)/CMakeLists.txt
	$(SED) 's|/opt/vc/lib|$(STAGING_DIR)/usr/lib|g' $(@D)/CMakeLists.txt
	$(SED) 's|/usr/lib|$(STAGING_DIR)/usr/lib|g' $(@D)/CMakeLists.txt
endef

RECALBOX_EMULATIONSTATION2_PRE_CONFIGURE_HOOKS += RECALBOX_EMULATIONSTATION2_RPI_FIXUP

ifeq ($(BR2_ENABLE_DEBUG),y)
RECALBOX_EMULATIONSTATION2_CONF_OPTS += -DCMAKE_BUILD_TYPE=Debug
endif

RECALBOX_EMULATIONSTATION2_CONF_OPTS += -DCMAKE_C_ARCHIVE_CREATE="<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>"
RECALBOX_EMULATIONSTATION2_CONF_OPTS += -DCMAKE_C_ARCHIVE_FINISH=true
RECALBOX_EMULATIONSTATION2_CONF_OPTS += -DCMAKE_CXX_ARCHIVE_CREATE="<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>"
RECALBOX_EMULATIONSTATION2_CONF_OPTS += -DCMAKE_CXX_ARCHIVE_FINISH=true
RECALBOX_EMULATIONSTATION2_CONF_OPTS += -DCMAKE_AR="$(TARGET_CC)-ar"
RECALBOX_EMULATIONSTATION2_CONF_OPTS += -DCMAKE_C_COMPILER="$(TARGET_CC)"
RECALBOX_EMULATIONSTATION2_CONF_OPTS += -DCMAKE_CXX_COMPILER="$(TARGET_CXX)"
RECALBOX_EMULATIONSTATION2_CONF_OPTS += -DCMAKE_LINKER="$(TARGET_LD)"
RECALBOX_EMULATIONSTATION2_CONF_OPTS += -DCMAKE_C_FLAGS="$(COMPILER_COMMONS_CFLAGS_EXE)"
RECALBOX_EMULATIONSTATION2_CONF_OPTS += -DCMAKE_CXX_FLAGS="$(COMPILER_COMMONS_CXXFLAGS_EXE)"
RECALBOX_EMULATIONSTATION2_CONF_OPTS += -DCMAKE_LINKER_EXE_FLAGS="$(COMPILER_COMMONS_LDFLAGS_EXE)"
RECALBOX_EMULATIONSTATION2_CONF_OPTS += -DOPTION_RECALBOX_PRODUCTION_BUILD=true

$(eval $(cmake-package))
