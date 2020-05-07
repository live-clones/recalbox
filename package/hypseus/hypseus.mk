################################################################################
#
# Hypseus
#
################################################################################

HYPSEUS_VERSION = 41fc33edaa8273cbf1ad807b57d8c2a7ae143351
HYPSEUS_SITE = $(call github,btolab,hypseus,$(HYPSEUS_VERSION))
HYPSEUS_LICENSE = GPL3
HYPSEUS_DEPENDENCIES = sdl2 sdl2_image sdl2_ttf zlib libogg libvorbis libmpeg2

# Create build directory and build in it
HYPSEUS_SUBDIR = build
HYPSEUS_CONF_OPTS = ../src -DBUILD_SHARED_LIBS=OFF

# Post-install: create link to configuration file
define HYPSEUS_CONFIG_LINK
	ln -fs /recalbox/share/system/configs/daphne/dapinput.ini $(TARGET_DIR)/usr/share/daphne/hypinput.ini
endef

HYPSEUS_POST_INSTALL_TARGET_HOOKS += HYPSEUS_CONFIG_LINK

HYPSEUS_CONF_OPTS += -DCMAKE_C_ARCHIVE_CREATE="<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>"
HYPSEUS_CONF_OPTS += -DCMAKE_C_ARCHIVE_FINISH=true
HYPSEUS_CONF_OPTS += -DCMAKE_CXX_ARCHIVE_CREATE="<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>"
HYPSEUS_CONF_OPTS += -DCMAKE_CXX_ARCHIVE_FINISH=true
HYPSEUS_CONF_OPTS += -DCMAKE_AR="$(TARGET_CC)-ar"
HYPSEUS_CONF_OPTS += -DCMAKE_C_COMPILER="$(TARGET_CC)"
HYPSEUS_CONF_OPTS += -DCMAKE_CXX_COMPILER="$(TARGET_CXX)"
HYPSEUS_CONF_OPTS += -DCMAKE_LINKER="$(TARGET_LD)"
HYPSEUS_CONF_OPTS += -DCMAKE_C_FLAGS="$(COMPILER_COMMONS_CFLAGS_EXE)"
HYPSEUS_CONF_OPTS += -DCMAKE_CXX_FLAGS="$(COMPILER_COMMONS_CXXFLAGS_EXE)"
HYPSEUS_CONF_OPTS += -DCMAKE_LINKER_EXE_FLAGS="$(COMPILER_COMMONS_LDFLAGS_EXE)"

$(eval $(cmake-package))
