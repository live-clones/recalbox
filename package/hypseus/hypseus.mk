################################################################################
#
# Hypseus
#
################################################################################

HYPSEUS_VERSION = 365c0e92d4015f2d504e0120f50354ab851f2782
HYPSEUS_REPO = btolab
HYPSEUS_SITE = $(call github,$(HYPSEUS_REPO),hypseus,$(HYPSEUS_VERSION))
HYPSEUS_LICENSE = GPL3
HYPSEUS_DEPENDENCIES = sdl2 sdl2_image sdl2_ttf zlib libogg libvorbis libmpeg2

# Create build directory and build in it
HYPSEUS_SUBDIR = build
HYPSEUS_CONF_OPTS = ../src -DBUILD_SHARED_LIBS=OFF

# Post-install: create script for easy launching + link to configuration file
define HYPSEUS_POST_INSTALL
	echo '#!/bin/bash' > $(TARGET_DIR)/usr/bin/hypseus
	echo 'cd /usr/share/hypseus; ./hypseus "$$@"' >> $(TARGET_DIR)/usr/bin/hypseus

        ln -fs /recalbox/share/system/configs/daphne/dapinput.ini $(TARGET_DIR)/usr/share/hypseus
endef

HYPSEUS_POST_INSTALL_TARGET_HOOKS += HYPSEUS_POST_INSTALL

$(eval $(cmake-package))
