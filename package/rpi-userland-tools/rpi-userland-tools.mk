################################################################################
#
# rpi-userland-tools
#
################################################################################

RPI_USERLAND_TOOLS_VERSION = 093b30bbc2fd083d68cc3ee07e6e555c6e592d11
RPI_USERLAND_TOOLS_SITE = $(call github,raspberrypi,userland,$(RPI_USERLAND_TOOLS_VERSION))
RPI_USERLAND_TOOLS_LICENSE = BSD-3-Clause
RPI_USERLAND_TOOLS_LICENSE_FILES = LICENCE
RPI_USERLAND_TOOLS_INSTALL_STAGING = YES
RPI_USERLAND_TOOLS_CONF_OPTS = \
	-DVMCS_INSTALL_PREFIX=/usr \
	-DALL_APPS=OFF \
	-DBUILD_MMAL=OFF \
	-DBUILD_MMAL_APPS=OFF

define RPI_USERLAND_TOOLS_POST_TARGET_CLEANUP
	rm -Rf $(TARGET_DIR)/usr/src
endef
RPI_USERLAND_TOOLS_POST_INSTALL_TARGET_HOOKS += RPI_USERLAND_TOOLS_POST_TARGET_CLEANUP

$(eval $(cmake-package))
