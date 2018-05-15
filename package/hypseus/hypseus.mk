################################################################################
#
# Hypseus
#
################################################################################

HYPSEUS_VERSION = c6af13b1274bbeb3e0406c84db6b9a5b3ce0bc72
HYPSEUS_REPO = btolab
HYPSEUS_SITE = $(call github,$(HYPSEUS_REPO),hypseus,$(HYPSEUS_VERSION))
HYPSEUS_LICENSE = GPL3
HYPSEUS_DEPENDENCIES = sdl2 sdl2_image sdl2_ttf zlib libogg libvorbis libmpeg2

# Create build directory and build in it
HYPSEUS_SUBDIR = build
HYPSEUS_CONF_OPTS = ../src -DBUILD_SHARED_LIBS=OFF

# Post-install: create link to configuration file
define HYPSEUS_CONFIG_LINK
        ln -fs /recalbox/share/system/configs/daphne/dapinput.ini $(TARGET_DIR)/usr/share/daphne
endef

HYPSEUS_POST_INSTALL_TARGET_HOOKS += HYPSEUS_CONFIG_LINK

$(eval $(cmake-package))
